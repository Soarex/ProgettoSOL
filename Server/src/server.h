#pragma once

#include "utils.h"
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include "object_store.h"
#include "message_processer.h"
#include "hash_table.h"

#define UNIX_PATH_MAX 108
#define SOCKNAME "objstore.sock"
#define BUFFER_SIZE 500

static hash_table clients;
static pthread_t signalProcesser;
volatile sig_atomic_t print = 0;
volatile sig_atomic_t running = 1;
static int clientConnessi = 0;

static void signalHandler(int signum);

int setSignalHandlers();
int initServer();
int listenForClients(int socket);
void* processClient(void* client);
void* processSignals(void* arg);
void printStatus();