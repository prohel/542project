#include "LinkedList.h"
#include "twilio.h"
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
char serinput[1];
char curr[50];
char message[50];
char soundbuff[6];
char acceBuff[6];
int target; // Keeps track of which command the player should be sending
int success; // Boolean (1 or 0) to check if the player was successful or not
int score = 0; // keeps track of score
int fd;
int fdard;

/**
 * Linked lists holding phone numbers & emails
 */
node* emailListHead = NULL;
node* phoneListHead = NULL;


/**
 * Prototypes
 */
int sendEmail(char *body, char* to);
int sendAllEmails(char* body, int force);
int sendAllSMS(char* body, int force );

/** Starts listening for client requests. */
int start_server(int PORT_NUMBER) {

	// structs to represent the server and client
	struct sockaddr_in server_addr,client_addr;    

	int sock; // socket descriptor

	// 0. Serial config
	int fdard = open("/dev/ttyUSB10", O_RDWR);
	
	// 1. socket: creates a socket descriptor that you later use to make other system calls
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	
	int temp;
	
	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}

	// configure the server
	server_addr.sin_port = htons(PORT_NUMBER); // specify port number
	server_addr.sin_family = AF_INET;         
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	bzero(&(server_addr.sin_zero),8); 

	// 2. bind: use the socket and associate it with the port number
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}

	// 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(1);
	}

	// once you get here, the server is set up and about to start listening
	printf("\nServer configured to listen on port %d\n", PORT_NUMBER);
	fflush(stdout);


	//Buffer for e-mail/text body AND HTTP response
	char* reply = malloc(1000 * sizeof(char));


	//get and response to requests
	while (1) {
		printf("IN THE WHILE\n");

		// 4. accept: wait here until we get a connection on that port
		int sin_size = sizeof(struct sockaddr_in);
		int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
		printf("Server got a connection from (%s, %d)\n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

		// buffer to read data into
		char request[1024];

		// 5. recv: read incoming message into buffer
		int bytes_received = recv(fd,request,1024,0);
		
		// null-terminate the string
		request[bytes_received] = '\0';
		//printf("Here comes the message:\n");
		printf("%s\n", request);

		// Check if sent an 'audio' request
		if (strncmp("GET /audio/", request, 11) == 0) {
			
			//Get the float value out of the header
			double d;
			memcpy( soundbuff, &request[11], 5);
			soundbuff[5] = '\0';
			sscanf(soundbuff, "%lf", &d);

			printf("Sound level is: %lf\n", d);
			
			//Send alert
			strcpy(reply, "SUPER SECURE ALARM SYSTEM ALERT: sound level has reached ");
			strcat(reply, soundbuff);
			sendAllEmails(reply, 0);
			sendAllSMS(reply, 0);
		}
		
		// Check if sent an 'accel' requet
		if (strncmp("GET /accel/", request, 11) == 0) {
			
			//Get the float value out of the header
			double d;
			memcpy(acceBuff, &request[11], 5);
			acceBuff[5] = '\0';
			sscanf(acceBuff, "%lf", &d);

			printf("Accel level is: %lf\n", d);
			
			//Send the alert
			strcpy(reply, "SUPER SECURE ALARM SYSTEM ALERT: acceleration level has reached ");
			strcat(reply, acceBuff);
			sendAllEmails(reply, 0);
			sendAllSMS(reply, 0);
		}
		
		//Check if sent email request
		if (strncmp("GET /email/", request, 11) == 0) {

			//Get the email from header
			char* pch = strtok(&request[11], " ");
			int length = strlen(pch);			
			char* email = malloc(length + 1);
			memcpy(email, &request[11], length);
			email[length] = '\0';

			printf("email: %s\n", email);

			addToList(&emailListHead, email);
			printList(emailListHead);
		}

		//Check if sent phone request
		if (strncmp("GET /phone/", request, 11) == 0) {

			//Get the phone number from header
			char* pch = strtok(&request[11], " ");
			int length = strlen(pch);			
			char* phone = malloc(length + 1);
			memcpy(phone, &request[11], length);
			phone[length] = '\0';

			printf("phone: %s\n", phone);

			addToList(&phoneListHead, phone);
			printList(phoneListHead);
		}


		//Check if sent death request
		if (strncmp("GET /death/", request, 11) == 0) {

			//Send battery messages
			strcpy(reply, "SUPER SECURE ALARM SYSTEM ALERT: Battery will die in 1 hour");
			sendAllEmails(reply, 1);
			sendAllSMS(reply, 1);
		}


		//Check if remove email request
		if (strncmp("GET /remov/", request, 11) == 0) {

			//get e-mail to remove
			char* pch = strtok(&request[11], " ");
			int length = strlen(pch);			
			char* email = malloc(length + 1);
			memcpy(email, &request[11], length);
			email[length] = '\0';

			printf("email: %s\n", email);
			
			//remove the node
			emailListHead = removeNode(emailListHead, email);
			printList(emailListHead);
		}

		//Check if remove phone number reuqest
		if (strncmp("GET /remop/", request, 11) == 0) {
			
			//Get the number to remove
			char* pch = strtok(&request[11], " ");
			int length = strlen(pch);			
			char* phone = malloc(length + 1);
			memcpy(phone, &request[11], length);
			phone[length] = '\0';

			printf("phone: %s\n", phone);

			//Remove the node
			phoneListHead = removeNode(phoneListHead, phone);
			printList(phoneListHead);
		}



		// 6. send OKAY response to notify sender the message was received, and close socket
		send(fd, "HTTP/1.1 200 OK\n\nOKAY", strlen("HTTP/1.1 200 OK\n\nOKAY"), 0);
		close(fd);

	}

	//Clean up
	free(reply);
	freeList(phoneListHead);
	freeList(emailListHead);
	close(sock);
	
	printf("Server closed connection\n");

	return 0;
}


