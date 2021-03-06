#include "libs/MPL3115A2_Altimeter.h"
#include "libs/LSM6DS3_IMU.h"
#include "libs/Packet.h"
#include <TinyGPS++.h>
#include <stdarg.h>

#define LOG_SD 1
#define TX_XBEE 1

#if LOG_SD
#include <SD.h>
#endif

// Useful pin numbers (see 64-pin-avr pins_arduino.h in MegaCore)
#define PIN_GPS_WAKE 23
#define PIN_RF_SLEEP 24
#define PIN_GPS_FIX 22
#define PIN_BAT_LEVEL 45
#define PIN_SD_CHIP_SELECT 12

#if LOG_SD
const String SD_LOG_FILE = "log.csv";
#endif

void TimerInit() {
  ASSR &= ~0x20; // Use IO Clock
  
  TCCR2A = 0x00; // Normal Mode
  TCCR2B = 0x06; // Overflows at 152.58789 Hz
  
  TIMSK2 = 0x01; // Only Overflow Interrupts
}

void USART1Init() { // Manually configure USART1 so we can use interrupts to feed TinyGPS
  UBRR1 = 64; // Set baud rate to 9600 ((10MHz/(16*9600Hz)-1 ~= 64)
  UCSR1B = 0x98; // Enable receive interrupts, receiver, and transmitter
  // Other initial values are okay (8-bit 1-stop no parity)
  UCSR1B |= 0x20; //Enable data ready interupts (start transmission of cold reset
}

bool gps_flag, accel_flag, alt_flag, bat_flag;
TinyGPSPlus gps;
Packet packetizer(&Serial);

#if LOG_SD
File loggerFile;
#endif

ISR(TIMER2_OVF_vect) { // Runs every 6.5536 ms

  static uint8_t gps_counter=0; //Might be unecessary, could use TinyGPS isUpdated() on relevant streams
  if(++gps_counter >= 153){ // Set every 1.0027 s
    gps_flag=1;
    gps_counter=0;
  }

  static uint8_t accel_counter=0;
  if(++accel_counter >= 2){ // Set every 13.1072 ms (Based on 104Hz rate selection)
    accel_flag=1;
    accel_counter=0;
  }

  static uint8_t alt_counter=0;
  if(++alt_counter >= 20){ // Set every 131.072 ms (Just over 130 ms at 32x Oversampling)
    alt_flag=1;
    alt_counter=0;
  }

  static uint16_t bat_counter=0;
  if(++bat_counter >= 1526) { // Set every 10s
    bat_flag=1;
    bat_counter=0;
  }
}

ISR(USART1_RX_vect) { // Feed GPS UART data to parser
  char data = UDR1;
  gps.encode(data);
}
ISR(USART1_UDRE_vect) { // Send GPS cold restart at power up
  const static char PMTK_CMD[] = "$PMTK104*37\r\n";
  static uint16_t transmit_char = 0;
  if(PMTK_CMD[transmit_char] != '\0'){
    UDR1 = PMTK_CMD[transmit_char++]; //Send char
  } else {
    transmit_char = 0;
    UCSR1B &= ~0x20; //Disable data reg empty interrupts
  }
}


