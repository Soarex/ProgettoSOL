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

#define CHECK(x) if (x == -1) perror("Error");

int main()
{
	int socketListen = socket(AF_UNIX, SOCK_STREAM, 0);
	CHECK(socketListen);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	CHECK(bind(socketListen, (struct sockaddr*) &address, sizeof(address)));
	CHECK(listen(socketListen, SOMAXCONN));

	int socketClient = accept(socketListen, NULL, 0);
	CHECK(socketClient);

	char buffer[100];
	read(socketClient, buffer, 100);

	printf("%s", buffer);
	close(socketClient);
	close(socketListen);
    return 0;
}