#pragma once

#include "utils.h"
#include <fcntl.h>
#include <dirent.h>

typedef struct {
	int file_count;
	int directory_count;
	int size;
}storage_status;

static storage_status status;

int init_store();
int store(char* clientName, char* blockName, void* data, size_t size);
size_t retrieve(char* clientName, char* blockName, void** dataOut);
int delete(char* clientName, char* objectName);

int load_status();
int save_status();
storage_status* get_status();