#include "server.h"


int main() {
	int serverSocket = initServer();
	if (serverSocket < 0) {
		LOG("Errore inizializzazione server", ERROR);
		exit(EXIT_FAILURE);
	}

	listenForClients(serverSocket);

	save_status();
	close(serverSocket);
	unlink(SOCKNAME);
	
	return 0;
}

static void signalHandler(int signum) {
	if (signum == SIGUSR1) {
		printStatus();
		print = 1;
	}
	else {
		save_status();
		running = 0;
		_exit(EXIT_FAILURE);
	}
	
}


int setSignalHandlers() {
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));

	sigaction(SIGINT, NULL, &sa);
	sa.sa_handler = signalHandler;
	sigaction(SIGINT, &sa, NULL);

	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	sigaction(SIGUSR1, NULL, &sa);
	sa.sa_handler = signalHandler;
	sigaction(SIGUSR1, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
	
	return 1;
}


int initServer() {
	setSignalHandlers();
	pthread_mutex_init(&lock, NULL);
	hash_init(&clients);
	init_store();

	//if (pthread_create(&signalProcesser, NULL, &processSignals, NULL) == -1) return -5;

	struct stat st;
	memset(&st, 0, sizeof(st));
	if (stat("data", &st) == -1) {
		if(mkdir("data", 0700)) return -1;
	}

	int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(serverSocket == -1) return -2;

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	if(bind(serverSocket, (struct sockaddr*) & address, sizeof(address)) == -1) {
		unlink(SOCKNAME);
		if (bind(serverSocket, (struct sockaddr*) & address, sizeof(address))) return -3;
	}

	if(listen(serverSocket, SOMAXCONN)) return -4;

	return serverSocket;
}

int listenForClients(int serverSocket) {
	int clientSocket;
	pthread_t thread;
	while (running) {
		clientSocket = accept(serverSocket, NULL, 0);
		if(clientSocket == -1) return -1;

		int* socket = (int*)malloc(sizeof(int));
		*socket = clientSocket;
		if(pthread_create(&thread, NULL, &processClient, (void*)socket) == -1) return -2;
	}

	return clientSocket;
}

void* processClient(void* client)
{
	int clientSocket = *((int*)client);
	int status = 1, res;
	char clientName[MAX_NAME_SIZE] = "";
	char buffer[BUFFER_SIZE];
	Command command;
	int bytes, temp; 

	while (status == 1 && running == 1) {
		if ((bytes = read(clientSocket, buffer, BUFFER_SIZE)) == 0) {
			hash_remove(&clients, clientName);
			status = 0;
			close(clientSocket);
			pthread_mutex_lock(&lock);
			clientConnessi--;
			pthread_mutex_unlock(&lock);
			break;
		}

		temp = process_message(buffer, bytes, &command);
;
		switch (command.type) {
		case REGISTER:
			pthread_mutex_lock(&lock);
			if (hash_get(&clients, command.name) != -1) {
				pthread_mutex_unlock(&lock);
				sprintf(buffer, "KO Utente gia` registrato \n");
				write(clientSocket, buffer, strlen(buffer));
				close(clientSocket);
				status = 0;
			} else {
				strcpy(clientName, command.name);
				hash_insert(&clients, clientName, clientSocket);
				pthread_mutex_unlock(&lock);
				write(clientSocket, "OK \n", 4);
				pthread_mutex_lock(&lock);
				clientConnessi++;
				pthread_mutex_unlock(&lock);
			}
			break;

		case STORE:
			if (clientName[0] == '\0') {
				sprintf(buffer, "KO Utente non registrato \n");
				write(clientSocket, buffer, strlen(buffer));
			} else {
				int i, j = 0;

				for (i = temp; i < bytes; i++) {
					((char*)command.data)[j] = buffer[i];
					j++;
				}

				if (command.data_length - j > 0)
					read(clientSocket, (void*)(((char*)command.data) + j), command.data_length - j);

				res = store(clientName, command.name, command.data, command.data_length);
				
				switch (res) {
				case -1:
					sprintf(buffer, "KO Errore creazione file \n");
					break;

				case -2:
					sprintf(buffer, "KO Errore scrittura file \n");
					break;

				default:
					sprintf(buffer, "OK \n");
					break;
				}

				write(clientSocket, buffer, strlen(buffer));
		
			}
			break;

		case RETRIEVE:
			if (clientName[0] == '\0') {
				sprintf(buffer, "KO Utente non registrato \n");
				write(clientSocket, buffer, strlen(buffer));
			} else {

				char* data;
				size_t size = retrieve(clientName, command.name, (void**)& data);

				switch (size) {
				case -1:
					sprintf(buffer, "KO Errore apertura file (possibile file non esistente) \n");
					write(clientSocket, buffer, strlen(buffer));
					break;

				case -2:
					sprintf(buffer, "KO Errore lettura file (possibili dimensioni sbagliate) \n");
					write(clientSocket, buffer, strlen(buffer));
					break;

				default:;
					char* t = (char*)& size;
					sprintf(buffer, "DATA %c%c%c%c%c%c%c%c \n ", t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7]);
					write(clientSocket, buffer, 4 + 1 + 8 + 3);
					write(clientSocket, data, size);
					free(data);
					break;
				}
			}
			break;

		case DELETE:
			if (clientName[0] == '\0') {
				sprintf(buffer, "KO Utente non registrato \n");
				write(clientSocket, buffer, strlen(buffer));
			} else {

				res = delete(clientName, command.name);

				switch (res) {
				case -1:
					sprintf(buffer, "KO File non esistente \n");
					break;

				case -2:
					sprintf(buffer, "KO Errore eliminazione file \n");
					break;

				case 1:
					sprintf(buffer, "OK \n");
					break;
				}

				write(clientSocket, buffer, strlen(buffer));
			}
			break;

		case LEAVE:
			hash_remove(&clients, clientName);
			status = 0;
			write(clientSocket, "OK \n", 4);
			close(clientSocket);
			pthread_mutex_lock(&lock);
			clientConnessi--;
			pthread_mutex_unlock(&lock);
			break;

		case UNKNOWN:
			break;
		}
		memset(buffer, 0, BUFFER_SIZE);
	}
	
	pthread_exit(0);
}

void* processSignals(void* arg) {
	while (running) {
		if (print) {
			printStatus();
			print = 0;
		}
		sleep(1);
	}

	sleep(1);
	pthread_exit(0);
}

void printStatus() {
	char status[200];
	storage_status* storageStatus = get_status();
	sprintf(status, "Client connessi: %d, Numero file: %d, Numero cartelle: %d, Size totale: %d",
		clientConnessi,
		storageStatus->file_count,
		storageStatus->directory_count,
		storageStatus->size
	);
	LOG(status, INFO);
}
