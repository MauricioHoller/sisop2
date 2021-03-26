#ifndef ESTRUTURAlISTA_H
#define ESTRUTURAlISTA_H

#include <pthread.h>
#include "servidor.h"

typedef struct client_list {
    struct client_list * next; //proximo da lista
    Cliente client; //cliente
    pthread_mutex_t mutex; // mutex do cliente atual
} Client_list;

void newList(struct client_list *client_list);
void insertList(struct client_list **client_list, struct client client);
int isEmpty(struct client_list *client_list);
int findNode(char *userid, struct client_list *client_list, struct client_list **client_node);




#endif







