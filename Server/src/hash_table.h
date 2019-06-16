#pragma once

#include <stdlib.h>
#include <string.h>

#define START_SIZE 500

typedef struct {
	int used;
	char key[100];
	int value;
}data;

typedef struct {
	data* table;
	int size;
	int elements;
}hash_table;

unsigned long hash(char* str);

int hash_init(hash_table* table);
int hash_insert(hash_table* table, char* key, int value);
int hash_remove(hash_table* table, char* key);
int hash_get(hash_table* table, char* key);