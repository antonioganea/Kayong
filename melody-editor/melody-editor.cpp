/*	Author : Antonio Alexandru Ganea
	July 2017, Binary Melody Editor for the Kayong Robotic Flute Project
*/

#include <ncurses.h>
#include <string.h>
#include <vector>
#include <fstream>

#include "Notes.h"

WINDOW * stats;
WINDOW * editor;

unsigned int WIDTH, HEIGHT; // Terminal Width and Height in cells
unsigned int ENTRIES_PER_PAGE; // Number of notes per page

unsigned int TOTAL_DURATION = 0; // Total song duration in miliseconds
const int DURATION_STEP = 100; // The change applied to a note's duration when certain keys are pressed

unsigned int currentPage;
unsigned int currentRow;

struct Entry{
	unsigned char notes;
	unsigned int duration;
};
std::vector<Entry*> entries;

/** Decomposes a byte into bits and draws them onscreen */
void drawBits( unsigned int entry ){
	unsigned char config = noteValues[ entries.at(entry)->notes ];
	for ( int i = 7; i >= 0; i-- ){
		waddch( editor, '0' + ( ( config >> i ) & 1 ) );
	}
}

/** Draws a specific entry on a specific row */
void drawEntry( unsigned int row, unsigned int entry ){
	if ( entry >= entries.size() )
		return;

	if ( row == currentRow ) // Highlight currentRow
		wattron(editor,COLOR_PAIR(2) | A_BOLD );

	wmove(editor,row+1,1);
	wprintw(editor, "%-*d", 8, entry ); // Write entry number

	waddstr(editor," | "); // Spacing
	wprintw(editor, "%s", noteNames[entries.at(entry)->notes] ); // Write note name

	waddstr(editor," | "); // Spacing
	drawBits( entry ); // Write note configuration

	waddstr(editor," | "); // Spacing
	wprintw(editor,"%*d",8,entries.at(entry)->duration); // Write note duration

	wattrset(editor,A_NORMAL);
}

/** Removes a specific entry from the song */
void deleteEntry( unsigned int entry ){
	TOTAL_DURATION -= entries.at(entry)->duration;
	delete entries.at(entry);
	entries.erase ( entries.begin()+entry );
}

/** Updates editor view */
void drawPage(){
	wclear(editor);
	box(editor,0,0); // Draw border

	for ( unsigned int i = 0; i < ENTRIES_PER_PAGE; i++ ) // For each entry ..
		drawEntry(i,i + ENTRIES_PER_PAGE * currentPage );

	//Draw page number in bottom right corner
	mvwprintw( editor, HEIGHT-4, WIDTH-13, " Page : %*d ", 3, currentPage );
	wrefresh(editor); // Push buffer to screen
}

char filePath[256]; // Stores the current file's path

/** Updates stats view */
void drawStats(){
	box(stats, 0, 0); // Draw border

	wattron( stats, COLOR_PAIR(3) ); // Set color pair 3
	mvwaddstr(stats,0,(WIDTH-strlen(filePath))/2,filePath); // Draw title ( filename )
	wattrset( stats, A_NORMAL);

	wattron(stats,COLOR_PAIR(1)); // Set color pair 1
	mvwaddstr(stats,1,1,"File Statistics : ");
	wattrset(stats,A_NORMAL);

	//Draw song statistics
	wprintw(stats,"%*d notes, %*d miliseconds", 8, entries.size(), 8, TOTAL_DURATION );
	wrefresh(stats); // Push buffer to screen
}

/** Loads the file into memory and parses the data, adding entries to the vector */
void loadFromFile( char * fullPath ){
	strcpy( filePath, fullPath ); // Store the filename for future use ( saving )
	char * memblock; // Pointer to the reader buffer
    std::streampos size; // Used for reading
    std::ifstream file ( fullPath, std::ios::in|std::ios::binary|std::ios::ate ); // Open file in binary reading mode
    if (file.is_open())
    {
        size = file.tellg(); // Count filesize in bytes
        memblock = new char[size]; // Allocate buffer size accordingly
        file.seekg (0, std::ios::beg); // Reposition reading cursor to 0
        file.read (memblock, size); // Read 'size' bytes into the buffer
        file.close(); // Dealocate file structure memory

		//Following variables are used for reading
        unsigned int n_notes, duration; // n_notes = number of notes, duration = specific note duration
        unsigned char notes; // = byte handling specific note configuration ( holes )

        memcpy(&n_notes,memblock,4); // Read number of notes from the file header

        for ( unsigned int pos = 0; pos < n_notes; pos++ ){ // For each note in the file
			memcpy( &notes, memblock+4+pos*5, 1 ); // Read configuration
			memcpy( &duration, memblock+5+pos*5, 4 ); // Read duration

			// Create new entry and push it in the vector
			Entry * newEntry = new Entry;
			newEntry->notes = getNoteFromConfiguration( notes );
			newEntry->duration = duration;
			TOTAL_DURATION += duration;
			entries.push_back(newEntry);
        }
    }
}

/** Saves the song to the same file path it was loaded from */
void saveMelody(){
	FILE * fout; // File output pointer
	fout = fopen( filePath, "wb" ); // Open file in binary writing mode
	int size = entries.size();
	fwrite( &size, sizeof(int), 1, fout ); // Write file header ( note number )
	for( std::vector<Entry*>::iterator it = entries.begin(); it != entries.end(); it++ ){ // For every note
		fwrite( &(noteValues[(*it)->notes]), sizeof(char), 1, fout ); // Write configuration ..
		fwrite( &((*it)->duration), sizeof(int), 1, fout ); // .. and duration
	}
	mvwaddstr(editor,HEIGHT-4,1,"FILE SAVED"); // Display a message in the bottom left corner
	wrefresh(editor);

	fclose(fout);
}

