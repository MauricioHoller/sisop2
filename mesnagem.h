#ifndef MENSAGEM_H
#define MENSAGEM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <pwd.h>



#define MAXNAME 20
#define MINNAME 3

#define FOLLOW 0
#define SEND 1
#define QUIT 2
#define LOGIN 3



int readMessage(int socket, PACOTE *msg);
int sendMessage(int socket, PACOTE *msg);
void serverMessage(PACOTE* msg, int type, char* text);
void serverMessage(PACOTE* msg, int type, char* text);
void clientMessage(PACOTE* msg, int type, char* username, char* text) {




 typedef struct packet {
      uint16_t type; // tipo de pacote
      uint16_t seqn; // numero de sequencia
      uint16_t lenght; // comprimento do payload
      uint16_t timestamp; // timestamp do dado
      const char* dados; //dados da mensagem
      char username[MAXNAME];
     } PACOTE;

 typedef struct notification {
      uint32_t id; // identificador
      uint32_t timestamp; // timestamp da notificação
      const char* mensagem; //mensagem
      uint16_t lenght; // tamanho da mensagem
      uint16_t pendentes; // quantidade de leitores pendentes
     } MENSAGEM;





#endif







