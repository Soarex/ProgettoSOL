#include "client.h"

static int socketServer = 0;
static char messageBuffer[BUFFER_SIZE];
static Command command;

int os_connect(char* name) {
	if (socketServer != 0) return 0;

	socketServer = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socketServer == -1) return 0;

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	int tries = 0;
	while (connect(socketServer, (struct sockaddr*) & address, sizeof(address)) == -1 && tries < 10) {
		tries++;
		LOG("Server non disponibile, nuovo tentativo tra 1 secondo", WARNING);
		sleep(1);
	}

	if (tries == 10) {
		LOG("Numero tentativi superato", ERROR);
		return 0;
	}

	sprintf(messageBuffer, "REGISTER %s \n", name);
	if(write(socketServer, messageBuffer, strlen(messageBuffer)) == -1) return 0;

	if(read(socketServer, messageBuffer, BUFFER_SIZE) <= 0) return 0;
	
	process_message(messageBuffer, &command);

	switch (command.type) {
	case OK:
		return 1;
		break;

	case KO:
		sprintf(messageBuffer, "Errore durante connect: %s", command.message);
		LOG(messageBuffer, ERROR);
		close(socketServer);
		socketServer = 0;
		return 0;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return 0;
		break;
	}

	return 0;
}

int os_store(char* name, void* block, size_t len) {
	if (socketServer == 0) return 0;

	byte* t = (byte*)& len;
	sprintf(messageBuffer, "STORE %s %c%c%c%c%c%c%c%c \n ", name, t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7]);
	
	
	if(write(socketServer, messageBuffer, 5 + 1 + strlen(name) + 1 + 8 + 3) == -1) return 0;

	if(write(socketServer, block, len) == -1) return 0;

	if (read(socketServer, messageBuffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		return 0;
	}
	

	process_message(messageBuffer, &command);
	switch (command.type) {
	case OK:
		return 1;
		break;

	case KO:
		sprintf(messageBuffer, "Errore durante store: %s", command.message);
		LOG(messageBuffer, ERROR);
		return 0;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return 0;
		break;
	}
}

void* os_retrieve(char* name) {
	if (socketServer == 0) return NULL;

	sprintf(messageBuffer, "RETRIEVE %s \n", name);
	if (write(socketServer, messageBuffer, strlen(messageBuffer)) == -1) return NULL;

	int bytes = read(socketServer, messageBuffer, 4 + 1 + 8 + 3);

	if (bytes <= 0) {
		os_disconnect();
		return NULL;
	}
	
	int res = process_message(messageBuffer, &command);

	if (!res) return NULL;
	if (command.data_length <= 0) return NULL;

	switch (command.type) {
	case KO:
		sprintf(messageBuffer, "Errore durante retrieve: %s", command.message);
		LOG(messageBuffer, ERROR);
		return NULL;
		break;

	case DATA:;
		int dataBytes = 0;

		while (dataBytes != command.data_length)
			dataBytes += read(socketServer, (void*)(((byte*)command.data) + dataBytes), BUFFER_SIZE);
		
		return command.data;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return NULL;
		break;
	}
	
	return NULL;
}

int os_delete(char* name) {
	if (socketServer == 0) return 0;

	sprintf(messageBuffer, "DELETE %s \n", name);
	if(write(socketServer, messageBuffer, strlen(messageBuffer)) == -1) return 0;

	if (read(socketServer, messageBuffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		return 0;
	}
	process_message(messageBuffer, &command);

	switch (command.type) {
	case OK: 
		return 1; 
		break;

	case KO:
		sprintf(messageBuffer, "Errore durante delete: %s", command.message);
		LOG(messageBuffer, ERROR);
		return 0; 
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR); 
		return 0; 
		break;
	}
	
	return 0;
}

int os_disconnect() {
	if (socketServer == 0) return 0;

	if(write(socketServer, "LEAVE \n", 8) == -1) return 0;
	if (read(socketServer, messageBuffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		close(socketServer);
		return 0;
	}

	process_message(messageBuffer, &command);

	close(socketServer);
	socketServer = 0;

	switch (command.type) {
	case OK:
		return 1;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return 0;
		break;
	}

	return 0;
}
