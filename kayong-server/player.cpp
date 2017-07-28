#include "player.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <SFML/System/Clock.hpp>

FILE * serial;

bool initSerial()
{
	//serial = fopen("/dev/ttyACM0","w");
	//serial = fopen("fileoutput.txt","w");
	serial = fopen(OUTPUT_FILE,"w");
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
bool loaded = false;

void loadFromFile( const char * fullPath ){
	if ( loaded )
		delete[] memblock;
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
        loaded = true;
    }
    else{
        puts( "Unable to open melody file!");
    }
}

// Public functions below

bool initPlayer(){
	if ( !initSerial() )
		return false;
	return true;
}


sf::Clock songClock;
unsigned int melodyLength, currentNote;
bool setSong( char * fileName ){
	printf( "Player setting song to %s!\n", fileName );

	loadFromFile(fileName);
	currentNote = 0;
	memcpy( &melodyLength, memblock, 4 );
	songClock.restart();

	return true;
}


int duration = 0;
int playSong(){
	char notes;
	if ( currentNote < melodyLength ){
        if ( songClock.getElapsedTime().asMilliseconds() >= duration ){
			memcpy( &notes, memblock+4+currentNote*5, 1 );
			memcpy( &duration, memblock+5+currentNote*5, 4);
			sendNote( notes );
			printf("Note duration:%*d\n",6,duration);
			songClock.restart();
			currentNote++;
        }
    }

	return melodyLength - currentNote;
}
