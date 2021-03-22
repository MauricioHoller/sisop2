#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include "mesnagem.c"
#include <time.h>

#define PORT 2000

pthread_t sender, receiver; //produtor / consumidor



void main(int argc, char *argv[])
{
	char userid[MAXNAME];
	char *host;
	int port;
	int sockfd = -1, sync_socket = -1;
	int notifyfd;
	int watchfd;
	char buffer[1024] = {0}; 




	if (argc < 3) 
	{
		printf("Insufficient arguments\n");
		exit(0);
	}

	// primeiro argumento nome do usuário
	if (strlen(argv[1]) <= MAXNAME && strlen(argv[1]) > MINNAME)
		{
			strcpy(userid, argv[1]);	
			}
	else {
		printf("Nome de usuario fora do padrao \n");
		exit(0);	
		}
	// segundo argumento host
	host = malloc(sizeof(argv[2]));
	strcpy(host, argv[2]);

	// terceiro argumento porta
	port = atoi(argv[3]);
		
   char oi[256];
  
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); //trocar por argv[2] <--------<------------<----------------<-------------<-- 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  //colocar endereço aqui argv[1]  < -----------------------
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
   	
   login();
  
    return 0; 
} 

void login(int socket) {
    PACOTE mensagem;
    clientMessage(&mensagem, LOGIN, username, "");
    sendMessage(socket, &mensagem);
    readMessage(socket, &mensagem);

}




  //  scanf("%s",oi);
  
  
  //  send(sock , oi , strlen(oi) , 0 ); 
  //  printf("Hello message sent\n"); 
  //  login(sockfd));  
	
  //  printf("%s\n",buffer ); 
	
  
