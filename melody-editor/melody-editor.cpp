#include <ncurses.h>
#include <string.h>
#include <vector>
#include <fstream>

//TODO : use Unsigned Int insteat of int ( duration + notes )

char noteNames[][4]={
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

unsigned int getNoteFromConfiguration( unsigned char config ){
	for ( int i = 0; i < NOTE_VALUES_COUNT; i++ ){
		if ( noteValues[i] == config )
			return i;
	}
	return 0;
}



unsigned int WIDTH, HEIGHT;
unsigned int ENTRIES_PER_PAGE;

unsigned int currentPage;
unsigned int currentRow;
unsigned int currentColumn;

unsigned int TOTAL_DURATION = 0;

//int entryCount;

WINDOW * stats;
WINDOW * editor;

struct Entry{
	unsigned char notes;
	unsigned int duration;
};

//Entry entries[1024];

unsigned int min( unsigned int a, unsigned int b ){
	if ( a < b )
		return a;
	else
		return b;
}

std::vector<Entry*> entries;

void drawBits( unsigned int entry ){
	unsigned char config = noteValues[ entries.at(entry)->notes ];
	for ( int i = 7; i >= 0; i-- ){
		//if ( ( entries[entry].notes >> i ) & 1 )
		waddch( editor, '0' + ( ( config >> i ) & 1 ) );
	}
}

void drawEntry( unsigned int row, unsigned int entry ){
	if ( entry >= entries.size() )
		return;

	if ( row == currentRow )
		wattron(editor,COLOR_PAIR(2) | A_BOLD );

	wmove(editor,row+1,1);
	wprintw(editor, "%-*d", 8, entry );

	waddstr(editor," | ");
	wprintw(editor, "%s", noteNames[entries.at(entry)->notes] );

	waddstr(editor," | ");
	drawBits( entry );

	waddstr(editor," | ");
	wprintw(editor,"%*d",8,entries.at(entry)->duration);

	wattrset(editor,A_NORMAL);
}

void deleteEntry( unsigned int entry ){
	TOTAL_DURATION -= entries.at(entry)->duration;
	delete entries.at(entry);
	entries.erase ( entries.begin()+entry );
}

void drawPage(){
	wclear(editor);
	box(editor,0,0);

	for ( int i = 0; i < ENTRIES_PER_PAGE; i++ )
		drawEntry(i,i + ENTRIES_PER_PAGE * currentPage );

	mvwprintw( editor, HEIGHT-4, WIDTH-13, " Page : %*d ", 3, currentPage );

	wrefresh(editor);
}

char filePath[256];

void drawStats(){
	box(stats, 0, 0);

	mvwaddstr(stats,0,(WIDTH-strlen(filePath))/2,filePath);

	wattron(stats,COLOR_PAIR(2));
	mvwaddstr(stats,1,1,"File Statistics : ");
	wattrset(stats,A_NORMAL);

	wprintw(stats,"%*d notes, %*d miliseconds", 8, entries.size(), 8, TOTAL_DURATION );

	wrefresh(stats);
}

char * memblock;

void loadFromFile( char * fullPath ){
	strcpy( filePath, fullPath );
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

        unsigned int n_notes;
        unsigned char notes;
        unsigned int duration;
        memcpy(&n_notes,memblock,4);
        unsigned int pos;

        for ( pos = 0; pos < n_notes; pos++ ){
			memcpy( &notes, memblock+4+pos*5, 1 );
			memcpy( &duration, memblock+5+pos*5, 4 );

			Entry * newEntry = new Entry;
			newEntry->notes = getNoteFromConfiguration( notes );
			newEntry->duration = duration;
			TOTAL_DURATION += duration;
			entries.push_back(newEntry);
        }



        drawStats();
    }
    else{
        puts( "Unable to open melody file!");
    }
}

void saveMelody(){
	FILE * fout;
	fout = fopen( filePath, "wb" );
	int size = entries.size();
	fwrite( &size, sizeof(int), 1, fout );
	for( std::vector<Entry*>::iterator it = entries.begin(); it != entries.end(); it++ ){
		fwrite( &(noteValues[(*it)->notes]), sizeof(char), 1, fout );
		fwrite( &((*it)->duration), sizeof(int), 1, fout );
	}
	mvwaddstr(editor,HEIGHT-4,1,"FILE SAVED");
	wrefresh(editor);

	fclose(fout);
}

