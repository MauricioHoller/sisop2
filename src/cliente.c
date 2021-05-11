#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

#include "cliente.h"
#include "mensagem.h"

#define PORT 2000

pthread_t sender, receiver; //produtor / consumidor
char* username;


void check_args(char* argv[])
{
	if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL ) {
		printf("Not enough args!\n");
		exit(1);
	}
} 

int verificaTipo(char *msg){
	char aux[10]; 
     	
	int i;
	
	if (strcmp(msg,"QUIT") == 0)
		return SAIR;

	if (strlen (msg)>4){
		for (i=0;msg[i] != ' ';i++){
			aux[i] = msg[i]; 
		}
		aux[i]= '\0';
	}
//	printf("%s \n",aux);
	if (strcmp(aux, "SEND") == 0)
		return SEND;
		
	if (strcmp(aux,"FOLLOW") == 0)
		return FOLLOW;

	return ERRO;

}

void filtro (PACOTE *msg){
int i,j;
j=0;
	if ( (msg->type)==SEND){
		for (i=5;i <(strlen(msg->txt));i++){
			msg->txt[j] = msg->txt[i]; 
			j++;
		}
		msg->txt[j]= '\0';
}
	if ( (msg->type)==FOLLOW){
		for (i=7;i <(strlen(msg->txt));i++){
			msg->txt[j] = msg->txt[i]; 
			j++;
		}
		msg->txt[j]= '\0';
}


}

void *clientSnd(void *args) {
    

    int sockfd =  *((int *) args);
    
    PACOTE msg;
    msg.type = SEND;
    
    char aux[MSG_MAX_SIZE];
    strncpy(msg.username, username, strlen(username) + 1);
    do {


        char msg_str[MSG_MAX_SIZE];
        printf("#: ");
        fgets(msg_str, MSG_MAX_SIZE, stdin);
        
        //msg.type = verificaTipo(aux);
    
        /* Remove trailing newline, if there. */
        if ((strlen(msg_str) > 0) && (msg_str[strlen (msg_str) - 1] == '\n'))
            msg_str[strlen (msg_str) - 1] = '\0';

	
        strncpy(msg.txt, msg_str, strlen(msg_str) + 1);
	
        msg.seqn= msg.seqn+1;
        msg.type = verificaTipo(msg.txt);
    	filtro(&msg);    
        
        sendMessage(sockfd, &msg);
	
	} while (msg.type != SAIR);

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

        if (strlen(msg->txt) > 0 && strlen(msg->username) > MINNAME ){
         printf("%s says: %s\n", msg->username, msg -> txt); 
         printf("#: ");
        }
        if (msg->type == ERRO){
        printf("Ocorreu um erro");
        exit(0);
    	}
    }
    
}

int main(int argc, char *argv[]) 
{ 

    check_args(argv);

	username = argv[1];
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 

    //printf("\nPORT: %d", atoi(argv[2]));

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

