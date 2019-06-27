#pragma once
#include "utils.h"

#define MAX_NAME_SIZE		100
#define MAX_LENGTH_SIZE		20

typedef enum {
	UNKNOWN = 0,
	REGISTER = 1,
	STORE = 2,
	RETRIEVE = 3,
	DELETE = 4,
	LEAVE = 5
}CommandType;

typedef struct {
	CommandType type;
	char name[MAX_NAME_SIZE];
	size_t data_length;
	void* data;
}Command;

int process_message(char* message, int bytes, Command* out);