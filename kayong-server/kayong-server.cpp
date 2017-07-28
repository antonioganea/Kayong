/*	Author : Antonio Alexandru Ganea
	July 2017, Server for the Kayong Robotic Flute Project
*/
#include "player.h" // Include player module
#include <SFML/Network.hpp> // Used for TcpSockets
#include <stdio.h>
using namespace std;

#define LISTENER_PORT 56000 // Server port
#define BUFFER_SIZE 256 // Data buffer maximum size

// Used for receiving data :
char data[BUFFER_SIZE];
std::size_t received;

bool socketAvailable; // Boolean flag used for loop breaking; allows only 1 connection at a time

sf::TcpListener listener; // Used for accepting TCP connections

int main(){
	puts("Kayong-Server started!");

	if (listener.listen(LISTENER_PORT) != sf::Socket::Done){ // Bind the listener to a port
		printf("Port %d is occupied / Error : Cannot bind listener to port %d\n", LISTENER_PORT, LISTENER_PORT );
		return 10048;
	}
	printf("Listener was bound to port %d !\n", LISTENER_PORT );

	initPlayer(); // Initialize player module ( for note output )

	while ( true ){
		puts("Awaiting connection...\n");
		sf::TcpSocket client; // client socket
		if (listener.accept( client ) != sf::Socket::Done){ // Accept a new connection
			puts("Socket tried to connect but failed!");
		}
		else{ // If there are no errors accepting a connection
			printf("Socket connected from %s\n", client.getRemoteAddress().toString().c_str() );
			socketAvailable = true; // Loop-breaking flag
			while ( socketAvailable ){
				sf::Socket::Status status = client.receive(data, BUFFER_SIZE, received);
				switch( status ){
					case sf::Socket::Done:
						printf( "Received a message : %lu bytes!\n", received );
						data[received] = 0; // Add null character at the end to finish the string
						setSong(data); // Update song
						client.setBlocking(false); // Disable blocking mode to play the song correctly
						break;
					case sf::Socket::Disconnected:
						puts( "Socket Disconected!");
						socketAvailable = false; // Release socket for other user
						break;
					default: // This is used to prevent compilation warnings
						break;
				}
				if ( !playSong() ){ // In case the song is finished ..
					client.setBlocking(true); // .. enter blocking mode
				}
			}
			client.setBlocking(true); // Return into blocking mode
		}
	}
	return 0;
}