int DURATION_STEP = 100;

int main(int argc, char *argv[])
{
/*
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);

	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
*/


	initscr();			/* Start curses mode 		*/
	start_color();
	noecho();
	curs_set(0);
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, true);		/* I need that nifty F1 	*/

	WIDTH = getmaxx(stdscr);
	HEIGHT = getmaxy(stdscr);

	ENTRIES_PER_PAGE = HEIGHT - 5;

	//deleteEntry(0);
	//entryCount = 10;

	init_pair(1, COLOR_CYAN, COLOR_BLACK );
	init_pair(2, COLOR_YELLOW, COLOR_BLACK );
	init_pair(3, COLOR_GREEN, COLOR_BLACK );

	//printw("HELLO");
	refresh();

	stats = newwin( 3,WIDTH, 0, 0 );
	drawStats();

	editor = newwin( HEIGHT-3, WIDTH, 3, 0 );

	if ( argc == 2 )
		loadFromFile( argv[1] );
	else{
		endwin();
		printf("ERROR : cannot open file! Usage : editor <filename>\n");
		return 404;
	}

	drawPage();

	int ch;
	while((ch = getch()) != 27)
	{	switch(ch)
		{	case KEY_LEFT:
				//destroy_win(my_win);
				//my_win = create_newwin(height, width, starty,--startx);
				break;
			case KEY_RIGHT:
				//destroy_win(my_win);
				//my_win = create_newwin(height, width, starty,++startx);
				break;
			case KEY_UP:
				if ( currentRow > 0 ){
					currentRow--;
				}
				else{
					if ( currentPage > 0 ){
					currentRow = ENTRIES_PER_PAGE-1;
					currentPage--;
					}
				}
				drawPage();
				break;
			case KEY_DOWN:
				if ( currentRow + ENTRIES_PER_PAGE*currentPage < entries.size()-1 ){
					if ( currentRow < ENTRIES_PER_PAGE-1 )
						currentRow++;
					else{
						currentPage++;
						currentRow=0;
					}
				}
				drawPage();
				break;
			case 'c':
				if ( currentPage > 0 )
					currentPage--;
				drawPage();
				break;
			case 'v':
				if ( currentPage < entries.size()/ENTRIES_PER_PAGE ){
					currentPage++;
					currentRow = min( ( entries.size()-1 )%ENTRIES_PER_PAGE, currentRow );
					drawPage();
				}
				break;
			case 'a':{
				int init = entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration;
				entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration -= DURATION_STEP;
				if ( entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration < 0 )
					entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration = 0;
				TOTAL_DURATION -= init - entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration;
				drawStats();
				drawPage(); // TODO : DRAW ENTRY INSTEAD OF PAGE
				break;
				}
			case 's':
				entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->duration += DURATION_STEP;
				TOTAL_DURATION+=DURATION_STEP;
				drawStats();
				drawPage(); // TODO : DRAW ENTRY INSTEAD OF PAGE
				break;
			case 'n':{
				Entry * newEntry = new Entry;
				newEntry->notes = 0;
				newEntry->duration = 0;
				//TOTAL_DURATION += duration;
				int pos = currentPage*ENTRIES_PER_PAGE+currentRow+1;
				if ( pos > entries.size() )
					pos = entries.size();
				entries.insert( entries.begin() + ( pos ) , newEntry);
				drawPage();
				drawStats();
				break;
				}
			case 'd':
				deleteEntry( currentPage*ENTRIES_PER_PAGE+currentRow );
				drawStats();
				drawPage();
				break;
			case 'x'://increment note
				entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes++;
				if ( entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes >= NOTE_VALUES_COUNT )
					entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes = 0;
				drawPage();// TOOD : DRAW ENTRY INSTEAD OF PAGE
				break;
			case 'z'://increment note
				entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes--;
				if ( entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes < 0 )
					entries.at(currentPage*ENTRIES_PER_PAGE+currentRow)->notes = NOTE_VALUES_COUNT-1;
				drawPage();// TOOD : DRAW ENTRY INSTEAD OF PAGE
				break;
			case 'o':
                saveMelody();
                break;
		}
	}
	delwin(stats);
	delwin(editor);
	endwin();			/* End curses mode		  */
	return 0;
}
