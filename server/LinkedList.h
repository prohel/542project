

typedef struct node {
	char* string;
	struct node* next;
} node;

int addToList(node** pointerToHead, char* string);

void printList(node* head);

node* removeNode(node* head, char* string);

int freeList(node* head);
