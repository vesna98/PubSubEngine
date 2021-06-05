#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#define INITIAL_SIZE 1024
#define MAP_MISSING -3  /* No such element */
#define MAP_FULL -2 	/* Hashmap is full */
#define MAP_OMEM -1 	/* Out of Memory */
#define MAP_OK 0 	/* OK */

typedef struct _hashmap_element {
	char key[21];
	int in_use;
	TNODE* ids;
} hashmap_element;

// A hashmap has some maximum size and current size,
// as well as the data to hold.
typedef struct _hashmap_map {
	int table_size;
	int size;
	hashmap_element* data;
} hashmap_map;

void hashmap_free(hashmap_map* in); //Clear
hashmap_map* hashmap_new(); //Initialize
unsigned int hashmap_hash_str(hashmap_map* m, const char* k);
int hashmap_hash(hashmap_map* in, const char* key);
int hashmap_rehash(hashmap_map* in);
int subscribe(hashmap_map* in, const char* key, int value);
int addTopic(hashmap_map* in, const char* key);
int hashmap_get(hashmap_map* in, const char* key, TNODE** arg);
TNODE* getSubscribers(hashmap_map* in, const char* key);
int hashmap_length(hashmap_map* in);
int hashmap_get_one(hashmap_map* in, int* arg, int remove);
int hashmap_remove(hashmap_map* in, const char* key);


/* Deallocate the hashmap */
void hashmap_free(hashmap_map* in) {
	hashmap_map* m = (hashmap_map*)in;
	free(m->data);
	free(m);
}
//inicijalizacija
hashmap_map* hashmap_new() {
	hashmap_map* m = (hashmap_map*)malloc(sizeof(hashmap_map));
	if (!m) goto err;

	m->data = (hashmap_element*)calloc(INITIAL_SIZE, sizeof(hashmap_element));
	if (!m->data) goto err;

	m->table_size = INITIAL_SIZE;
	m->size = 0;

	return m;
err:
	if (m)
		hashmap_free(m);
	return NULL;
}

/*
 * Hashing function for an integer
 */
unsigned int hashmap_hash_str(hashmap_map* m, const char* k) {
	int key = atoi(k);

	/* Robert Jenkins' 32 bit Mix Function */
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);

	/* Knuth's Multiplicative Method */
	key = (key >> 3) * 2654435761;

	return key % m->table_size;
}

/*
 * Return the integer of the location in data
 * to store the point to the item, or MAP_FULL.
 */
int hashmap_hash(hashmap_map* in, const char* key) {
	int curr;
	int i;

	/* Cast the hashmap */
	hashmap_map* m = (hashmap_map*)in;

	/* If full, return immediately */
	if (m->size == m->table_size) return MAP_FULL;

	/* Find the best index */
	curr = hashmap_hash_str(m, key);

	/* Linear probling */
	for (i = 0; i < m->table_size; i++) {
		if (m->data[curr].in_use == 0)
			return curr;

		if (strcmp(m->data[curr].key, key) == 0 && m->data[curr].in_use == 1)
			return curr;

		curr = (curr + 1) % m->table_size;
	}

	return MAP_FULL;
}

int hashmap_rehash(hashmap_map* in) {
	int i;
	int old_size;
	hashmap_element* curr;

	/* Setup the new elements */
	hashmap_map* m = (hashmap_map*)in;
	hashmap_element* temp = (hashmap_element*)
		calloc(2 * m->table_size, sizeof(hashmap_element));
	if (!temp) return MAP_OMEM;

	/* Update the array */
	curr = m->data;
	m->data = temp;

	/* Update the size */
	old_size = m->table_size;
	m->table_size = 2 * m->table_size;
	m->size = 0;

	/* Rehash the elements */
	for (i = 0; i < old_size; i++) {
		while (curr[i].ids) {
			int status = subscribe(m, curr[i].key, curr[i].ids->data);
			if (status != MAP_OK)
				return status;
			curr[i].ids = curr[i].ids->next;
		}
	}

	free(curr);

	return MAP_OK;
}

/*
 * Add a pointer to the hashmap with some key
 */
int subscribe(hashmap_map* in, const char* key, int value) {
	int index;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* Lock for concurrency */
	/* Find a place to put our value */
	index = hashmap_hash(in, key);
	while (index == MAP_FULL) {
		if (hashmap_rehash(in) == MAP_OMEM) {
			return MAP_OMEM;
		}
		index = hashmap_hash(in, key);
	}

	/* Set the data */

	if (m->data[index].in_use == 1) {
		//strcpy_s(m->data[index].key, strlen(key), key);
		//strcpy(m->data[index].key, key);
		memcpy(m->data[index].key, key, sizeof(key));
		m->data[index].in_use = 1;
		append(&(m->data[index].ids), value);
	}
	else {
		TNODE* head;
		Init(&head);
		append(&head, value);
		m->data[index].ids = head;
		//strcpy_s(m->data[index].key, sizeof(key), key);
		//strcpy(m->data[index].key, key);
		memcpy(m->data[index].key, key, strlen(key));
		m->data[index].in_use = 1;
		m->size++;
	}

	return MAP_OK;
}

