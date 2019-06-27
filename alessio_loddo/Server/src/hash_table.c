#include "hash_table.h"

unsigned long hash(char* str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

int hash_init(hash_table* table) {
	table->table = malloc(START_SIZE * sizeof(data));
	table->size = START_SIZE;
	table->elements = 0;
	return 1;
}

int hash_insert(hash_table* table, char* key, int value) {
	unsigned long h = hash(key);
	
	int i = 0;
	while (table->table[h % table->size].used == 1 && strcmp(key, table->table[h % table->size].key) != 0 && i < table->size) {
		i++;
		h++;
	}

	if (i == table->size) return -1;

	table->table[h % table->size].used = 1;
	strcpy(table->table[h % table->size].key, key);
	table->table[h % table->size].value = value;

	table->elements++;

	if (table->size * 0.8 < table->elements) {
		table->table = realloc(table->table, (table->size + 100) * sizeof(data));
		table->size += 100;

		for (int i = 0; i < table->size; i++)
			if(table->table[i].used)
				if (hash(table->table[i].key) % table->size != i) {
					hash_remove(table, table->table[i].key);
					hash_insert(table, table->table[i].key, table->table[i].value);
				}
	}

	return 1;
}

int hash_remove(hash_table* table, char* key) {
	unsigned long h = hash(key);
	if (!table->table[h % table->size].used)
		return -1;

	int i = 0;
	while (table->table[h % table->size].used == 1 && strcmp(key, table->table[h % table->size].key) != 0 && i < table->size) {
		i++;
		h++;
	}

	if (table->table[h % table->size].used == 0 || i == table->size) return -1;

	table->table[h % table->size].used = 0;

	table->elements--;
	return 1;
}

int hash_get(hash_table* table, char* key) {
	unsigned long h = hash(key);
	if(!table->table[h % table->size].used)
		return -1;

	int i = 0;
	while (table->table[h % table->size].used == 1 && strcmp(key, table->table[h % table->size].key) != 0 && i < table->size) {
		i++;
		h++;
	}
	
	if (table->table[h % table->size].used == 0 || i == table->size) return -1;
	return table->table[h % table->size].value;
}