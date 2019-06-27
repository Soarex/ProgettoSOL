#pragma once
#include "utils.h"

#define MAX_MESSAGE_SIZE	100
#define MAX_LENGTH_SIZE		10

typedef enum {
	UNKNOWN = 0,
	OK		= 1,
	KO		= 2,
	DATA	= 3
}CommandType;

typedef struct {
	CommandType type;
	char message[MAX_MESSAGE_SIZE];
	size_t data_length;
	void* data;
}Command;

int process_message(char* message, Command* out);