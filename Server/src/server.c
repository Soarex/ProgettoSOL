#include "server.h"

int main()
{
	int serverSocket = initServer();
	listenForClients(serverSocket);

	close(serverSocket);
	unlink(SOCKNAME);
	return 0;
}

int initServer()
{
	int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	CHECK(serverSocket);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	if(bind(serverSocket, (struct sockaddr*) & address, sizeof(address)) == -1) {
		unlink(SOCKNAME);
		CHECK(bind(serverSocket, (struct sockaddr*) & address, sizeof(address)));
	}

	CHECK(listen(serverSocket, SOMAXCONN));

	return serverSocket;
}

int listenForClients(int serverSocket)
{
	int clientSocket;
	pthread_t thread;
	while (1)
	{
		LOG("In ascolto", MESSAGE)
		clientSocket = accept(serverSocket, NULL, 0);
		CHECK(clientSocket);
		LOG("Client accettato", MESSAGE);

		CHECK(pthread_create(&thread, NULL, &processClient, (void*)clientSocket));
	}

	return clientSocket;
}

void* processClient(void* client)
{
	int clientSocket = (int)client;

	char buffer[500];// , message[150];
	//ssize_t n; 
	int status = 1;

	while (status == 1)
	{
		read(clientSocket, buffer, 499);
		//buffer[n] = '\0';
		status = processMessage(buffer);
	}

	close(clientSocket);

	pthread_exit(0);
}

int processMessage(char* message) {
	char *command, *name, *length, *data, *terminator;

	if (message == NULL) {
		LOG("Messaggio nullo, chiusura connessione", ERROR);
		return 0;
	}


	command = strtok(message, " ");
	if (command == NULL) {
		LOG("Formato messaggio non riconosciuto, chiusura connessione", ERROR);
		return 0;
	}
	
	if (strcmp(command, "LEAVE") == 0) {
		terminator = strtok(NULL, " ");
		if (strcmp(terminator, "\n") != 0) LOG("Terminatore assente dal messaggio", WARNING);
		
		LOG("Messaggio di LEAVE ricevuto, chiusura connessione", INFO);
		disconnect();
		return 0;
	}

	name = strtok(NULL, " ");

	if (name == NULL) {
		LOG("Paramentro nome assente, chiusura connessione", ERROR);
		return 0;
	}

	if (strcmp(command, "REGISTER") == 0) {
		terminator = strtok(NULL, " ");

		if (strcmp(terminator, "\n") != 0) LOG("Terminatore assente dal messaggio", WARNING);

		registration(name);
		return 1;
	}

	if (strcmp(command, "RETRIEVE") == 0) {
		terminator = strtok(NULL, " ");
		if (strcmp(terminator, "\n") != 0) LOG("Terminatore assente dal messaggio", WARNING);

		retrieve(name);
		return 1;
	}

	if (strcmp(command, "DELETE") == 0) {
		terminator = strtok(NULL, " ");
		if (strcmp(terminator, "\n") != 0) LOG("Terminatore assente dal messaggio", WARNING);

		delete(name);
		return 1;
	}

	length = strtok(NULL, " ");
	if (length == NULL) {
		LOG("Paramentro lunghezza assente, chiusura connessione", ERROR);
		return 0;
	}

	int lunghezza = strtol(length, NULL, 10);
	
	if (lunghezza == 0) {
		LOG("Paramentro lunghezza non valido, chiusura connessione", ERROR);
		return 0;
	}

	if (strcmp(command, "STORE") == 0) {
		terminator = strtok(NULL, " ");
		if (strcmp(terminator, "\n") != 0) {
			LOG("Formato STORE non valido, chiusura connessione", ERROR);
			return 0;
		}

		data = strtok(NULL, " ");
		if (data == NULL) {
			LOG("Paramentro data assente, chiusura connessione", ERROR);
			return 0;
		}

		store(name, lunghezza, data);
	}
		 
	return 1;
}

void registration(char* name) {
	char buffer[500];
	sprintf(buffer, "Registrazione con nome %s", name);
	LOG(buffer, INFO);
}

void store(char* name, int length, char* data) {
	char buffer[150];
	sprintf(buffer, "Store con nome %s, lunghezza %d, dati[]", name);
	LOG(buffer, INFO);
}

void retrieve(char* name) {
	char buffer[150];
	sprintf(buffer, "Retrieve con nome %s", name);
	LOG(buffer, INFO);
}

void delete(char* name) {
	char buffer[150];
	sprintf(buffer, "Delete con nome %s", name);
	LOG(buffer, INFO);
}

void disconnect() {
	
}