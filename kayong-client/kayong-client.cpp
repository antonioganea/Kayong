/*	Author : Antonio Alexandru Ganea
	July 2017, Test client for the Kayong Robotic Flute Project
*/
#include <SFML/Network.hpp>
#include <stdio.h>
#include <string.h>

#define LISTENER_PORT 56000

char ip[16]; // xxx.xxx.xxx.xxx + null character
char buffer[256];

int main(){
	puts("Kayong-Client started!");
	sf::TcpSocket socket;

	while( true ){
		printf( "Enter the ip you want to connect to : " );
		scanf( "%s", ip );

		printf( "Attempting to connect to %s ....\n", ip );


		sf::Socket::Status status = socket.connect( ip, LISTENER_PORT );
		if (status != sf::Socket::Done){
			printf("Failed to connect to ip %s !\n", ip );
		}
		else break;
	}

	printf( "Connected to %s !\n", ip );

	while( true ){
		printf( "Type the melody name : " );
		scanf( "%s", buffer );
		socket.send ( buffer, strlen(buffer) );
		puts( "Message sent!" );
	}

	return 0;
}
