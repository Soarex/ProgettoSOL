#include "message_processer.h"

int get_name(char* message, Command* out) {
	int i = 0;

	while ((message[i] != ' ' && message[i] != '\0') && i < MAX_NAME_SIZE) {
		out->name[i] = message[i];
		i++;
	}

	out->name[i] = '\0';

	if (i == MAX_NAME_SIZE) return -1;
	
	return i + 1;
}

int process_message(char* message, int bytes, Command* out) {
	if (message == NULL || out == NULL) return 0;

	memset(out, 0, sizeof(Command));
	
	int i = 0, j = 0;
	char temp[MAX_NAME_SIZE];

	while (message[i] != ' ' && message[i] != '\0') {
		temp[i] = message[i];
		i++;
	}
	
	temp[i] = '\0';

	i++;
	if (strcmp(temp, "REGISTER") == 0) {
		out->type = REGISTER;
		if(get_name(message + i, out) == -1) return 0;
		return 1;
	}
	
	if (strcmp(temp, "STORE") == 0) {
		out->type = STORE;
		if ((j = get_name(message + i, out)) == -1) return 0;

		i += j;

		size_t a;

		unsigned char* t = (unsigned char*)& a;

		for (int j = 0; j < 8; j++) {
			t[j] = message[i];
			i++;
		}

		out->data_length = a;
		out->data = malloc(a);
		
		return i + 3;
	}

	if (strcmp(temp, "RETRIEVE") == 0) {
		out->type = RETRIEVE;

		if ((i = get_name(message + i, out)) == -1) return 0;
		return 1;
	}

	if (strcmp(temp, "DELETE") == 0) {
		out->type = DELETE;
		if ((i = get_name(message + i, out)) == -1) return 0;
		return 1;
	}

	if (strcmp(temp, "LEAVE") == 0) {
		out->type = LEAVE;
		return 1;
	}

	out->type = UNKNOWN;
	return 0;
}