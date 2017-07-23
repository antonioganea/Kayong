#include <fstream>
#include <stdio.h>
#include <string.h>
#include <SFML/System/Clock.hpp>

FILE * serial;

bool initSerial()
{
	serial = fopen("/dev/ttyACM0","w");
	//serial = fopen("fileoutput.txt","w");
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

char * memblock;

void loadFromFile( const char * fullPath ){
    std::streampos size;
    std::ifstream file ( fullPath, std::ios::in|std::ios::binary|std::ios::ate );
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        file.close();

        printf( "The entire melody file content is in memory ( %d bytes ).\n", (int)size );
    }
    else{
        puts( "Unable to open melody file!");
    }
}


FILE * melody;

int main(int argc,  char** argv)
{
    sf::Clock clock;

	puts("Kayong Player Booted!\n");
	if ( !initSerial() )
		return 404;

	if ( argc > 1 )
		puts( argv[1] );
	else{
		puts( "No input file passed!" );
	}

	loadFromFile(argv[1]);
	int melodyLength, currentNote = 0;
	int streampos = 4;
	memcpy( &melodyLength, memblock, 4 );

	char notes;
	int duration;//in miliseconds

	while ( currentNote < melodyLength )
    {
        memcpy( &notes, memblock+4+currentNote*5, 1 );
        memcpy( &duration, memblock+5+currentNote*5, 4);
        sendNote( notes );
        printf("Note duration : %d\n",duration);
        clock.restart();
        while ( clock.getElapsedTime().asMilliseconds() <= duration ) {} // block process until going to the next note
        currentNote++;
    }

    puts("Song finished!");
	return 0;
}

