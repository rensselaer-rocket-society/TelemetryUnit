#include "inc/pin_defs.h"
#include "inc/MPL3115A2_Altimeter.h"
#include "inc/LSM6DS3_IMU.h"
#include "inc/KMX63_Accel_Mag.h"
#include "inc/Packet.h"
#include <TinyGPS++.h>
#include <stdarg.h>

#define LOG_SD 0

#if LOG_SD
#include <SD.h>
#endif

void accelMagISR();
void imuISR();
void altimeterISR();


/***************************************************************************************************


                                       Global Variables


****************************************************************************************************/

bool imu_flag, accel_flag, alt_flag, bat_flag, gps_flag;
TinyGPSPlus gps;
Packet packetizer(&Serial);

#if LOG_SD
const String SD_LOG_FILE = "log.csv";
File loggerFile;
#endif


/***************************************************************************************************


                                       Initialization


****************************************************************************************************/

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
  // UCSR1B |= 0x20; //Enable data ready interupts (start transmission of cold reset)
}

void setup() {
  // put your setup code here, to run once:
  
  noInterrupts(); //At least UART inititialization said to run with interrupts disabled, we don't really care about interrupts here anyway
  
  TimerInit();
  USART1Init();
  Serial.begin(115200);

  pinMode(PIN_GPS_WAKE, OUTPUT);
  pinMode(PIN_RF_SLP, OUTPUT);
  pinMode(PIN_FIX, INPUT);

  digitalWrite(PIN_GPS_WAKE, HIGH); // GPS Awake
  digitalWrite(PIN_RF_SLP, LOW);  // xBee not sleeping

  pinMode(PIN_V_BAT, INPUT);
  analogReference(INTERNAL2V56); //Internal reference such that bat level is read in cV

  attachInterrupt(digitalPinToInterrupt(PIN_ACCEL_INT),accelMagISR,FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_IMU_INT),imuISR,RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_ALT_INT),altimeterISR,RISING);

  interrupts();

  I2c.timeOut(100);
  I2c.pullup(false);
  I2c.begin();

  //I2C Bus Check, could be removed, or set to disable missing or malfunctioning devices
  bool alt_ready = MPL::CheckDevicePresent();
  bool imu_ready = LSM::CheckDevicePresent();
  bool accelmag_ready = KMX::CheckDevicePresent();
  if(!(alt_ready && imu_ready)){
    Serial.println("I2C Device(s) not working!!!!!");
    Serial.print("Altimeter:    \t");
    Serial.println(alt_ready ? "PASS" : "FAIL");
    Serial.print("IMU:\t");
    Serial.println(imu_ready ? "PASS" : "FAIL");
    Serial.print("Accel/Mag:\t");
    Serial.println(accelmag_ready ? "PASS" : "FAIL");
    while(true);
  } else {
    MPL::Init();
    LSM::Init();
    KMX::Init();
    // Force first read to make sure interrupts are cleared and will be detected
    imu_flag = 1;
    accel_flag = 1;
    alt_flag = 1;
  }

#if LOG_SD
  // see if the card is present and can be initialized:
  if (!SD.begin(PIN_CS_SD)) {
    Serial.print("SD error code ");
//    Serial.println(SD.card.errorCode());
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


/***************************************************************************************************


                                       Program Logic


****************************************************************************************************/

//******************************
//        Interrupts
//******************************

ISR(TIMER2_OVF_vect) { // Runs every 6.5536 ms

  static uint8_t gps_counter=0; //Might be unecessary, could use TinyGPS isUpdated() on relevant streams
  if(++gps_counter >= 153){ // Set every 1.0027 s
    gps_flag=1;
    gps_counter=0;
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
// ISR(USART1_UDRE_vect) { // Send GPS cold restart at power up
//   const static char PMTK_CMD[] = "$PMTK104*37\r\n";
//   static uint16_t transmit_char = 0;
//   if(PMTK_CMD[transmit_char] != '\0'){
//     UDR1 = PMTK_CMD[transmit_char++]; //Send char
//   } else {
//     transmit_char = 0;
//     UCSR1B &= ~0x20; //Disable data reg empty interrupts
//   }
// }

void accelMagISR()
{
  accel_flag = 1;
}
void imuISR()
{
  imu_flag = 1;
}
void altimeterISR()
{
  alt_flag = 1;
}

//******************************
//        Main Loop
//******************************

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
  }
  if(imu_flag){
    // Interrupt sourced, no need to check status
    LSM::AccelGyroData dat = LSM::ReadData();
    uint32_t t = millis();

    #if LOG_SD
      float x_mpsps = dat.accel.x*LSM::ACCEL_TO_MPSPS;
      float y_mpsps = dat.accel.y*LSM::ACCEL_TO_MPSPS;
      float z_mpsps = dat.accel.z*LSM::ACCEL_TO_MPSPS;
      float x_dps = dat.gyro.x*LSM::GYRO_TO_DPS;
      float y_dps = dat.gyro.y*LSM::GYRO_TO_DPS;
      float z_dps = dat.gyro.z*LSM::GYRO_TO_DPS;

      logSDEvent('3',t,6,x_mpsps,y_mpsps,z_mpsps,x_dps,y_dps,z_dps);
    #endif
    
    packetizer.sendImu(t,dat.accel.x,dat.accel.y,dat.accel.z,dat.gyro.x,dat.gyro.y,dat.gyro.z);
    imu_flag=0;
  }
  if(accel_flag){
    // Interrupt sourced, no need to check status
    KMX::AccelMagData dat = KMX::ReadData();
    uint32_t t = millis();

    #if LOG_SD
      float x_mpsps = dat.accel.x*KMX::ACCEL_TO_MPSPS;
      float y_mpsps = dat.accel.y*KMX::ACCEL_TO_MPSPS;
      float z_mpsps = dat.accel.z*KMX::ACCEL_TO_MPSPS;
      float x_mag = dat.magneto.x*KMX::MAG_TO_UTSLA;
      float y_mag = dat.magneto.y*KMX::MAG_TO_UTSLA;
      float z_mag = dat.magneto.z*KMX::MAG_TO_UTSLA;

      logSDEvent('5',t,6,x_mpsps,y_mpsps,z_mpsps,x_mag,y_mag,z_mag);
    #endif
      
    packetizer.sendAccelMag(t,dat.accel.x,dat.accel.y,dat.accel.z,dat.magneto.x,dat.magneto.y,dat.magneto.z);
    accel_flag=0;
  }
  if(alt_flag){
    MPL::AltTempData dat;
    if(MPL::CheckAndRead(&dat)) {
      uint32_t t = millis();
      #if LOG_SD
        float alt_m = dat.alt*MPL::ALT_TO_M;
        float temp_c = dat.temp*MPL::TEMP_TO_C;

        logSDEvent('2',t,2,alt_m,temp_c);
      #endif
      
      packetizer.sendAltitude(t,dat.alt, dat.temp);
    }
    MPL::RequestData(); // ensure data is requested
    alt_flag=0;
  }
  if(bat_flag) {
    uint16_t centivolts = analogRead(PIN_V_BAT);
    uint32_t t = millis();
    packetizer.sendBattery(t,centivolts);

    #if LOG_SD
      logSDEvent('4',t,1,centivolts/100.0);
    #endif
    bat_flag=0;
  }
}
