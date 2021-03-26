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
    int seqn=0;	
    //teste
	PACOTE msg;

    clientMessage(&msg, SEND, username, "aiapos");

    // === ESTAVA FAZENDO NA MAO GRANDE ===
	//strncpy(msg.username, username, strlen(username) + 1);

	//msg.type = SEND;
	
    //char *asdf = "Mama eu";

    //msg.dados = strdup(asdf);

    while(1){
	
	msg.seqn= msg.seqn+1;
	sendMessage(sock, &msg);
	sleep(10);
    
    } 
		
	//pthread_create(&sender, NULL, produtor, &sock);	
    
    //  while(1){

    // }



    produtor(sock);

    return 0; 
} 


void *produtor(int sock) {
    
    int sockfd =  sock;
    
    PACOTE msg;
    do {
        
        //char msg_str[255];
        //scanf("%s", msg_str);
        
        //msg.dados = strdup(msg_str);
        msg.seqn= msg.seqn+1;

        puts(msg.dados);
        sendMessage(sockfd, &msg);
	
	} while (msg.type != QUIT);
  
    close(sockfd);
    
    return NULL;
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

