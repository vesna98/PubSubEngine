#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	int data;
	CRITICAL_SECTION socketLock;
	struct node* next;
} TNODE;

void Init(TNODE** head) {
	*head = NULL;
}

void push(TNODE** phead, int new_data) {
	TNODE* newNode = (TNODE*)malloc(sizeof(TNODE));
	if (newNode != NULL) {
		newNode->data = new_data;
		InitializeCriticalSection(&(newNode->socketLock));
		newNode->next = *phead;
		*phead = newNode;
	}
}

// dodavanje na kraj
void append(TNODE** phead, int new_data) {
	TNODE* newNode = (TNODE*)malloc(sizeof(TNODE)); // novi čvor
	if (newNode != NULL) {
		newNode->data = new_data;
		newNode->next = NULL; // bice poslednji
	}
	if (*phead == NULL) { // da li je liste prazna
		*phead = newNode;
		return;
	}
	TNODE* last = *phead;
	while (last->next) // pratimo pokazivace do poslednjeg cvora
		last = last->next;
	last->next = newNode; // novi cvor postaje poslednji
	return;
}

void printList(TNODE* node) {
	while (node) {
		printf(" %d ", node->data);
		node = node->next;
	}
}

TNODE* find(TNODE* node, int data) {
	while (node)
		if (node->data == data)
			return node; // nadjen
		else
			node = node->next;
	return NULL; // čvor ne postoji
}

void removeNode(TNODE** phead, int data) {
	TNODE* tmp = *phead,
		* node = find(*phead, data);
	if (!node) return; // nije nadjen
	if (*phead == node) { // nadjeni cvor je prvi
		*phead = node->next;
		DeleteCriticalSection(&(node->socketLock));
		free(node);
		return;
	}
	while (tmp->next != node)
		tmp = tmp->next;
	tmp->next = node->next; // prevezivanje liste
	free(node); // oslobadjanje cvora
}

void destroy(TNODE** phead) {
	TNODE* tmp;
	// osobadja se jedan po jedan cvor
	// pocevsi od prvog
	while (*phead) {
		tmp = *phead;
		*phead = tmp->next;
		DeleteCriticalSection(&(tmp->socketLock));
		free(tmp);
	}
}