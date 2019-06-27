#include "object_store.h"

static pthread_mutex_t slock;
static storage_status status;

int init_store() {
	memset(&status, 0, sizeof(status));
	//pthread_mutex_init(&slock, NULL);

	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat("data", &st) == -1)
		if (mkdir("data", 0700) == -1) return -1;

	return load_status();
}

int store(char* clientName, char* blockName, void* data, size_t size) {
	if (!clientName || !blockName || !data) return 0;

	char path[100] = "data/";

	strcat(path, clientName);
	
	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat(path, &st) == -1) {
		if (mkdir(path, 0700) == -1) return -1;
		else status.directory_count++;
	}


	strcat(path, "/");
	strcat(path, blockName);

	int new = stat(path, &st) == -1;

	int fileDescriptor = open(path, O_WRONLY | O_CREAT, 0777);
	if(fileDescriptor == -1) return -1;


	if (new) {
		//pthread_mutex_lock(&slock);
		status.file_count++;
		//pthread_mutex_unlock(&slock);
	}

	if(write(fileDescriptor, &size, sizeof(size_t)) == -1) return -2;
	if(write(fileDescriptor, data, size) == -1) return -2;

	//pthread_mutex_lock(&slock);
	if (new) status.size += sizeof(size_t);
	status.size += size;
	//pthread_mutex_unlock(&slock);

	close(fileDescriptor);
	return 1;
}

size_t retrieve(char* clientName, char* blockName, void** dataOut) {
	if (!clientName || !blockName || !dataOut) return 0;

	char path[100] = "data/";

	strcat(path, clientName);

	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat(path, &st) == -1) {
		LOG("Tentativo di read in blocco non esistente", WARNING);
	}

	strcat(path, "/");
	strcat(path, blockName);
	int fileDescriptor = open(path, O_RDONLY, 0666);
	if(fileDescriptor == -1) return -1;
	
	size_t size;

	if(read(fileDescriptor, &size, sizeof(size_t)) != sizeof(size_t)) return -2;
	*dataOut = malloc(size);
	if(read(fileDescriptor, *dataOut, size) != size) return -2;
	close(fileDescriptor);
	return size;
}

int delete(char* clientName, char* objectName) {
	if (!clientName) return 0;

	char path[100];
	sprintf(path, "data/%s/%s", clientName, objectName);
	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat(path, &st) == -1) return -1;

	if(unlink(path) == -1) return -2;

	//pthread_mutex_lock(&slock);
	status.file_count--;
	status.size -= st.st_size;
	//pthread_mutex_unlock(&slock);
	return 1;
}

int save_status() {
	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat("data", &st) == -1) return -2;

	int fileDescriptor = open("data/status", O_WRONLY | O_CREAT, 0666);
	if (fileDescriptor == -1) return -2;


	if (write(fileDescriptor, &(status.directory_count), sizeof(int)) == -1) return -3;
	if (write(fileDescriptor, &(status.file_count), sizeof(int)) == -1) return -3;
	if (write(fileDescriptor, &(status.size), sizeof(int)) == -1) return -3;

	close(fileDescriptor);
	return 1;
}

int load_status() {
	struct stat st;
	memset(&st, 0, sizeof(st));

	if (stat("data", &st) == -1) return -2;

	int fileDescriptor = open("data/status", O_RDONLY, 0666);
	if (fileDescriptor == -1) {
		status.directory_count = 0;
		status.file_count = 0;
		status.size = 0;
		save_status();
	}

	if (read(fileDescriptor, &(status.directory_count), sizeof(int)) != sizeof(int)) return -3;
	if (read(fileDescriptor, &(status.file_count), sizeof(int)) != sizeof(int)) return -3;
	if (read(fileDescriptor, &(status.size), sizeof(int)) != sizeof(int)) return -3;

	close(fileDescriptor);
	return 1;
}

storage_status* get_status() {
	return &status;
}