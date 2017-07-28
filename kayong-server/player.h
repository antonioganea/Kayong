#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

//#define OUTPUT_FILE "/dev/ttyACM0"
#define OUTPUT_FILE "fileoutput"

/** Opens filestream ( serial ) */
bool initPlayer();
/** Sets the currently playing song */
bool setSong ( char * filename );
/** Checks if time passed and sends the correct note to the filestream
	@return remaining notes
 */
int playSong ();

#endif // PLAYER_H_INCLUDED
