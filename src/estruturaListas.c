
#include "estruturaListas.h"
#include "servidor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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



	return;
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

SystemDataList *create_system_data_list()
{
	SystemDataList *s_dlist = (SystemDataList*) malloc(sizeof(SystemDataList));

	s_dlist -> system_data = NULL;

	s_dlist -> next = NULL;

	return s_dlist;
}

void insert_system_data_list(SystemDataList *s_list, SystemDataNode *s_node) 
{
	SystemDataList * current = s_list;

	if ( current -> system_data == NULL)
	{
		current -> system_data = s_node;
		current -> next = create_system_data_list();
		return;
	}

	insert_system_data_list(current -> next, s_node);
}


Client_seguidores *create_seguidores_list()
{
	Client_seguidores *s_list = (Client_seguidores*) malloc(sizeof(Client_seguidores));

	strcpy(s_list -> seguidor, "");

	s_list -> next = NULL;

	return s_list;
}

void insert_seguidores_list(Client_seguidores *s_list, char *seguidor) 
{
	Client_seguidores * current = s_list;

	if ( strcmp(current -> seguidor, "") == 0)
	{
		strcpy( current -> seguidor, seguidor);
		current -> next = create_seguidores_list();
		return;
	}

	insert_seguidores_list(current -> next, seguidor);

}


char* parse_string(char* string) {
    char *aux = (char *) calloc(strlen(string)+1,sizeof(char));
    char *out_str = (char *) calloc(strlen(string)+1,sizeof(char));
            
    aux = strcpy(aux, &string[1]);

    out_str = strncpy(out_str, aux, strlen(aux)-1);
    
    free(aux); 
    aux = NULL;

    return out_str;
}


Client_seguidores * parse_seguidores(char *token)
{

	Client_seguidores * seguidores = create_seguidores_list();

	char * parsed_str = parse_string(token);

	//printf("%s", parsed_str);
	
	char *n_token = strtok(parsed_str, ",");

	while (n_token) 
	{
		//printf("Seguidor: %s\n", n_token);
		insert_seguidores_list (seguidores, n_token);
		n_token = strtok(NULL, ",");

	}

	return seguidores;
}

int count_notification_number(char * string) 
{
	int nro_notificacoes = 0;

	char *parsed_str = parse_string(string);

	char *n_token = strtok(parsed_str, ",");

	while (n_token)
	{
		nro_notificacoes++; 
		n_token = strtok(NULL, ",");
	}

	return nro_notificacoes;

}


int * parse_fila_notificacoes(char *string)
{	


	int index = 0;
	int* arr = (int*) malloc(count_notification_number(string)  * sizeof(int));

	char *parsed_str = parse_string(string);

	char *n_token = strtok(parsed_str, ",");
	
	while (n_token)
	{
		arr[index] = atoi(n_token);
		n_token = strtok(NULL, ",");
		index++;
	}


	return arr;
}

SystemDataList* carregaSystemData(){
	

	FILE *csv_file;
	SystemDataList *s_data;

	csv_file =  fopen(SYSTEM_DATA_FILE, "r");
	
	if (!csv_file) {
		printf("Couldnt open system data file!!");
		return NULL;
	}

	s_data = create_system_data_list();

	int row = 0;
	int col = 0;

	char line_buffer[FILE_LINE_BUFFER];

	while (fgets(line_buffer, FILE_LINE_BUFFER, csv_file)) 
	{
		SystemDataNode *node = (SystemDataNode*) malloc(sizeof(SystemDataNode));

		col = 0;
		row++;

		if (row == 1)
			continue; 
		
		printf("Line: %s\n", line_buffer);

		char *token = strtok(line_buffer, ";");

		while (token) 
		{
			//puts(aux_token);
			switch (col)
			{
			case COL_USERNAME:
				node -> username = parse_string(token);
				break;

			case COL_SEGUIDORES:
				if (strcmp(token, "\"\"") != 0) {
					node -> seguidores = parse_seguidores(token);
				}
				break;
			case COL_NOTIFICACOES:
				if (strcmp(token, "\"\"") != 0) {
					//node -> mensagens = parse_string(token);
				}
				break;
			case COL_FILA_NOTIFICACOES:
				if (strcmp(token, "\"\"") != 0) {
					node -> fila_notificacoes = parse_fila_notificacoes(token);
				}
				break;

			default:
				//printf("Unidentified Column, ignoring...");
				break;
			}

			printf("column: %d token:%s\n", col, token);
			token = strtok(NULL, ";");
			col++;
		}
		insert_system_data_list(s_data, node);
	}

	fclose(csv_file);
	return s_data;
}