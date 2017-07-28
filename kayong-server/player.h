#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

//#define OUTPUT_FILE "/dev/ttyACM0"
#define OUTPUT_FILE "fileoutput"

bool initPlayer();
bool setSong ( char * filename );
int playSong ();

#endif // PLAYER_H_INCLUDED
