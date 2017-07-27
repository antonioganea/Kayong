#ifndef NOTES_H_INCLUDED
#define NOTES_H_INCLUDED

const char noteNames[][4]={
"OFF",
"C 1",
"CS1",
"D 1",
"DS1",
"E 1",
"F 1",
"FS1",
"G 1",
"GS1",
"A 2",
"AS2",
"B 2",
"C 2",
"CS2",
"D 2",
"DS2",
"E 2",
"F 2",
"FS2",
"G 2",
"GS2",
"A 3",
"AS3",
"B 3",
"C 3",
"CS3",
"D 3",
"DS3"
};

#define NOTE_NAMES_COUNT 29

const unsigned char noteValues[] =
{
  0b00000000, // AIR OFF
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
  //"E2",
  //"F2",
  //"FS2",
  //"G2",
  //"GS2",
  //"A3",
  //"AS3",
  //"B3",
  //"C3",
  //"CS3",
  //"D3",
  //"DS3"
};

#define NOTE_VALUES_COUNT 17

/** Returns note number from configuration. Warning : not an injective function */
unsigned int getNoteFromConfiguration( unsigned char config ){
	for ( int i = 0; i < NOTE_VALUES_COUNT; i++ ){
		if ( noteValues[i] == config )
			return i;
	}
	return 0;
}

#endif // NOTES_H_INCLUDED