void setup() {
  // put your setup code here, to run once:
  
  noInterrupts(); //At least UART inititialization said to run with interrupts disabled, we don't really care about interrupts here anyway
  
  TimerInit();
  USART1Init();
  Serial.begin(38400);

  pinMode(PIN_GPS_WAKE, OUTPUT);
  pinMode(PIN_RF_SLEEP, OUTPUT);
  pinMode(PIN_GPS_FIX, INPUT);

  digitalWrite(PIN_GPS_WAKE, HIGH); // GPS Awake
  digitalWrite(PIN_RF_SLEEP, LOW);  // xBee not sleeping

  pinMode(PIN_BAT_LEVEL, INPUT);
  analogReference(INTERNAL2V56); //Internal reference such that bat level is read in mV

  // Fallback if Arduino style doesn't work
  // DDRD |= 0x70;
  // PORTD |= 0x20; //GPS Awake
  // PORTD &= ~0x50; //xBee not sleep, no pullup on FIX
  

  interrupts();

  I2c.timeOut(100);
  I2c.pullup(false);
  I2c.begin();

  //I2C Bus Check, could be removed, or set to disable missing or malfunctioning devices
  bool alt_ready = MPL::CheckDevicePresent();
  bool accel_ready = LSM::CheckDevicePresent();
  if(!(alt_ready && accel_ready)){
    Serial.println("I2C Device(s) not working!!!!!");
    Serial.print("Altimeter:    \t");
    Serial.println(alt_ready ? "PASS" : "FAIL");
    Serial.print("Accelerometer:\t");
    Serial.println(accel_ready ? "PASS" : "FAIL");
    while(true);
  } else {
    MPL::Init();
    LSM::Init();
  }

#if LOG_SD
  // see if the card is present and can be initialized:
  if (!SD.begin(PIN_SD_CHIP_SELECT)) {
    Serial.print("SD error code ");
    Serial.println(SD.card.errorCode());
    while(true);
  }
  else {
//    SD.remove(SD_LOG_FILE); // Delete and recreate instead of append
    loggerFile = SD.open(SD_LOG_FILE, FILE_WRITE);
    if(!loggerFile){
      Serial.print("SD failed to open ");
      Serial.println(SD_LOG_FILE);
      while(true);
    }
    Serial.print("SD Card Initialized. Logging to ");
    Serial.println(SD_LOG_FILE);
  }
#endif
}

#if LOG_SD
void logSDEvent(char type, uint32_t t, unsigned int count, ...){
  loggerFile.write(type);
  loggerFile.write(',');
  loggerFile.print(t);
  va_list vals;
  va_start(vals, count);
  for (unsigned int i = 0; i < count; ++i) {
    loggerFile.write(',');
    // This needs to be read as a double because apparently (...) implies type promotion?
    loggerFile.print(va_arg(vals, double));
  }
  va_end(vals);
  loggerFile.write('\n');
}
#endif

void loop() {
  if(gps_flag){
    float lat = gps.location.lat();
    float lng = gps.location.lng();
    uint32_t t = millis();
    #if LOG_SD
      logSDEvent('1',t,2,lat, lng);
      loggerFile.flush();
    #endif
    packetizer.sendGPS(t,lat,lng);
    gps_flag=0;
  }
  if(accel_flag){
    LSM::AccelGyroData dat = LSM::CheckAndRead();
    uint32_t t = millis();
    float x_mpsps = dat.accel.x*LSM::ACCEL_TO_MPSPS;
    float y_mpsps = dat.accel.y*LSM::ACCEL_TO_MPSPS;
    float z_mpsps = dat.accel.z*LSM::ACCEL_TO_MPSPS;
    float x_dps = dat.gyro.x*LSM::GYRO_TO_DPS;
    float y_dps = dat.gyro.y*LSM::GYRO_TO_DPS;
    float z_dps = dat.gyro.z*LSM::GYRO_TO_DPS;

    #if LOG_SD
      logSDEvent('3',t,6,x_mpsps,y_mpsps,z_mpsps,x_dps,y_dps,z_dps);
    #endif
    
    packetizer.sendAccel(t,dat.accel.x,dat.accel.y,dat.accel.z,dat.gyro.x,dat.gyro.y,dat.gyro.z);
    accel_flag=0;
  }
  if(alt_flag){
    MPL::AltTempData dat = MPL::CheckAndRead();
    uint32_t t = millis();
    float alt_m = dat.alt*MPL::ALT_TO_M;
    float temp_c = dat.temp*MPL::TEMP_TO_C;

    #if LOG_SD
      logSDEvent('2',t,2,alt_m,temp_c);
    #endif
    
    packetizer.sendAltitude(t,dat.alt, dat.temp);
    MPL::RequestData();
    alt_flag=0;
  }
  if(bat_flag) {
    uint16_t centivolts = analogRead(PIN_BAT_LEVEL);
    uint32_t t = millis();
    packetizer.sendBattery(t,centivolts);

    #if LOG_SD
      logSDEvent('4',t,1,centivolts/100.0);
    #endif
    bat_flag=0;
  }
}
