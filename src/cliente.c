#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#include "cliente.h"
#include "mensagem.h"

#define PORT 2000

pthread_t sender, receiver; 

char username[MAXNAME];

void check_args(char const* argv[])
{
	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL ) {
		printf("Not enough args!\n");
		exit(1);
	}
} 

int main(int argc, char const* argv[]) 
{ 
	check_args(argv);

    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 


    strncpy(username, argv[1], strlen(argv[1]) + 1);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons( atoi(argv[3]) );  
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, argv[2], &serv_addr.sin_addr)<=0)  //colocar endereço aqui argv[1] 
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

    login(sock); // login feito com sucesso ao servidor 

	// Doing nothing atm
//    pthread_create(&receiver, NULL, clientRcv, &sock);

    clientSnd(&sock);

    close(sock);

    return 0; 
} 



void *clientSnd(void *args) {
    
    int sockfd =  *((int *) args);
    
    PACOTE msg;
    msg.type = SEND;

    strncpy(msg.username, username, strlen(username) + 1);
    
    do {
    
        char msg_str[MSG_MAX_SIZE];

        printf("#: ");
        fgets(msg_str, MSG_MAX_SIZE, stdin);

        parse_message_str(msg_str, &msg);
        
        if (msg.type == SEND   || 
			msg.type == FOLLOW || 
			msg.type == QUIT) {
         
		    msg.seqn= msg.seqn+1;
            sendMessage(sockfd, &msg);
         
		 }
	
	} while ( msg.type != QUIT );



}

void *clientRcv(void * args) 
{
    int sockfd =  *((int *) args);
    PACOTE* msg;
    int byteCount;
    while(1)
    {
        byteCount = readMessage(sockfd, msg);

        if (byteCount < 0 ) {
            printf("Error listening socket");
            break;
        }

        if (strlen(msg->txt) > 0 ){
         printf("%s says: '%s' to me *-*\n", msg->username, msg -> txt); 
        }
        sleep(1000);
    }
}



void login(int socket) {

    PACOTE msg;

    clientMessage(&msg, LOGIN, username, "");

    if (sendMessage(socket, &msg) < 0){

         fprintf(stdout, "Server disconnected.\n");

               close(socket);

    		exit(1);
    }

    
}

