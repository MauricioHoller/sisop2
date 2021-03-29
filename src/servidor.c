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

Client_list *client_list; // lista de clientes

pthread_mutex_t lock_insert;

SystemDataList* system_data;

pthread_mutex_t system_data_mutex;

struct mensagensAEnviar *notificacao;


int main()
{

  system_data = carregaSystemData();
  
  int serverSockfd, newsockfd, thread;

  socklen_t cliLen;
  struct sockaddr_in serv_addr, cli_addr;

  pthread_t clientThread; //, syncThread; se precisar

  int ret = pthread_mutex_init(&lock_insert, NULL);
  if (ret != 0)
    printf("Couldnt initialize mutex");

  // inicializa lista de clientes
  newList(client_list);

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

  listen(serverSockfd, 10);

  cliLen = sizeof(struct sockaddr_in);

  while (1)
  {

    // socket para atender requisição do cliente
    if ((newsockfd = accept(serverSockfd, (struct sockaddr *)&cli_addr, &cliLen)) == -1)
    {
      printf("ERROR on accept\n");
      return -1;
    }

    if (thread)
    {
      // cria thread para atender o cliente
      if (pthread_create(&clientThread, NULL, client_thread, &newsockfd))
      {
        printf("ERROR creating thread\n");
        return -1;
      }
    }
  }
  pthread_mutex_destroy(&lock_insert);
}

