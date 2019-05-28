#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <error.h>

#define CHECK(x) if (x == -1) perror("Errore client");

#define UNIX_PATH_MAX 108
#define SOCKNAME "socket"

int connectToServer() 
{
	int socketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	while (connect(socketDescriptor, (struct sockaddr*) & address, sizeof(address)) == -1) 
	{
		printf("Server non disponibile, nuovo tentativo tra 1 secondo \n");
		sleep(1);
	}

	return socketDescriptor;
}

int main()
{
	int socketDescriptor = connectToServer();

	write(socketDescriptor, "Ciao", 5);
	close(socketDescriptor);
	return 0;
}
