#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <error.h>
#include <stdlib.h>

#define UNIX_PATH_MAX 108
#define SOCKNAME "./socket"

#define CHECK(x) if (x == -1) perror("Error");

int initServer();
int listenForClients(int socket);
void* processClient(void* client);

int main()
{
	system("rm socket");

	int serverSocket = initServer();
	listenForClients(serverSocket);

	close(serverSocket);
	system("rm socket");
    return 0;
}

int initServer() 
{
	int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	CHECK(serverSocket);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	printf("Binding\n");
	CHECK(bind(serverSocket, (struct sockaddr*) & address, sizeof(address)));

	return serverSocket;
}

int listenForClients(int serverSocket)
{
	int clientSocket;
	pthread_t thread;
	while (1)
	{
		CHECK(listen(serverSocket, SOMAXCONN));
		clientSocket = accept(serverSocket, NULL, 0);
		CHECK(serverSocket);
		printf("Client accettato\n");

		CHECK(pthread_create(&thread, NULL, &processClient, (void*)clientSocket));
	}

	return clientSocket;
}

void* processClient(void* client)
{
	int clientSocket = (int)client;

	char buffer[100];
	ssize_t n;
	while ((n = read(clientSocket, buffer, 100)) != 0)
	{
		CHECK(n);
		buffer[n] = '\0';
		printf("%s\n", buffer);
	}

	close(clientSocket);

	pthread_exit(0);
}