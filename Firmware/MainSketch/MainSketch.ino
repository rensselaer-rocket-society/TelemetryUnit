#include "libs/MPL3115A2_Altimeter.h"
#include "libs/LSM6DS3_IMU.h"
#include <TinyGPS++.h>

// Useful pin numbers (see 64-pin-avr pins_arduino.h in MegaCore)
#define PIN_GPS_WAKE 23
#define PIN_RF_SLEEP 24
#define PIN_GPS_FIX 22
#define PIN_BAT_LEVEL 45


void TimerInit() {
  ASSR &= ~0x20; // Use IO Clock
  
  TCCR2A = 0x00; // Normal Mode
  TCCR2B = 0x06; // Overflows at 152.58789 Hz
  
  TIMSK2 = 0x01; // Only Overflow Interrupts
}

void USART1Init() { // Manually configure USART1 so we can use interrupts to feed TinyGPS
  UBRR1 = 64; // Set baud rate to 9600 ((10MHz/(16*9600Hz)-1 ~= 64)
  UCSR1B = 0x90; // Enable reciever w/ interrupts only for now, we currently don't send commands to GPS
  // Other initial values are okay (8-bit 1-stop no parity)
}

bool gps_flag, accel_flag, alt_flag;
TinyGPSPlus gps;

ISR(TIMER2_OVF_vect) { // Runs every 6.5536 ms

  static byte gps_counter=0; //Might be unecessary, could use TinyGPS isUpdated() on relevant streams
  if(++gps_counter >= 153){ // Set every 1.0027 s
    gps_flag=1;
    gps_counter=0;
  }

  static byte accel_counter=0;
  if(++accel_counter >= 2){ // Set every 13.1072 ms (Based on 104Hz rate selection)
    accel_flag=1;
    accel_counter=0;
  }

  static byte alt_counter=0;
  if(++alt_counter >= 20){ // Set every 131.072 ms (Just over 130 ms at 32x Oversampling)
    alt_flag=1;
    alt_counter=0;
  }
}

ISR(USART1_RX_vect) { // Feed GPS UART data to parser
  char data = UDR1;
  gps.encode(data);
  Serial.print(data);
}


void setup() {
  // put your setup code here, to run once:
  
  noInterrupts(); //At least UART inititialization said to run with interrupts disabled, we don't really care about interrupts here anyway
  
  TimerInit();
  USART1Init();
  Serial.begin(9600);

  pinMode(PIN_GPS_WAKE, OUTPUT);
  pinMode(PIN_RF_SLEEP, OUTPUT);
  pinMode(PIN_GPS_FIX, INPUT);
  pinMode(PIN_BAT_LEVEL, INPUT);

  digitalWrite(PIN_GPS_WAKE, HIGH); // GPS Awake
  digitalWrite(PIN_RF_SLEEP, LOW);  // xBee not sleeping

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
}

void loop() {
  if(gps_flag){
    // Serial.println();
    // Serial.print(gps.location.lat());
    // Serial.print(",");
    // Serial.print(gps.location.lng());
    // Serial.println();
    // Serial.println();
    gps_flag=0;
  }
  if(accel_flag){
    LSM::AccelGyroData dat = LSM::CheckAndRead();
//    char outstring[100];
//    sprintf(outstring, "%d,%d,%d,%d,%d,%d\r\n",dat.accel.x,dat.accel.y,dat.accel.z,dat.gyro.x,dat.gyro.y,dat.gyro.z);
//    Serial.print(outstring);
    accel_flag=0;
  }
  if(alt_flag){
    MPL::AltTempData dat = MPL::CheckAndRead();
//    Serial.println(dat.alt/16.0);
    MPL::RequestData();
    alt_flag=0;
  }
}
