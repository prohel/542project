#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

node* removeNode(node* head, char* string) {
	printf("remove: %s\n", string);
	
	if (head == NULL) {
		return NULL;
	}
	
	
	printf("strlen(head->string): %lu\n", strlen(head->string));
	printf("strlen(string): %lu\n", strlen(string));
	
	printf("passed in: %s, head: %s\n", string, head->string);
	printf("strcmp() evaluated to %s\n", strcmp(string, head->string) == 0 ? "TRUE" : "FALSE");
	
	if (strcmp(string, head->string) == 0) {
		node* newHead = head->next;
		free(head->string);
		free(head);
		return newHead;
	}
	
	node* previous = head;
	node* current = head->next;

	while (current != NULL) {
		
		if (strcmp(string, current->string) == 0) {
			previous->next = current->next;
			free(current->string);
			free(current);
			return head;
		}
		
		previous = current;
		current = current->next;
	}
	
	return head;
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