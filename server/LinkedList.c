#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LinkedList.h"

/**
 * Creates a node with the given string, and adds to the list 
 * of the given head. 
 * Updates the pointer to the new head of the list.
 */
int addToList(node** pointerToHead, char* string) {

	if (string == NULL)
		return -1;


	node* node = malloc(sizeof(node));
	node->string = string;
	node->timeLast = 0;
	node->next = *pointerToHead;
	
	*pointerToHead = node;

	return 0;
}


/**
 * Prints the contents of the list specified by the head
 */
void printList(node* head) {
	node* current = head;
	
	for (int i = 0; current != NULL; current = current->next, i++) {
		printf("%d. %s\n", i, current->string);
	}
}

/**
 * Deletes the node containing the given string from 
 * the list specified by head.
 * 
 * returns the new head of the list.
 */
node* removeNode(node* head, char* string) {
	printf("remove: %s\n", string);
	
	if (head == NULL) {
		return NULL;
	}
	
	//Handle case where head is the removed node
	if (strcmp(string, head->string) == 0) {
		node* newHead = head->next;
		free(head->string);
		free(head);
		return newHead;
	}
	
	node* previous = head;
	node* current = head->next;

	//Search rest of list
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


/**
 * Frees all the node and strings contained in the 
 * specified linked list
 */ 
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