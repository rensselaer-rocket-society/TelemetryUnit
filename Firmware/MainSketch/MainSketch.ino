#include "libs/MPL3115A2_Altimeter.h"
#include "libs/LSM6DS3_IMU.h"
#include <TinyGPS++.h>

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
  if(++alt_counter >= 15){ // Set every 98.304 ms
    alt_flag=1;
    alt_counter=0;
  }
}

ISR(USART1_RX_vect) { // Feed GPS UART data to parser
  char data = UDR1;
  gps.encode(data);
  Serial.write(data);
}


void setup() {
  // put your setup code here, to run once:
  
  noInterrupts(); //At least UART inititialization said to run with interrupts disabled, we don't really care about interrupts here anyway
  
  TimerInit();
  USART1Init();
  Serial.begin(9600);

  I2c.timeOut(100);
  I2c.pullup(false);
  I2c.begin();

  MPL::Init();
  LSM::Init();
  
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(gps_flag){
    printf("GPS\r\n");
    gps_flag=0;
  }
  if(accel_flag){
    LSM::AccelGyroData dat = LSM::CheckAndRead();
    printf("Accelerometer\r\n");
    accel_flag=0;
  }
  if(alt_flag){
    MPL::AltTempData dat = MPL::CheckAndRead();
    printf("Altimeter:\t%lu\t%u\r\n", dat.alt, dat.temp);
    alt_flag=0;
  }
}
