#ifndef ESTRUTURAlISTA_H
#define ESTRUTURAlISTA_H

#include <pthread.h>
#include "mensagem.h"
#include "cliente.h"
#include "servidor.h"

#define MAXNAME 20

#define COL_USERNAME          0
#define COL_SEGUIDORES        1
#define COL_NOTIFICACOES      2
#define COL_FILA_NOTIFICACOES 3

#define SYSTEM_DATA_FILE "./system_data.csv"

#define FILE_LINE_BUFFER 16384

#define MAX_NOTIFICACOES 100

typedef struct system_data_node {

    char* username;
    char** mensagens;

    int* fila_notificacoes;
    int total_de_notificacoes;
    struct seguidores * seguidores;

} SystemDataNode; 

typedef struct system_data_list {

    struct system_data_list *next;
    SystemDataNode * system_data;

} SystemDataList; 

typedef struct seguidores {
    struct seguidores *next; //proximo da lista
    char seguidor[MAXNAME]; //cliente
} Client_seguidores;


typedef struct client_list {
    struct client_list * next; //proximo da lista
    Cliente client; //cliente

    struct seguidores *me_seguem;
    struct seguidores *seguidores;	
} Client_list;





void newList(struct client_list *client_list);
void insertList(struct client_list **client_list, struct client client);
int isEmpty(struct client_list *client_list);
int findNode(char *userid, struct client_list *client_list, struct client_list **client_node);

SystemDataList* carregaSystemData();
SystemDataList *create_system_data_list();

void insert_system_data_list(SystemDataList *s_list, SystemDataNode *s_node);

char* parse_string(char* string) ;

char** parse_notificacoes(char *token);

int* parse_fila_notificacoes(char *token);


Client_seguidores *create_seguidores_list();
void insert_seguidores_list(Client_seguidores *s_list, char *seguidor) ;
Client_seguidores * parse_seguidores(char *token);

#endif







