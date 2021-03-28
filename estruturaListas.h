#ifndef ESTRUTURAlISTA_H
#define ESTRUTURAlISTA_H

#include <pthread.h>
#include "servidor.h"

#define MAXNAME 20

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




#endif







