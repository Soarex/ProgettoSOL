#include "client.h"

int main()
{
	int socketDescriptor = connectToServer();
	char command[150], name[150], length[150], data[150];
	int input;

	do {
		printf("Inserire input: ");
		scanf("%d", &input);
		switch (input) {
		case 1:
			printf("Inserire nome per register: ");
			scanf("%s", &name);
			sprintf(command, "REGISTER %s \n", name);
			break;

		case 2:
			printf("Inserire nome, lunghezza e data per store: ");
			scanf("%s %s %s", &name, &length, &data);
			sprintf(command, "STORE %s %s \n %s", name, length, data);
			break;

		case 3:
			printf("Inserire nome per retrieve: ");
			scanf("%s", &name);
			sprintf(command, "RETRIEVE %s \n", name);
			break;

		case 4:
			printf("Inserire nome per delete: ");
			scanf("%s", &name);
			sprintf(command, "DELETE %s \n", name);
			break;

		case 0:
			sprintf(command, "LEAVE \n");
			break;
		}

		write(socketDescriptor, command, strlen(command));
	} while (input != 0);

	close(socketDescriptor);
	return 0;
}

int connectToServer() 
{
	int socketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un address;
	strncpy(address.sun_path, SOCKNAME, UNIX_PATH_MAX);
	address.sun_family = AF_UNIX;

	while (connect(socketDescriptor, (struct sockaddr*) & address, sizeof(address)) == -1) 
	{
		LOG("Server non disponibile, nuovo tentativo tra 1 secondo", WARNING);
		sleep(1);
	}

	return socketDescriptor;
}
