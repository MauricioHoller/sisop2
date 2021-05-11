#ifndef SERVIDOR_H
#define SERVIDOR_H

void* client_thread (void *socket);
#include "mensagem.h"
#include "cliente.h"

#define FREEDEV -1

#define NO_REPLICAS 3


int initializeClient(int client_socket, char *userid, struct client *client);
void listen_client(int client_socket, char *userid);
int tratamentoFollow(PACOTE *mensagem);
int tratadorSend(PACOTE *mensagem);
int tratador_meSeguem(PACOTE *mensagem);

void tratamentoQuit(int client_socket, PACOTE *mensagem);
void trataNotificacao(char *usuario_a_receber, char *usuario_enviou, char *txt);
void enviaMSGNotificacao(void);

void carregaTabelaNotificacoes();
void updateData();


void* replica_listen_thread(void * port); 
void  update_replicas();
void  client_main_loop(int listen_port);

#endif
