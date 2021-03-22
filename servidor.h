#ifndef SERVIDOR_H
#define SERVIDOR_H



void* client_thread (void *socket);
#include "mensagem.h"

#define FREEDEV -1
#define MAXNAME 20
#define MINNAME 3

/* User client struct */
typedef struct client {
    int socket;                         // user current socket
    pthread_t thread;                   // session thread
    pthread_mutex_t mutex;              // session socket mutex
    char username[MAXNAME];   // user name
    int devices[2];	//quantidade de devices logados	
} Cliente;

/*
Cliente* createCliente(int socket, char* username); 
void deleteCliente(Cliente* s);
void freeCliente(Cliente *s); 
void ClienteRun(Cliente *s, void *thread (void*)); 
int ClientSendMessage(Cliente *s, PACOTE *msg);
int ClientReadMessage(Cliente *s, PACOTE *msg); 
int startClient(int socket);

*/

int initializeClient(int client_socket, char *userid, struct client *client);
void listen_client(int client_socket, char *userid);



#endif
