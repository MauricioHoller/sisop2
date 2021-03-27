#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#include "cliente.h"
#include "mensagem.h"

#define PORT 2000

pthread_t sender, receiver; //produtor / consumidor

char* username = "@leandro";

void *clientSnd(void *args) {
    

    int sockfd =  *((int *) args);
    
    PACOTE msg;
    msg.type = SEND;

    strncpy(msg.username, username, strlen(username) + 1);
    do {


        char msg_str[MSG_MAX_SIZE];
        fgets(msg_str, MSG_MAX_SIZE, stdin);

        /* Remove trailing newline, if there. */
        if ((strlen(msg_str) > 0) && (msg_str[strlen (msg_str) - 1] == '\n'))
            msg_str[strlen (msg_str) - 1] = '\0';

        strncpy(msg.txt, msg_str, strlen(msg_str) + 1);

        msg.seqn= msg.seqn+1;
        
        //puts(msg.txt);
        sendMessage(sockfd, &msg);


	
	} while (msg.type != QUIT);

    close(sockfd);
    
    return NULL;
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
         printf("%s says: %s\n", msg->username, msg -> txt); 
        }
        sleep(1000);
    }
}


int main(int argc, char const *argv[]) 
{ 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    char *Y = "Hello from client"; 
    char buffer[1024] = {0}; 
    char oi[256];
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); //trocar por argv[2] 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  //colocar endereço aqui argv[1] 
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
    // agora criamos uma thread para receber e enviar msg
    // as threads que recebem mensagem devem ser mostradas na tela a meg recebida as de enviar apaenas enviam e testam se o servidor ainda esta aberto  

    pthread_create(&receiver, NULL, clientRcv, &sock);

    clientSnd(&sock);
    // producer
    //pthread_create(&sender, NULL, clientSnd, &sock);

    //pthread_create()


    close(sock);

    return 0; 
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

