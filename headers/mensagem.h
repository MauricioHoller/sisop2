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

#define MSG_MAX_SIZE 280

#define FOLLOW 5
#define SEND 1
#define QUIT 2
#define LOGIN 3
#define ERRO 4
#define SAIR 7
#define REPLICA_UPDATE 8

 typedef struct packet {
      uint16_t type; // tipo de pacote
      uint16_t seqn; // numero de sequencia
      uint16_t length; // comprimento do payload
      uint16_t timestamp; // timestamp do dado 

      char user_sent[MAXNAME];    
      char username[MAXNAME];
      char txt[MSG_MAX_SIZE];

     } PACOTE;



void clientMessage(PACOTE* msg, int type, char* username, char* text);
void serverMessage(PACOTE* msg, int type, char* text);
int sendMessage(int socket, PACOTE *msg);
int readMessage(int socket, PACOTE *msg);


#endif







