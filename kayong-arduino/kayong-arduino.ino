#include <Servo.h>
#define AIR_CONTROL_SERVO 8

Servo servos[9];

void setup() {
  Serial.begin(9600);
  
  for ( int i = 0; i < 9; i++ )
    servos[i].attach(i+2);
}

void setNote ( int note ){
  if ( note == 0 ){
    servos[AIR_CONTROL_SERVO].write ( 180 );
  }
  else{
    servos[AIR_CONTROL_SERVO].write ( 0 );
    for ( int i = 0; i < 8; i-- ){
      servos[i].write ( 180 * ( ( note >> (7-i) ) & 1 ) );
    }
  }
}

int inByte;
void loop() {
  inByte = Serial.read();
  if ( inByte != -1 ){
    setNote ( inByte );
  }
  delay(1);
}