/** Usage : melody-editor <filename> */
int main(int argc, char *argv[])
{
	initscr(); // Start curses mode
	start_color(); // Initialize color mode
	noecho(); // Turn of echoing
	curs_set(0); // Hide Curosr
	cbreak(); // Line buffering disabled
	keypad(stdscr, true); // Enable special keys ( for up and down arrow keys )

	WIDTH = getmaxx(stdscr); // Screen width
	HEIGHT = getmaxy(stdscr); // Screen height

	ENTRIES_PER_PAGE = HEIGHT - 5; // Number of notes per page


	// Define some color sets
	init_pair(1, COLOR_CYAN, COLOR_BLACK );
	init_pair(2, COLOR_YELLOW, COLOR_BLACK );
	init_pair(3, COLOR_GREEN, COLOR_BLACK );

	refresh(); // Refresh the window for the first time so drawing is enabled correctly

	stats = newwin( 3,WIDTH, 0, 0 ); // Init stats window ( upper part )
	//drawStats();

	editor = newwin( HEIGHT-3, WIDTH, 3, 0 ); // Init editor window ( lower part )

	// The first command-line argument should be the file name
	if ( argc == 2 ){
		// Load the file into the Entries vector and update screen
		loadFromFile( argv[1] );
		drawStats();
		drawPage();
	}
	else{
		endwin(); // Exit curses mode
		printf("ERROR : cannot open file! Usage : melody-editor <filename>\n");
		return 404; // File not found.
	}

	int ch; // Reader character
	while((ch = getch()) != 27) // 27 = ESCAPE
	{
		unsigned int currentEntry = currentPage*ENTRIES_PER_PAGE+currentRow;
		switch(ch){
			case KEY_UP: // Try to switch currentRow to the upper row
				if ( currentRow > 0 )
					currentRow--;
				else{
					if ( currentPage > 0 ){
					currentPage--; // Switch to the previous page
					currentRow = ENTRIES_PER_PAGE-1; // Update row position
					}
				}
				drawPage(); // Update editor view
				break;
			case KEY_DOWN: // Try to switch currentRow to next row
				if ( currentRow + ENTRIES_PER_PAGE*currentPage < entries.size()-1 ){ // Prevent overflow
					if ( currentRow < ENTRIES_PER_PAGE-1 ) // Stay in the same page
						currentRow++;
					else{ // Switch to previous page
						currentPage++;
						currentRow=0;
					}
				}
				drawPage();
				break;
			case 'c': // Switch to previous page
				if ( currentPage > 0 )
					currentPage--;
				drawPage();
				break;
			case 'v': // Switch to next page
				if ( currentPage < entries.size()/ENTRIES_PER_PAGE ){ // Prevent page overflow
					currentPage++;
					currentRow = ( ( entries.size()-1 ) % ENTRIES_PER_PAGE < currentRow ) ? ( entries.size()-1 ) % ENTRIES_PER_PAGE : currentRow; // Prevent currentRow overflow
					drawPage();
				}
				break;
			case 'a':{ // Decrement current entry duration
				Entry * entry = entries.at(currentEntry);
				if ( entry->duration >= DURATION_STEP ){ // Prevent underflow
					entry->duration -= DURATION_STEP;
					TOTAL_DURATION -= DURATION_STEP;
				}
				else{
					TOTAL_DURATION -= entry->duration;
					entry->duration = 0;
				}

				// Update screen
				drawStats();
				drawEntry( currentRow, currentEntry ); // Update row
				wrefresh( editor ); // Push buffer to console;
				break;
				}
			case 's': // Increment current entry duration
				entries.at(currentEntry)->duration += DURATION_STEP;
				TOTAL_DURATION += DURATION_STEP;
				// Update screen
				drawStats();
				drawEntry( currentRow, currentEntry ); // Update row
				wrefresh( editor ); // Push buffer to console;
				break;
			case 'n':{ // Create new entry
				Entry * newEntry = new Entry;
				newEntry->notes = 0;
				newEntry->duration = 0;
				unsigned int pos = currentEntry+1; // Calculate position ( immediately after the currently selected entry )
				if ( pos > entries.size() ) // Prevent overflow
					pos = entries.size();
				entries.insert( entries.begin() + pos , newEntry); // Add entry to the list
				// Update screen
				drawPage();
				drawStats();
				break;
				}
			case 'd': // Delete current entry
				deleteEntry( currentEntry );
				// Update screen
				drawStats();
				drawPage();
				break;
			case 'x':{ // Increment note
				Entry * entry = entries.at(currentEntry);
				if ( ++entry->notes >= NOTE_VALUES_COUNT )
					entry->notes = 0;
				drawEntry( currentRow, currentEntry ); // Update row
				wrefresh( editor ); // Push buffer to console;
				break;
				}
			case 'z':{ // Increment note
				Entry * entry = entries.at(currentEntry);
				if ( entry->notes == 0 )
					entry->notes = NOTE_VALUES_COUNT-1;
				else
					entry->notes--;
				drawEntry( currentRow, currentEntry ); // Update row
				wrefresh( editor ); // Push buffer to console;
				break;
				}
			case 'o': // Save song to file
                saveMelody();
                break;
		}
	}
	delwin(stats); // Dealocate memory for the stats window
	delwin(editor); // Dealocate memory for the editor window
	endwin(); // End curses mode
	return 0;
}
