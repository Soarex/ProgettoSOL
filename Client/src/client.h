#pragma once

#include <sys/socket.h>
#include "utils.h"
#include "message_processer.h"

#define UNIX_PATH_MAX 108
#define SOCKNAME "objstore.sock"
#define BUFFER_SIZE 500

int os_connect(char* name);
int os_store(char* name, void* block, size_t len);
void* os_retrieve(char* name);
int os_delete(char* name);
int os_disconnect();