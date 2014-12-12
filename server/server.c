#include "LinkedList.h"

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
char msg[20];
pthread_t thread_id1;
pthread_t thread_id2;
void update_stats(float);
int target; // Keeps track of which command the player should be sending
int success; // Boolean (1 or 0) to check if the player was successful or not
int score = 0; // keeps track of score
int fd;
int fdard;
pthread_mutex_t  lock;

node* emailListHead = NULL;
node* phoneListHead = NULL;

/**
 * Prototypes
 */
int sendEmail(char *body, char* to);
int sendAllEmails(char* body);

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
        char reply[1000];

        // Check if player sent an 'up' request
        if (strncmp("GET /audio/", request, 11) == 0) {
    	   /*Target should be equal to 2, the code for the 7SEG display
         when it shows a bar lit up in the top right.*/
          memcpy( soundbuff, &request[11], 5);
          soundbuff[5] = '\0';
          double d;

          sscanf(soundbuff, "%lf", &d);

          printf("Sound level is: %lf\n", d);
          strcpy(reply, "SUPER SECURE ALARM SYSTEM ALERT: sound level has reached ");
          strcat(reply, soundbuff);
          sendAllEmails(reply);
    	}
		
		if (strncmp("GET /email/", request, 11) == 0) {
			printf("request: %s\n", request);
			
			char* pch = strtok(&request[11], " ");
			
			int length = strlen(pch);			
			char* email = malloc(length + 1);
			memcpy(email, &request[11], length);
			email[length] = '\0';
			
			printf("email: %s\n", email);
			
			addToList(&emailListHead, email);
			
			printList(emailListHead);
			
		}
		
		
		if (strncmp("GET /accel/", request, 11) == 0) {
			printf("request: %s\n", request);
			
            memcpy(acceBuff, &request[11], 5);
			acceBuff[5] = '\0';
			
            double d;

            sscanf(acceBuff, "%lf", &d);

            printf("Accel level is: %lf\n", d);
            strcpy(reply, "SUPER SECURE ALARM SYSTEM ALERT: acceleration level has reached ");
            strcat(reply, soundbuff);
            sendAllEmails(reply);
		
		
		}
		
		
		
		if (strncmp("GET /phone/", request, 11) == 0) {
			printf("request: %s\n", request);
			
			char* pch = strtok(&request[11], " ");
			
			int length = strlen(pch);			
			char* phone = malloc(length + 1);
			memcpy(phone, &request[11], length);
			phone[length] = '\0';
			
			printf("phone: %s\n", phone);
			
			addToList(&phoneListHead, phone);
			
			printList(phoneListHead);
			
		}
		
		if (strncmp("GET /remov/", request, 11) == 0) {
			printf("request: %s\n", request);
			
			char* pch = strtok(&request[11], " ");
			
			int length = strlen(pch);			
			char* email = malloc(length + 1);
			memcpy(email, &request[11], length);
			email[length] = '\0';
			
			printf("email: %s\n", email);
			
			emailListHead = removeNode(emailListHead, email);
			printf("emailListHead %p\n", emailListHead);
			printList(emailListHead);
			
		}
		
		if (strncmp("GET /remop/", request, 11) == 0) {
			printf("request: %s\n", request);
			
			char* pch = strtok(&request[11], " ");
			
			int length = strlen(pch);			
			char* phone = malloc(length + 1);
			memcpy(phone, &request[11], length);
			phone[length] = '\0';
			
			printf("phone: %s\n", phone);
			
			phoneListHead = removeNode(phoneListHead, phone);
			printf("phoneListHead %p\n", phoneListHead);
			printList(phoneListHead);
			
		}
		
  
      // 6. send: send the message over the socket
      // note that the second argument is a char*, and the third is the number of chars
      //send(fd, reply, strlen(reply), 0);
      //printf("Server sent message: %s\n", reply);

      // 7. close: close the socket connection
		send(fd, "OKAY", strlen("OKAY"), 0);
      	close(fd);
		
	 }// Check if player sent a 'down' request
    
  /*Building the message to the server to give the player appropriate
  feedback*/
     
      
      close(sock);
      printf("Server closed connection\n");
  
      return 0;
}


int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 2)
    {
      printf("\nUsage: server [port_number]\n");
      exit(0);
    }
  int PORT_NUMBER = atoi(argv[1]);
  printf("\n\nPlease read the README.txt to understand the specifications of our server\n\n");
  //Start the thread
  start_server(PORT_NUMBER);
  return 0;
}

int sendAllEmails(char* body) {
	node* current = emailListHead;
	while(current != NULL) {
		sendEmail(body, current->string);
		current = current->next;
	}
	
	return 0;
}

int sendEmail(char* body, char* to) {
	printf("trying to send e-mail\n");
        char cmd[5000];  // to hold the command.
		
        sprintf(cmd,"echo \"%s\" | /usr/bin/mailx -s 'Security Alert' %s", body, to); // prepare command.
        
		printf("%s\n", cmd);
		system(cmd);     // execute it.

        return 0;
}



