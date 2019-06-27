#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <sys/types.h>
#include <sys/un.h>
#include "sys/stat.h"


#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define MESSAGE	0
#define INFO	1
#define WARNING 2
#define ERROR	3

#define LOG(message, level) \
	switch(level) { \
		case INFO:		printf(COLOR_GREEN "[Client] %s\n" COLOR_RESET, message); break; \
		case WARNING:	printf(COLOR_YELLOW "[Client] %s\n" COLOR_RESET, message); break; \
		case ERROR:		printf(COLOR_RED "[Client] %s\n" COLOR_RESET, message); break; \
		default:		printf("[Client] %s\n", message); \
	} \
	fflush(stdout);

#define CHECK(x) \
	if (x == -1) { \
		printf(COLOR_RED "[Client] %s linea %d: " COLOR_RESET, __FILE__, __LINE__); \
		fflush(stdout); \
		perror(""); \
	}


typedef unsigned char byte;