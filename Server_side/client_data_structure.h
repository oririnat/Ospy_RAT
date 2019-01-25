#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_LEN 34
#define MAX_USER_NAME_LEN 32

typedef enum {
	ATTACKER, 
	VICTIM
} sender;

//  to delet it
typedef struct {
	sender i_am;
	int socket_fd;
	int other_side_sfd; // for the attakcer 'other_side_sfd' means the socket file descriptor of his courent victim.
					   //  for the victim 'other_side_sfd' means the socket file descriptor of his attcker.
	char id[HASH_LEN];
	char name[MAX_USER_NAME_LEN];
} client_delet;

typedef struct client_node {
	sender i_am;
	int socket_fd;
	int other_side_sfd; // for the attakcer 'other_side_sfd' means the socket file descriptor of his courent victim.
					   //  for the victim 'other_side_sfd' means the socket file descriptor of his attcker.
	char id[HASH_LEN];
	char name[MAX_USER_NAME_LEN];	
	struct client_node * next_client;
} client, * client_ptr;

// global list
client_ptr clients;
void print_clients_list(client_ptr head);
void insert_client(sender i_am, int socket_fd, int other_side_sfd, char id[], char name[], client_ptr * head);
void remove_client(int socket_id, client_ptr * head);
