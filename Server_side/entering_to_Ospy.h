#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
	
#define MAX_CLIENTS 30
#define MAX_USER_NAME_LEN 32
#define LICENSE_KEY_LENGTH 20
#define HASH_LEN 34
#define HASH_SALT_LEN 5
#define HASH_SALT "Ospyy"

typedef enum {
	SUCCESS,
	FAILUR
} STATUS;

typedef enum {
	REGISTERED_SUCCESSFULLY,
	USERNAME_TAKEN,
	INVALID_USERNAME,
	LICENES_KEY_INVALID,
	PASSWORD_VERIFICTION_INVALID
} registration_status;

typedef struct {
	char licenses_key[LICENSE_KEY_LENGTH];
} licenses_key_item;

typedef struct {
	char username[MAX_USER_NAME_LEN];
	char password[HASH_LEN];
	int active;
} attacker_info;
 
registration_status register_new_attacker(char licenes_key[], char username[], char password[]);
STATUS add_attacket_to_attackers_list(char username_input[], char password_input[]);
STATUS log_in_attacker(char username_input[], char password_input[]);
STATUS licenses_key_validation (char licenes_key_input[]);
licenses_key_item add_new_license_key();
char * salted_hashed_password(char hashed_password_input[]);
int attacker_exsist_in_list(char username_input[]);
void remove_license_key (char licenes_key[]);
void print_all_license_key();