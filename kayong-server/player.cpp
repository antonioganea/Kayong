/*	Author : Antonio Alexandru Ganea
	July 2017, Note Player for the Kayong Robotic Flute Project
*/
#include "player.h"
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <SFML/System/Clock.hpp>

FILE * serial;

/** Open filestream defined in the player.h file */
bool initSerial(){
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

/** Break down a byte into bits and print them*/
void displayByte( char byte ){
	for ( int i = 0; i < 8; i++ )
		putchar( 48+ ( ( byte >> i ) & 1 ) );
}

/** Send byte to serial */
void sendNote( char note ){
	fputc( note, serial ); // Writes a byte on the filestream
	fflush( serial ); // Flushes byte from internal buffers to serial
	fputs( "Byte sent : ", stdout );
	displayByte( note );
	putchar('\n');
}

char * memblock; // Buffer for the song
bool loaded = false; // Flag indicating wheter or not a song is loaded

/** Loads song file into memory */
void loadFromFile( const char * fullPath ){
	if ( loaded )
		delete[] memblock; // Delete last memory block, if any
    std::streampos size;
    std::ifstream file ( fullPath, std::ios::in|std::ios::binary|std::ios::ate );
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size]; // Allocate new memory block
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
	return initSerial();
}


sf::Clock songClock;
unsigned int melodyLength, currentNote;
bool setSong( char * fileName ){
	printf( "Player setting song to %s!\n", fileName );

	// Set variables for playback :
	loadFromFile(fileName);
	currentNote = 0;
	memcpy( &melodyLength, memblock, 4 );
	songClock.restart();

	return true;
}


int duration = 0;
/** Non-blocking playback function */
int playSong(){
	char config; // Byte storing current configuration to be sent to the serial port
	if ( currentNote < melodyLength ){ // If there are still notes to be played
        if ( songClock.getElapsedTime().asMilliseconds() >= duration ){ // If enough time passed, set new note
			memcpy( &config, memblock+4+currentNote*5, 1 );
			memcpy( &duration, memblock+5+currentNote*5, 4);
			sendNote( config );
			printf("Note duration:%*d\n",6,duration);
			songClock.restart();
			currentNote++;
        }
    }

	return melodyLength - currentNote; // Return remaining notes
}