int main(int argc, char *argv[]) {
	// check the number of arguments
	if (argc != 2) {
		printf("\nUsage: server [port_number]\n");
		exit(0);
	}
	
	int PORT_NUMBER = atoi(argv[1]);
	start_server(PORT_NUMBER);
	return 0;
}

/**
 * Send emails to all recipients in the email linked list.
 * The body is the body of the e-mail.
 * force indicates whether to ignore the time since last sending an e-mail
 */
int sendAllEmails(char* body, int force) {
	node* current = emailListHead;
	
	while(current != NULL) {	
		//Send the email if forced, or if enough time has passed.	
		if (force || current->timeLast == 0 || (time(NULL) - current->timeLast) > 60 * 5) {
			printf("time since last send %ld seconds", (time(NULL) - current->timeLast));
			sendEmail(body, current->string);
			current->timeLast = time(NULL);		
		}

		current = current->next;
	}

	return 0;
}

/**
 * Sends an e-mail with the given body to the given recipient
 */
int sendEmail(char* body, char* to) {
	printf("trying to send e-mail\n");
	char cmd[5000];  // to hold the command.

	sprintf(cmd,"echo \"%s\" | /usr/bin/mailx -s 'Security Alert' %s", body, to); // prepare command.

	printf("%s\n", cmd);
	system(cmd);     // execute it.

	return 0;
}


/**
 * Send sms to all recipients in the phone linked list.
 * The body is the body of the text message.
 * force indicates whether to ignore the time since last sending an text
 */
int sendAllSMS(char* body, int force ) {
	node* current = phoneListHead;
	
	while(current != NULL) {
		
		if (force || current->timeLast == 0 || (time(NULL) - current->timeLast) > 60 * 5) {
			printf("time since last send %ld seconds\n", (time(NULL) - current->timeLast));
			sendSMS(body, current->string);
			current->timeLast = time(NULL);		
		}

		current = current->next;
	}

	return 0;
}