void *client_thread(void *socket)
{
  int byteCount, connected;
  int *client_socket = (int *)socket;
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

  // inicializa estrutura do client
  if (initializeClient(*client_socket, msg.username, &client) > 0)
  {
    // avisamos cliente que conseguiu conectar
    connected = 1;

    serverMessage(&enviar, LOGIN, "");

    byteCount = sendMessage(*client_socket, &enviar);

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

  printf("Initialize %s", userid);
  // não encontrou na lista ---- NEW CLIENT
  if (!findNode(userid, client_list, &client_node))
  {
    client->devices[0] = client_socket;
    client->devices[1] = FREEDEV;
    strcpy(client->username, userid);

    // insere cliente na lista de client
    pthread_mutex_lock(&lock_insert);

    insertList(&client_list, *client);

    pthread_mutex_unlock(&lock_insert);

    client_list->seguidores = NULL; //Primeira vez que o cliente entra no servidor é colocado seguidores como 0
    client_list->me_seguem = NULL;
  }
  // encontrou CLIENT na lista, atualiza device
  else
  {
    if (client_node->client.devices[0] == FREEDEV) // se o primeiro slot de devices do cliente estiver vazio
    {
      client_node->client.devices[0] = client_socket;
      return 1;
    }
    else if (client_node->client.devices[1] == FREEDEV) // se o segundo slot de devices do cliente estiver vazio
    {
      client_node->client.devices[1] = client_socket;
      return 1;
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
	enviaMSGNotificacao();
  do
  {

    byteCount = readMessage(client_socket, mensagem);

    if (byteCount < 0)
      printf("ERROR listening to the client\n");

    //	printf("%s says: %s\n", mensagem->username, mensagem -> txt);

    switch (mensagem->type)
    {
    case SEND:
      if (mensagem->dados != NULL)
        //printf("%s says: %s\n", mensagem->username, mensagem -> txt);
        //              tratadorSend(mensagem);
        tratadorSend(mensagem);

      break;
    case FOLLOW:
      tratamentoFollow(mensagem);

      //tratador_meSeguem(mensagem);
      break;

      case ERRO:printf("entrou no ERRO \n " ); tratamentoQuit(client_socket, mensagem);break;
 
      //default: printf("ERROR invalid command\n");
    }

  } while (mensagem->type != QUIT);
}

/*a MENSAGEM RECEBIDA JA DEVE SER FILTRADA DO COMANDO ---FOLLOW--- RETIRANDO ELA DO TEXTO
	TAVA FUNCIONANDO ATÉ FALTAR LUZ E EU PERDER ALGO QUE NAO SEI O QUE É

*/
int tratamentoFollow(PACOTE *mensagem)
{

  struct client_list *usuario_mandou_msg;

  struct client_list *aux;

  struct seguidores *aux2;

  struct seguidores *previous;

  //verifica se o cliente ja esta em execução
  if (findNode(mensagem->username, client_list, &usuario_mandou_msg) == 0) //client list é variavel global
  {
    printf("Cliente não inicializado com exito\n");
    return -1; // cliente não inicializado
  }

  if (findNode(mensagem->txt, client_list, &aux) == 0)
  {

      printf("Usuario inexistente para seguir \n");

      return -1; // Não existe Usuario com esse nome para seguir
  }

  if (usuario_mandou_msg->seguidores == NULL)
  { // se ainda não seguiu ninguem


    pthread_mutex_lock(&lock_insert);
       
    usuario_mandou_msg -> seguidores = malloc(sizeof(struct seguidores));

    strncpy(usuario_mandou_msg -> seguidores->seguidor, mensagem->txt, strlen(mensagem->txt) + 1);

    usuario_mandou_msg-> seguidores -> next = NULL;

    printf("Seguindo o primeiro: %s \n", mensagem->txt);
    
    pthread_mutex_unlock(&lock_insert);

  }
  else
  {

    aux2 = usuario_mandou_msg -> seguidores;
    

    while (aux2 != NULL)
    {

      if (strcmp(aux2->seguidor, mensagem->txt) == 0)
      {

        printf("Já é um seguidor \n");

        return -1;
      }

      previous = aux2;
      aux2 = aux2->next;


    }

    pthread_mutex_lock(&lock_insert);

    aux2 = malloc(sizeof(struct seguidores));

    strncpy(aux2 -> seguidor, mensagem->txt, strlen(mensagem->txt) + 1);

    aux2 -> next = NULL;

    previous -> next = aux2;
    
    pthread_mutex_unlock(&lock_insert);
    
    printf("Seguindo mais um: %s \n", mensagem -> txt);

    return 0;
  }

  return 0;
}

int tratadorSend(PACOTE *mensagem)
{

  //	sendMessage(int socket, PACOTE *msg);

  struct client_list *usuario_mandou_msg;
  struct client_list *enviar_msg;

  struct client_list *aux;
  struct seguidores *aux2;
  //	FUNCIONA MAS FIZ ERRADO AQUI QUEM EU SiGO RECEBE AS MSG E NÂO RECEBO DELAS

  if (findNode(mensagem->username, client_list, &usuario_mandou_msg) == 0) //client list é variavel global
    return -1;                                                             //erro
                                                                           //acha quem mandou a mensagem

  aux2 = usuario_mandou_msg->seguidores;
	aux = client_list;

	while (aux != NULL){

		aux2= aux->seguidores;
		if (strcmp(aux->client.username , mensagem->username)!=0){
	 		while (aux2 != NULL){ //verifica todos os seguidores de todos os clientes
		 		
	 			if (strcmp(aux2->seguidor, mensagem->username)==0)
	 			if (findNode( aux->client.username, client_list , &enviar_msg)==1){				      			
						if (enviar_msg->client.devices[0] != FREEDEV)
						sendMessage(enviar_msg->client.devices[0], mensagem);
						else if (enviar_msg->client.devices[1] != FREEDEV)
						sendMessage(enviar_msg->client.devices[1], mensagem);
			 			else {trataNotificacao(aux->client.username,mensagem->username, mensagem->txt); }
		 		}
		 	aux2=aux2->next;
	 		}
	 	}
	aux = aux->next;
}

  return 0;
}

void trataNotificacao(char *usuario_a_receber, char *usuario_enviou, char *txt){

	struct mensagensAEnviar *aux;
	struct mensagensAEnviar *aux2;
	
	aux = notificacao;
	
	if (notificacao != NULL){	
	
	while (aux!=NULL){
     
      aux2 = aux;
     
      aux = aux2->prox;
	}
    pthread_mutex_lock(&lock_insert);

	aux = malloc(sizeof(struct mensagensAEnviar));
	
	strcpy(aux->usuario,usuario_a_receber);
	strcpy(aux->usuario_envio,usuario_enviou);
	strcpy(aux->txt,txt);
	
	aux->prox=NULL;

   	aux2 -> prox = aux;
 	printf("mensagem guardada de %s para %s \n",aux->usuario_envio,aux->usuario);   
    pthread_mutex_unlock(&lock_insert);
    
}
 else{
 
    pthread_mutex_lock(&lock_insert);

 	notificacao = malloc(sizeof(struct mensagensAEnviar));
	
	strcpy(notificacao->usuario , usuario_a_receber);	
	strcpy(notificacao->usuario_envio,usuario_enviou);
	strcpy(notificacao->txt,txt);
	
	
	notificacao->prox=NULL;
 	printf("mensagem guardada de %s para %s \n",notificacao->usuario_envio,notificacao->usuario);	
  pthread_mutex_unlock(&lock_insert);
    
 }



}

void enviaMSGNotificacao(void){

	struct mensagensAEnviar *aux;
	int cont=0;	
	struct mensagensAEnviar *previous;
	PACOTE *msg;
	struct client_list *enviar_msg;	
	aux = notificacao;
	int enviou=-1;
	previous = NULL;
	
	if (notificacao!=NULL){
        pthread_mutex_lock(&lock_insert);
    	
    		//printf("ENTROU NO LOOP");
	while (aux!=NULL){
  	
	if (findNode( aux->usuario, client_list , &enviar_msg)==1){				      			
      			msg=malloc(sizeof(PACOTE));
      			strcpy(msg->username,aux->usuario_envio);
			strcpy(msg->txt,aux->txt);

			if (cont>0){
			if (enviar_msg->client.devices[0] != FREEDEV){
			
	      		previous = aux->prox;

			free(aux);      	

	      		aux=previous;

			sendMessage(enviar_msg->client.devices[0], msg);
			}
			else if (enviar_msg->client.devices[1] != FREEDEV){
				sendMessage(enviar_msg->client.devices[1], msg);

		      		previous = aux->prox;

				free(aux);      	

		      		aux=previous;
				}
      		      	else{
		      	previous=aux;
			aux=aux->prox;
			}
			}
			cont++;
			free(msg);
	}
	    pthread_mutex_unlock(&lock_insert);
}
}
}


void tratamentoQuit(int client_socket, PACOTE *mensagem){

  struct client_list *usuario_mandou_msg;
  struct client_list *enviar_msg;

  struct client_list *aux;
  struct seguidores *aux2;
  PACOTE msg;
  if (findNode(mensagem->username, client_list, &usuario_mandou_msg) == 0) //client list é variavel global
  		  exit (0);                                                             //erro
  
  msg.type= ERRO;
  strcpy(msg.username, "server");
  strcpy(msg.txt,"");
  msg.timestamp = time(NULL);
 pthread_mutex_lock(&lock_insert);
   	
  if (usuario_mandou_msg->client.devices[0] == client_socket){
  	
  	//sendMessage(enviar_msg->client.devices[0], &msg);
  	usuario_mandou_msg->client.devices[0] =FREEDEV;
  	
  }
  
  if (usuario_mandou_msg->client.devices[1] == client_socket){
  	
  	//sendMessage(enviar_msg->client.devices[1], &msg);
  	usuario_mandou_msg->client.devices[1] = FREEDEV;
  }
  /*
  printf("%d \n ", usuario_mandou_msg->client.devices[0]);
  printf("%d \n ",usuario_mandou_msg->client.devices[1] );
/*  close(client_socket);
 */
 pthread_mutex_unlock(&lock_insert);
   

}
