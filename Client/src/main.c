#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <error.h>

#define UNIX_PATH_MAX 108
#define SOCKNAME "./mysockmomo"

int main()
{
	int socketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	bind(socketDescriptor, (struct sockaddr*) &address, sizeof(address));
	connect(socketDescriptor, (struct sockaddr*) &address, sizeof(address));

	write(socketDescriptor, "Ciao", 5);
	close(socketDescriptor);
	return 0;
}