int addTopic(hashmap_map* in, const char* key) {
	int index;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* Lock for concurrency */
	/* Find a place to put our value */
	index = hashmap_hash(in, key);
	while (index == MAP_FULL) {
		if (hashmap_rehash(in) == MAP_OMEM) {
			return MAP_OMEM;
		}
		index = hashmap_hash(in, key);
	}

	/* Set the data */

	if (m->data[index].in_use == 0) {
		TNODE* head;
		Init(&head);
		m->data[index].ids = head;
		//strcpy_s(m->data[index].key, strlen(key), key);
		//strcpy(m->data[index].key, key);
		memcpy(m->data[index].key, key, sizeof(key));
		m->data[index].in_use = 1;
		m->size++;
	}

	return MAP_OK;
}

/*
 * Get your pointer out of the hashmap with a key
 */
int hashmap_get(hashmap_map* in, const char* key, TNODE** arg) {
	int curr;
	int i;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* Lock for concurrency */

	/* Find data location */
	curr = hashmap_hash_str(m, key);

	/* Linear probing, if necessary */
	for (i = 0; i < m->table_size; i++) {

		if (strcmp(m->data[curr].key, key) == 0 && m->data[curr].in_use == 1) {
			*arg = (TNODE*)(m->data[curr].ids);
			return MAP_OK;
		}

		curr = (curr + 1) % m->table_size;
	}

	*arg = NULL;

	/* Unlock */

	/* Not found */
	return MAP_MISSING;
}

TNODE* getSubscribers(hashmap_map* in, const char* key) {
	int curr;
	int i;
	hashmap_map* m;
	TNODE* subscribers = NULL;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* Lock for concurrency */

	/* Find data location */
	curr = hashmap_hash_str(m, key);

	/* Linear probing, if necessary */
	for (i = 0; i < m->table_size; i++) {

		if (strcmp(m->data[curr].key, key) == 0 && m->data[curr].in_use == 1) {
			subscribers = m->data[curr].ids;
		}

		curr = (curr + 1) % m->table_size;
	}

	/* Not found */
	return subscribers;
}


/* Return the length of the hashmap */
int hashmap_length(hashmap_map* in) {
	hashmap_map* m = (hashmap_map*)in;
	if (m != NULL) return m->size;
	else return 0;
}

/*
 * Get a random element from the hashmap
 */
int hashmap_get_one(hashmap_map* in, int* arg, int remove) {
	int i;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* On empty hashmap return immediately */
	if (hashmap_length(m) <= 0)
		return MAP_MISSING;

	/* Lock for concurrency */

	/* Linear probing */
	for (i = 0; i < m->table_size; i++)
		if (m->data[i].in_use != 0) {
			*arg = (int)(m->data[i].ids);
			if (remove) {
				m->data[i].in_use = 0;
				m->size--;
			}
			return MAP_OK;
		}

	/* Unlock */

	return MAP_OK;
}

/*
 * Iterate the function parameter over each element in the hashmap.  The
 * additional any_t argument is passed to the function as its first
 * argument and the hashmap element is the second.
 */
 /*int hashmap_iterate(hashmap_map* in, PFany f, int item) {
	 int i;

	 hashmap_map* m = (hashmap_map*)in;

	 if (hashmap_length(m) <= 0)
		 return MAP_MISSING;



	 for (i = 0; i < m->table_size; i++)
		 if (m->data[i].in_use != 0) {
			 int data = (int)(m->data[i].data);
			 int status = f(item, data);
			 if (status != MAP_OK) {
				 semaphore_V(m->lock);
				 return status;
			 }
		 }


	 return MAP_OK;
 } */

 /*
  * Remove an element with that key from the map
  */
int hashmap_remove(hashmap_map* in, const char* key) {
	int i;
	int curr;
	hashmap_map* m;

	/* Cast the hashmap */
	m = (hashmap_map*)in;

	/* Lock for concurrency */

	/* Find key */
	curr = hashmap_hash_str(m, key);

	/* Linear probing, if necessary */
	for (i = 0; i < m->table_size; i++) {
		if (strcmp(m->data[curr].key, key) == 0 && m->data[curr].in_use == 1) {
			/* Blank out the fields */
			m->data[curr].in_use = 0;
			destroy(&(m->data[curr].ids));
			//strcpy_s(m->data[curr].key, 1, "\0");
			//strcpy(m->data[curr].key, "\0");
			memcpy(m->data[curr].key, "\0", 1);

			/* Reduce the size */
			m->size--;
			return MAP_OK;
		}
		curr = (curr + 1) % m->table_size;
	}

	/* Unlock */

	/* Data not found */
	return MAP_MISSING;
}

