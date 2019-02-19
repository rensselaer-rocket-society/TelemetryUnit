void TimerInit() {
  ASSR &= ~0x20; // Use IO Clock
  
  TCCR2A = 0x00; // Normal Mode
  TCCR2B = 0x06; // Overflows at 152.58789 Hz
  
  TIMSK2 = 0x01; // Only Overflow Interrupts
}

bool gps_flag, accel_flag, alt_flag;

ISR(TIMER2_OVF_vect) { // Runs every 6.5536 ms

  static byte gps_counter=0;
  if(++gps_counter >= 153){ // Set every 1.0027 s
    gps_flag=1;
    gps_counter=0;
  }

  static byte accel_counter=0;
  if(++accel_counter >= 15){ // Set every 98.304 ms
    accel_flag=1;
    accel_counter=0;
  }

  static byte alt_counter=0;
  if(++alt_counter >= 15){ // Set every 98.304 ms
    alt_flag=1;
    alt_counter=0;
  }
}


void setup() {
  // put your setup code here, to run once:
  TimerInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(gps_flag){
    printf("GPS\r\n");
    gps_flag=0;
  }
  if(accel_flag){
    printf("Accelerometer\r\n");
    accel_flag=0;
  }
  if(alt_flag){
    printf("Altimeter\r\n");
    alt_flag=0;
  }
}
