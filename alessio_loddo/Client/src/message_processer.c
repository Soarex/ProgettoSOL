#include "message_processer.h"

int process_message(char* message, Command* out) {
	if (message == NULL || out == NULL) return 0;

	memset(out, 0, sizeof(Command));
	int i = 0;
	char temp[MAX_MESSAGE_SIZE];

	
	while (message[i] != ' ' && message[i] != '\0') {
		temp[i] = message[i];
		i++;
	}

	temp[i] = '\0';

	i++;

	if (strcmp(temp, "OK") == 0) {
		out->type = OK;
		return 1;
	}

	if (strcmp(temp, "KO") == 0) {
		out->type = KO;

		int j = 0;
		while ((message[i + 1] != '\n' && message[i] != '\0') && j < MAX_MESSAGE_SIZE) {
			out->message[j] = message[i];
			i++;
			j++;
		}

		out->message[j] = '\0';

		if (j == MAX_MESSAGE_SIZE) return 0;

		return 1;
	}

	if (strcmp(temp, "DATA") == 0) {
		out->type = DATA;

		size_t a;
		unsigned char* t = (unsigned char*)& a;

		for (int j = 0; j < 8; j++) {
			t[j] = message[i];
			i++;
		}

		out->data_length = a;
		out->data = malloc(out->data_length);
		if (!out->data) return 0;
		return i + 3;
	}

	out->type = UNKNOWN;
	return 0;
}