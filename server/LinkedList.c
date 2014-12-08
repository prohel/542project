#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

int addToList(node** pointerToHead, char* string) {

	if (string == NULL)
		return -1;


	node* node = malloc(sizeof(node));
	node->string = string;
	node->next = *pointerToHead;
	
	*pointerToHead = node;

	return 0;
}


void printList(node* head) {
	node* current = head;
	
	for (int i = 0; current != NULL; current = current->next, i++) {
		printf("%d. %s\n", i, current->string);
	}
}

int freeList(node* head) {
	
	node* current = head;
	
	while (current != NULL) {
		node* next = current->next;
		
		free(current->string);
		free(current);
		current = next;
	}
	
	return 0;
	
}