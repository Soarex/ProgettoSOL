#include "client.h"

static int socketServer = 0;
static char buffer[BUFFER_SIZE];
static Command command;

int os_connect(char* name) {
	if (socketServer != 0) return 0;

	socketServer = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socketServer == -1) return 0;

	static struct sockaddr_un address;
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

	sprintf(buffer, "REGISTER %s \n", name);
	if(write(socketServer, buffer, strlen(buffer)) == -1) return 0;
	memset(buffer, 0, BUFFER_SIZE);
	if(read(socketServer, buffer, BUFFER_SIZE) <= 0) return 0;
	
	process_message(buffer, &command);
	memset(buffer, 0, BUFFER_SIZE);
	switch (command.type) {
	case OK:
		return 1;
		break;

	case KO:
		sprintf(buffer, "Errore durante connect: %s", command.message);
		//LOG(buffer, ERROR);
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
	memset(buffer, 0, BUFFER_SIZE);
	return 0;
}

int os_store(char* name, void* block, size_t len) {
	if (socketServer == 0) return 0;
	memset(buffer, 0, BUFFER_SIZE);

	unsigned char* t = (unsigned char*)& len;
	sprintf(buffer, "STORE %s %c%c%c%c%c%c%c%c \n ", name, t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7]);
	
	
	if(write(socketServer, buffer, 5 + 1 + strlen(name) + 1 + 8 + 3) == -1) return 0;

	if(write(socketServer, block, len) == -1) return 0;
	memset(buffer, 0, BUFFER_SIZE);
	if (read(socketServer, buffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		return 0;
	}
	
	//LOG(buffer, INFO);
	process_message(buffer, &command);
	switch (command.type) {
	case OK:
		return 1;
		break;

	case KO:
		sprintf(buffer, "Errore durante store: %s", command.message);
		LOG(buffer, ERROR);
		return 0;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return 0;
		break;
	}
	memset(buffer, 0, BUFFER_SIZE);
}

void* os_retrieve(char* name) {
	if (socketServer == 0) return NULL;
	memset(buffer, 0, BUFFER_SIZE);

	sprintf(buffer, "RETRIEVE %s \n", name);
	//LOG(buffer, INFO);
	if (write(socketServer, buffer, strlen(buffer)) == -1) return NULL;

	int bytes = read(socketServer, buffer, BUFFER_SIZE);
	//for (int i = 0; i < bytes; i++)
	//	printf("%d ", buffer[i]);

	//printf("\n");
	if (bytes <= 0) {
		os_disconnect();
		return NULL;
	}
	
	int res = process_message(buffer, &command);
	if (!res) return NULL;
	if (command.data_length <= 0) return NULL;

	switch (command.type) {
	case KO:
		sprintf(buffer, "Errore durante retrieve: %s", command.message);
		LOG(buffer, ERROR);
		return NULL;
		break;

	case DATA:;
		int i, j = 0;

		for (i = res; i < bytes; i++) {
			((char*)command.data)[j] = buffer[i];
			j++;
		}
		
		if(command.data_length - j > 0)
			bytes = read(socketServer, ((char*)command.data) + j, command.data_length - j);
		
		//for (int i = 0; i < bytes; i++)
		//	printf("%d ", ((char*)command.data)[i]);

		//printf("\n");
		return command.data;
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR);
		return NULL;
		break;
	}
	
	memset(buffer, 0, BUFFER_SIZE);
	return NULL;
}

int os_delete(char* name) {
	if (socketServer == 0) return 0;
	memset(buffer, 0, BUFFER_SIZE);

	sprintf(buffer, "DELETE %s \n", name);
	if(write(socketServer, buffer, strlen(buffer)) == -1) return 0;
	if (read(socketServer, buffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		return 0;
	}
	process_message(buffer, &command);

	switch (command.type) {
	case OK: 
		return 1; 
		break;

	case KO:
		sprintf(buffer, "Errore durante delete: %s", command.message);
		LOG(buffer, ERROR);
		return 0; 
		break;

	default:
	case UNKNOWN:
		LOG("Formato messaggio non riconosciuto", ERROR); 
		return 0; 
		break;
	}
	
	return 0;
	memset(buffer, 0, BUFFER_SIZE);
}

int os_disconnect() {
	if (socketServer == 0) return 0;
	memset(buffer, 0, BUFFER_SIZE);

	if(write(socketServer, "LEAVE \n", 8) == -1) return 0;
	if (read(socketServer, buffer, BUFFER_SIZE) <= 0) {
		socketServer = 0;
		close(socketServer);
		return 0;
	}

	process_message(buffer, &command);

	close(socketServer);
	socketServer = 0;

	switch (command.type) {
	case OK:
		return 1;
		break;

	default:
	case UNKNOWN:
		//LOG("Formato messaggio non riconosciuto", ERROR);
		return 0;
		break;
	}

	memset(buffer, 0, BUFFER_SIZE);
	return 0;
}
