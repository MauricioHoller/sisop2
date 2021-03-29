#ifndef SERVIDOR_H
#define SERVIDOR_H

void* client_thread (void *socket);
#include "mensagem.h"
#include "cliente.h"

#define FREEDEV -1



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
int tratamentoFollow(PACOTE *mensagem);
int tratadorSend(PACOTE *mensagem);
int tratador_meSeguem(PACOTE *mensagem);

void tratamentoQuit(int client_socket, PACOTE *mensagem);
//void trataNotificacao(PACOTE *mensagem, char *usuario_a_receber);
void trataNotificacao(char *usuario_a_receber, char *usuario_enviou, char *txt);
void enviaMSGNotificacao(void);

//void escreveTabelaNotificacoes(Client_list * cliente);
void carregaTabelaNotificacoes();

#endif
