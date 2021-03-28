
#include "estruturaListas.h"
#include "servidor.h"
#include <stdlib.h>
#include <string.h>



void newList(struct client_list *client_list)
{
	client_list = NULL;
}

void insertList(struct client_list **client_list, struct client client)
{
	struct client_list *client_node;
	struct client_list *client_list_aux = *client_list;

	client_node = malloc(sizeof(struct client_list));

	client_node->client = client;
	client_node->next = NULL;

	if (*client_list == NULL)
	{
		*client_list = client_node;
	}
	else
	{
		while(client_list_aux->next != NULL)
			client_list_aux = client_list_aux->next;

		client_list_aux->next = client_node;
	}
}

int isEmpty(struct client_list *client_list)
{
	if (client_list== NULL)
		return 0;
	
	return 1;

}

int findNode(char *userid, struct client_list *client_list, struct client_list **client_node)
{
	struct client_list *client_list_aux = client_list;
	
	while(client_list_aux != NULL)
	{
		if (strcmp(userid, client_list_aux->client.username) == 0)
		{
			*client_node = client_list_aux;
			return 1;
		}
		else
			client_list_aux = client_list_aux->next;
	}
	return 0;
}

