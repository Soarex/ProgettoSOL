#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <error.h>

#define UNIX_PATH_MAX 108
#define SOCKNAME "socket"

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define INFO	0
#define WARNING 1
#define ERROR	2
#define DEBUG	3

#define LOG(message, level) \
	switch(level) { \
		case INFO:		printf(COLOR_GREEN "Client: %s\n" COLOR_RESET, message); break; \
		case WARNING:	printf(COLOR_YELLOW "Client: %s\n" COLOR_RESET, message); break; \
		case ERROR:		printf(COLOR_RED "Client: %s\n" COLOR_RESET, message); break; \
		default:		printf("Client: %s\n", message); \
	} \
	fflush(stdout);

#define CHECK(x) \
	if (x == -1) { \
		printf(COLOR_RED "Client, file %s linea %d" COLOR_RESET, __FILE__, __LINE__); \
		fflush(stdout); \
		perror(""); \
	}



int connectToServer();