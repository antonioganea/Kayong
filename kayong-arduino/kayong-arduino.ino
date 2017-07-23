#define C1 0
#define CS1 1
#define D1 2
#define DS1 3
#define E1 4
#define F1 5
#define FS1 6
#define G1 7
#define GS1 8
#define A2 9
#define AS2 10
#define B2 11
#define C2 12
#define CS2 13
#define D2 14
#define DS2 15
#define E2 16
#define F2 17
#define FS2 18
#define G2 19
#define GS2 20
#define A3 21
#define AS3 22
#define B3 23
#define C3 24
#define CS3 25
#define D3 26
#define DS3 27

#include <Servo.h>

const int notes[] =
{
  0b11111111, // C1
  0b11111110, // CSharp1
  0b11111110, // D1
  0b11111100, // DSharp1
  0b11111100, // E1
  0b11111011, // F1
  0b11111010, // FSharp1
  0b11110000, // G1
  0b11101110, // GSharp1
  0b11100000, // A2
  0b11011010, // ASharp2
  0b11000000, // B2
  0b10100000, // C2
  0b01100000, // CSharp2
  0b00100000, // D2
  0b00111110, // DSharp2
};

Servo myservo[9];

//int pos = 0;    // variable to store the servo position

void setup() {
  pinMode ( 22, OUTPUT );
  //digitalWrite( 13, HIGH );
  Serial.begin(9600);
  
  for ( int i = 0; i < 9; i++ )
   myservo[i].attach(i+2);

/*
  while ( true ){
    for ( int i = 0; i <= DS2; i++ )
    {
      setNote ( i );
      delay(500);
    }
  }
*/
}

void setNote ( int note )
{
  for ( int i = 7; i >= 0; i-- )
  {
    myservo[i].write ( 180 * ( ( note >> (7-i) ) & 1 ) );
    
    //Serial.write( 65 + ( ( note >> (7-i) ) & 1 ) );
    //Serial.write("\n");
    /*
    Serial.write( "Note " );
    Serial.write( 65 + i );
    Serial.write( " " );
    Serial.write( 65 + ( (  notes[note] >> (7-i) ) & 1 ) );
    Serial.write("\n");
    */
  }
}

void setVolume ( bool on )
{
  //TODO
}

int inByte;

void loop() {
  //Serial.write( "a" );
  //delay(3000);
  
  
  inByte = Serial.read();

  if ( inByte != -1 )
  {
    digitalWrite( 22, HIGH );
    delay(100);
    digitalWrite( 22, LOW );

    setNote ( inByte );
    
    /*
    if ( inByte = 0 )
      //setVolume( false );
    else
    {
      //setVolume( true );
      setNote( inByte-1 );
    }*/
  }
  delay(100);

/*
  Serial.write( "HELLO\n" );
  delay(5000);
*/
  
  /*
  for ( int i = 0; i < 9; i++ )
  {
    myservo[i].write( 0 );
    delay(100);
  }
  delay(300);
  for ( int i = 8; i >= 0; i-- )
  {
    myservo[i].write( 180 );
    delay(100);
  }
  delay(300);
  */
/*
  for ( int i = 0; i < 9; i++ )
    myservo[i].write( 0 );
  delay(5000);
  for ( int i = 0; i < 9; i++ )
    myservo[i].write( 180 );
  delay(5000);
*/
}
