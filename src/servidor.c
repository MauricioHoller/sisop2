#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "servidor.h"
#include "mensagem.h"
#include "estruturaListas.h"

#define PORT 2000

struct client_list *client_list; // lista de clientes

int main()
{
   
  int serverSockfd, newsockfd, thread;

  socklen_t cliLen;
  struct sockaddr_in serv_addr, cli_addr;
  NotificationList* n_list = new_notification_list(NULL, NULL);

  pthread_t clientThread; //, syncThread; se precisar

  // inicializa lista de clientes
  new_client_list(client_list);
  // inicializa a lista de notificacoes 
  
  // abre o socket
  if ((serverSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("ERROR opening socket\n");
    return -1;
  }
  // inicializa estrutura do serv_addr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(serv_addr.sin_zero), 8);

  // associa o descritor do socket a estrutura
  if (bind(serverSockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
      printf("ERROR on bindining\n");
      return -1;
    }
  // espera pela tentativa de conexão de algum cliente
  //PACOTE msg;

  listen(serverSockfd, 10);
  	
  cliLen = sizeof(struct sockaddr_in);


  // Server main while
  while(1)
  {

    // socket para atender requisição do cliente
    if((newsockfd = accept(serverSockfd, (struct sockaddr *)&cli_addr, &cliLen)) == -1)
    {
      printf("ERROR on accept\n");
      return -1;
    }

    if (thread)
    {
      // cria thread para atender o cliente
      if(pthread_create(&clientThread, NULL, client_thread, &newsockfd))
      {
        printf("ERROR creating thread\n");
        return -1;
      }
    
    }


    sleep(100);
  }
}


void* client_thread (void *socket)
{   
  int byteCount, connected;
  int *client_socket = (int*)socket;
	char userid[MAXNAME];

  struct client client;
	PACOTE msg, enviar; // colocar na lista de clientes
  	
	//lê os dados de um cliente
	byteCount = readMessage(*client_socket, &msg); 

	// erro de leitura
	connected = 1;
	if (byteCount < 1)
		{
		printf("ERROR reading from socket\n");
		connected = -1;
		}
		
	byteCount = -1;
		
        serverMessage(&enviar, LOGIN, "");
	
	byteCount = sendMessage(*client_socket, &enviar);

    
	if (byteCount < 0)
		printf("ERROR sending connected message\n");
    

	printf("%s connected!\n", msg.username);
    
    	 // inicializa estrutura do client
	if (initializeClient(*client_socket, msg.username, &client) > 0)
	{
	   // avisamos cliente que conseguiu conectar
		connected = 1;
		
		serverMessage(&enviar, LOGIN, "");
		
		byteCount =sendMessage(*client_socket, &enviar);
		
		if (byteCount < 0)
			printf("ERROR sending connected message\n");

		printf("%s connected!\n", userid);
	}
	else
	{
		// avisa cliente que não conseguimos conectar
		connected = 0;
		
		serverMessage(&enviar, ERRO, "");
		
		byteCount = sendMessage(*client_socket, &enviar); 
    
		if (byteCount < 0)
			printf("ERROR sending connected message\n");

		return NULL;
  	}
   
     listen_client(*client_socket, userid);
 /* */      	
}

int initializeClient(int client_socket, char *userid, struct client *client)
{
  int i;  
  struct client_list *client_node; //vai apontar para o nodo da lista com o cliente se ele existir

  // não encontrou na lista ---- NEW CLIENT
  if (!find_client_node(userid, client_list, &client_node))
  {
    client->devices[0] = client_socket;
    client->devices[1] = -1;
    strcpy(client->username, userid);

    // insere cliente na lista de client
    insert_client_list(&client_list, *client);
  }
  // encontrou CLIENT na lista, atualiza device
  else
  {
    if(client_node->client.devices[0] == FREEDEV) // se o primeiro slot de devices do cliente estiver vazio
    {
      client_node->client.devices[0] = client_socket;
    }
    else if (client_node->client.devices[1] == FREEDEV) // se o segundo slot de devices do cliente estiver vazio
    {
      client_node->client.devices[1] = client_socket;
    }
    // caso em que cliente já está conectado em 2 dipostivos
    else
      return -1;
  }
  return 1;

}

void listen_client(int client_socket, char *userid)
{
	PACOTE *mensagem;
	int byteCount;
  do
  {
      byteCount = readMessage(client_socket, mensagem);

      if (byteCount < 0)
        printf("ERROR listening to the client\n");

      switch (mensagem->type)
      {
          case SEND: 
            if (mensagem->txt != NULL ) {
             
              printf("%s says: '%s' to his followers\n", mensagem->username, mensagem -> txt); 
              //printf("Adding this message to notification_list");
            } 
            break;

          case QUIT:
            printf("%s quits its session\n", mensagem->username);
            break;

          case FOLLOW:
            printf("%s follows %s\n", mensagem->username, mensagem->txt);
            break;

          case LOGIN:
            printf("%s logs in\n", mensagem->username);
            break;

          default:
            printf("Invalid command!\n");
            break;
      }
      
  } while( mensagem -> type != QUIT );

  printf("Quiting");
}








