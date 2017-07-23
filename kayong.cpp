#include <stdio.h>
#include <time.h>

FILE * serial;

bool initSerial()
{
	serial = fopen("/dev/ttyACM0","w");
	if ( serial ){
		puts("Serial opened!\n");
		return true;
	}
	else{
		puts("Error : failed to initialize serial!");
		return false;
	}
}

void displayByte( char byte )
{
	for ( int i = 0; i < 8; i++ )
		putchar( 48+ ( ( byte >> i ) & 1 ) );
}

void sendNote( char note )
{
	fputc( note, serial );
	fflush( serial );
	fputs( "Byte sent : ", stdout );
	displayByte( note );
	putchar('\n');
}

/*
void toggleNote( int pin )
{
	notes = notes ^ ( 1 << pin );
	sendNote(notes);
}
*/

FILE * melody;

int main(int argc,  char** argv)
{
	puts("Kayong Player Initiated!\n");
	if ( !initSerial() )
		return 404;

	if ( argc > 1 )
		puts( argv[1] );
	else{
		puts( "No input file passed!" );
	}

	melody = fopen( argv[1], "r" );
	int melodyLength;
	//TODO

	return 0;
}

