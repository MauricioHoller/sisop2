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
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "servidor.h"
#include "mensagem.h"
#include "estruturaListas.h"

#define PORT 2000

bool is_primary;

int priority;

char master[3][16] = {"127.0.0.1", "2000", "2001"};

// 0: IP, 1: LISTEN_CLIENT_PORT 2: LISTEN_REPLICA_PORT
char replicas[NO_REPLICAS][3][16] = {{"127.0.0.1", "2000", "2001"}, {"127.0.0.1", "3000", "3001"}, {"127.0.0.1", "4000", "4001"}};

int listen_replica_port;
int listen_client_port;

Client_list *client_list; // lista de clientes

pthread_mutex_t lock_insert;

SystemDataList *system_data;

pthread_mutex_t system_data_mutex;

pthread_mutex_t lock_file_write;

struct mensagensAEnviar *notificacao;

void check_args(char *argv[])
{
  if (argv[1] == NULL || argv[2] == NULL || argv[2] == NULL)
  {
    printf("Not enough args!\n");
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  check_args(argv);

  listen_client_port = atoi(argv[1]);
  listen_replica_port = atoi(argv[2]);

  pthread_t replica_thread;

  if (pthread_create(&replica_thread, NULL, replica_listen_thread, argv[2]))
  {
    printf("ERROR creating replica listen thread\n");
    exit(-1);
  }

  client_main_loop(atoi(argv[1]));
}

void client_main_loop(int port)
{

  int serverSockfd, newsockfd, thread;

  socklen_t cliLen;
  struct sockaddr_in serv_addr, cli_addr;

  pthread_t clientThread;

  if (pthread_mutex_init(&lock_insert, NULL) != 0)
    printf("Couldnt initialize lock insert mutex");

  if (pthread_mutex_init(&system_data_mutex, NULL) != 0)
    printf("Couldnt initialize system data mutex");

  if (pthread_mutex_init(&lock_file_write, NULL) != 0)
    printf("Couldnt initialize lock file write mutex");

  // inicializa lista de clientes
  newList(client_list);

  // abre o socket
  if ((serverSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("ERROR opening socket\n");
    exit(-1);
  }
  // inicializa estrutura do serv_addr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(serv_addr.sin_zero), 8);

  // associa o descritor do socket a estrutura
  if (bind(serverSockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("ERROR on binding\n");
    exit(-1);
  }

  listen(serverSockfd, 10);

  cliLen = sizeof(struct sockaddr_in);

  printf("Listening port %d for client connections....\n", port);
  while (1)
  {

    // socket para atender requisição do cliente
    if ((newsockfd = accept(serverSockfd, (struct sockaddr *)&cli_addr, &cliLen)) == -1)
    {
      printf("ERROR on accept\n");
      exit(-1);
    }

    // cria thread para atender o cliente
    if (pthread_create(&clientThread, NULL, client_thread, &newsockfd))
    {
      printf("ERROR creating thread\n");
      exit(-1);
    }
  }

  pthread_mutex_destroy(&lock_insert);
  pthread_mutex_destroy(&system_data_mutex);
  pthread_mutex_destroy(&lock_file_write);
}


void *replica_listen_thread(void *listen_port)
{

  int port = atoi((char *)listen_port);

  int serverSockfd, newsockfd, thread;

  PACOTE msg;
  socklen_t cliLen;
  struct sockaddr_in serv_addr, cli_addr;

  // abre o socket
  if ((serverSockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    printf("ERROR replica listen opening socket\n");
    exit(-1);
  }
  // inicializa estrutura do serv_addr
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  bzero(&(serv_addr.sin_zero), 8);

  // associa o descritor do socket a estrutura
  if (bind(serverSockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("ERROR on binding\n");
    exit(-1);
  }

  listen(serverSockfd, 10);

  cliLen = sizeof(struct sockaddr_in);

  printf("Listening port %d for replica connections....\n", port);
  while (1)
  {

    newsockfd = accept(serverSockfd, (struct sockaddr *)&cli_addr, &cliLen);
    if (newsockfd == -1)
    {
      printf("ERROR on accept\n");
      exit(-1);
    }

    int byteCount = readMessage(newsockfd, &msg);

    if (byteCount < 0)
    {
      printf("Error listening socket");
      break;
    }

    if (strlen(msg.txt) > 0 && strlen(msg.user_sent) > 0)
    {
      printf("Synchronized: (\"%s\") from user %s to user %s\n", msg.txt, msg.user_sent, msg.username);

      trataNotificacao( msg.username, msg.user_sent, msg.txt);

    }
  }
}

int connect_replica(char *ip, char *port)
{
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Socket creation error para replica %s:%s \n", ip, port);
    return -1;
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(port));

  if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
  {
    printf("Invalid address %s\n", ip);
    return -1;
  }
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("Erro ao se conectar a replica %s:%s \n", ip, port);
    return -1;
  }

  return sock;
}

void update_replica(char *ip, char *port)
{

  struct mensagensAEnviar *aux;
  struct mensagensAEnviar *aux2;

  // TODO: send this via tcp socket
  if (notificacao != NULL)
  {

    aux = notificacao;

    if (notificacao != NULL)
    {

      while (aux != NULL)
      {

        int sock = connect_replica(ip, port);

        if (sock == -1)
        {
          return;
        }

        PACOTE *msg = malloc(sizeof(PACOTE));

        // The user who sent
        strcpy(msg->user_sent, aux->usuario_envio);
        // User that will receive the message
        strcpy(msg->username, aux->usuario);
        // Msg Text
        strcpy(msg->txt, aux->txt);

        printf("Stored message (\"%s\") from user %s to user %s\n", msg->txt, msg->user_sent, msg->username);

        sendMessage(sock, msg);

        aux = aux->prox;
      }
    }
  }
}

void update_replicas()
{


  pthread_mutex_lock(&lock_insert);
  for (int i = 0; i < NO_REPLICAS; i++)
  {

    if (strcmp(replicas[i][2], master[2]) == 0)
    {
      continue;
    }

    update_replica(replicas[i][0], replicas[i][2]);
  }
  pthread_mutex_unlock(&lock_insert);

}

void escreveTabelaNotificacoes(Client_list *client)
{
  pthread_mutex_lock(&lock_insert);
  pthread_mutex_lock(&lock_file_write);

  FILE *csvfile;
  //char writebuffer[]

  csvfile = fopen(SYSTEM_DATA_FILE, "w");

  if (csvfile == NULL)
  {
    printf("Couldnt append to file");
    return;
  }

  fprintf(csvfile, "Username;Seguidores;Notificacoes;Fila_Notificacoes\n");

  Client_list *current_clist = client;

  while (current_clist != NULL)
  {

    fprintf(csvfile, "\"%s\";", current_clist->client.username);

    Client_seguidores *current_seguidores = current_clist->seguidores;

    fprintf(csvfile, "\"");

    while (current_seguidores != NULL)
    {

      //printf("writing seguidor \n");
      fprintf(csvfile, ",%s", current_seguidores->seguidor);

      current_seguidores = current_seguidores->next;
    }

    fprintf(csvfile, "\";\"\";\"\";\"\"\n");

    current_clist = current_clist->next;
  }

  fclose(csvfile);

  pthread_mutex_unlock(&lock_file_write);
  pthread_mutex_unlock(&lock_insert);
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

    printf("%s connected! via socket: %d\n", userid, *client_socket);
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

  escreveTabelaNotificacoes(client_list);
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

    switch (mensagem->type)
    {
    case SEND:
      if (mensagem->username != NULL)

        tratadorSend(mensagem);

      break;
    case FOLLOW:

      tratamentoFollow(mensagem);
      escreveTabelaNotificacoes(client_list);

      break;

    case ERRO:
      printf("Something Unexpected happened...\n");
      //tratamentoQuit(client_socket, mensagem);

      break;
    case SAIR:
      printf("UM device de %s saiu\n", mensagem->username);
      tratamentoQuit(client_socket, mensagem);
      break;

      //default: printf("ERROR invalid command\n");
    }

  } while (mensagem->type != QUIT);
}

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

    usuario_mandou_msg->seguidores = malloc(sizeof(struct seguidores));

    strncpy(usuario_mandou_msg->seguidores->seguidor, mensagem->txt, strlen(mensagem->txt) + 1);

    usuario_mandou_msg->seguidores->next = NULL;

    printf("Seguindo o primeiro: %s \n", mensagem->txt);

    pthread_mutex_unlock(&lock_insert);
  }
  else
  {

    pthread_mutex_lock(&lock_insert);

    aux2 = usuario_mandou_msg->seguidores;

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

    aux2 = malloc(sizeof(struct seguidores));

    strncpy(aux2->seguidor, mensagem->txt, strlen(mensagem->txt) + 1);

    aux2->next = NULL;

    previous->next = aux2;

    pthread_mutex_unlock(&lock_insert);

    printf("Seguindo mais um: %s \n", mensagem->txt);

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

  while (aux != NULL)
  {

    aux2 = aux->seguidores;
    if (strcmp(aux->client.username, mensagem->username) != 0)
    {
      while (aux2 != NULL)
      { //verifica todos os seguidores de todos os clientes

        if (strcmp(aux2->seguidor, mensagem->username) == 0)
          if (findNode(aux->client.username, client_list, &enviar_msg) == 1)
          {
            if (enviar_msg->client.devices[0] != FREEDEV)
              sendMessage(enviar_msg->client.devices[0], mensagem);
            else if (enviar_msg->client.devices[1] != FREEDEV)
              sendMessage(enviar_msg->client.devices[1], mensagem);
            else
            {
              trataNotificacao(aux->client.username, mensagem->username, mensagem->txt);
              update_replicas();
            }
          }
        aux2 = aux2->next;
      }
    }
    aux = aux->next;
  }

  return 0;
}

void trataNotificacao(char *usuario_a_receber, char *usuario_enviou, char *txt)
{

  struct mensagensAEnviar *aux;
  struct mensagensAEnviar *aux2;

  aux = notificacao;

  if (notificacao != NULL)
  {

    while (aux != NULL)
    {

      aux2 = aux;

      aux = aux2->prox;
    }
    pthread_mutex_lock(&lock_insert);

    aux = malloc(sizeof(struct mensagensAEnviar));

    strcpy(aux->usuario, usuario_a_receber);
    strcpy(aux->usuario_envio, usuario_enviou);
    strcpy(aux->txt, txt);

    aux->prox = NULL;

    aux2->prox = aux;
    printf("mensagem guardada de %s para %s \n", aux->usuario_envio, aux->usuario);
    pthread_mutex_unlock(&lock_insert);
  }
  else
  {

    pthread_mutex_lock(&lock_insert);

    notificacao = malloc(sizeof(struct mensagensAEnviar));

    strcpy(notificacao->usuario, usuario_a_receber);
    strcpy(notificacao->usuario_envio, usuario_enviou);
    strcpy(notificacao->txt, txt);

    notificacao->prox = NULL;
    printf("mensagem guardada de %s para %s \n", notificacao->usuario_envio, notificacao->usuario);
    pthread_mutex_unlock(&lock_insert);
  }
}

void enviaMSGNotificacao(void)
{

  struct mensagensAEnviar *aux;
  int cont = 0;
  struct mensagensAEnviar *previous;
  PACOTE *msg;
  struct client_list *enviar_msg;
  aux = notificacao;
  int enviou = -1;
  previous = NULL;
  printf("Seeing if there are notifications to send\n");
  if (notificacao != NULL)
  {
    printf("Seems that there are notifications :-)\n");
    pthread_mutex_lock(&lock_insert);

    while (aux != NULL)
    {

      if (findNode(aux->usuario, client_list, &enviar_msg) == 1)
      {
        msg = malloc(sizeof(PACOTE));
        strcpy(msg->username, aux->usuario_envio);
        strcpy(msg->txt, aux->txt);

        if (enviar_msg->client.devices[0] != FREEDEV)
        {

          previous = aux->prox;

          free(aux);

          aux = previous;

          printf("Sending msg %s via socket %d\n", msg->txt, enviar_msg->client.devices[0] );
          sendMessage(enviar_msg->client.devices[0], msg);
        }
        else if (enviar_msg->client.devices[1] != FREEDEV)
        {
          sendMessage(enviar_msg->client.devices[1], msg);

          previous = aux->prox;

          free(aux);

          aux = previous;
        }
        else
        {
          previous = aux;
          aux = aux->prox;
        }
        free(msg);
      }
      pthread_mutex_unlock(&lock_insert);
    }
  }
}

void tratamentoQuit(int client_socket, PACOTE *mensagem)
{

  struct client_list *usuario_mandou_msg;
  struct client_list *enviar_msg;

  struct client_list *aux;
  struct seguidores *aux2;
  PACOTE msg;
  if (findNode(mensagem->username, client_list, &usuario_mandou_msg) == 0) //client list é variavel global
    exit(0);                                                               //erro

  msg.type = ERRO;
  strcpy(msg.username, "server");
  strcpy(msg.txt, "");
  msg.timestamp = time(NULL);
  pthread_mutex_lock(&lock_insert);

  if (usuario_mandou_msg->client.devices[0] == client_socket)
  {

    usuario_mandou_msg->client.devices[0] = FREEDEV;
  }

  if (usuario_mandou_msg->client.devices[1] == client_socket)
  {

    usuario_mandou_msg->client.devices[1] = FREEDEV;
  }

  pthread_mutex_unlock(&lock_insert);
}
