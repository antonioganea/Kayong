/*	Author : Antonio Alexandru Ganea
	July 2017, Server for the Kayong Robotic Flute Project
*/
#include <SFML/Network.hpp>
#include <stdio.h>
using namespace std;

#define LISTENER_PORT 56000
#define BUFFER_SIZE 256

// Used for receiving data :
char data[BUFFER_SIZE];
std::size_t received;

bool socketAvailable;

int main(){
	puts("Kayong-Server started!");

	sf::TcpListener listener;
	if (listener.listen(LISTENER_PORT) != sf::Socket::Done){ // bind the listener to a port
		printf("Port %d is occupied / Error : Cannot bind listener to port %d\n", LISTENER_PORT, LISTENER_PORT );
		return 10048;
	}
	printf("Listener was bound to port %d !\n", LISTENER_PORT );

	while ( true )
	{
		puts("Awaiting connection...\n");
		sf::TcpSocket client;
		if (listener.accept( client ) != sf::Socket::Done){ // accept a new connection
			puts("Socket tried to connect but failed!");
		}
		else{
			printf("Socket connected from %s\n", client.getRemoteAddress().toString().c_str() );
			socketAvailable = true;
			while ( socketAvailable ){
				sf::Socket::Status status = client.receive(data, BUFFER_SIZE, received);
				switch( status ){
					case sf::Socket::Done:
						printf( "Received a message : %lu bytes!\n", received );
						break;
					case sf::Socket::Disconnected:
						puts( "Socket Disconected!");
						socketAvailable = false;
						break;
					default:
						break;
				}
			}
		}
	}

	return 0;
}
