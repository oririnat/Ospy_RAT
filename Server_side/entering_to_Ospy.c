#include "entering_to_Ospy.h"
#include "md5.h"
#include <stdbool.h>
//* registration handling  *//

bool valid_attacker_name (char * victim_name){
	for (int i = 0; i < strlen(victim_name); i++){
		if (!((victim_name[i] >= 'a' && victim_name[i] <= 'z') || (victim_name[i] >= 'A' && victim_name[i] <= 'Z') || victim_name[i] == '_' || (victim_name[i] >= '0' && victim_name[i] <= '9')))
			return false;
	}
	return true;
}

registration_status register_new_attacker(char licenes_key_input[], char username_input[], char hashed_password_input[]) {
	if (valid_attacker_name(username_input)){
		if(licenses_key_validation(licenes_key_input) == SUCCESS){
			if (add_attacket_to_attackers_list(username_input, hashed_password_input) == FAILUR)
				return USERNAME_TAKEN;
		}
		else
			return LICENES_KEY_INVALID;
	}
	else
		return INVALID_USERNAME;

	remove_license_key(licenes_key_input);
	return REGISTERED_SUCCESSFULLY;
}

licenses_key_item add_new_license_key () { 	
	FILE * licenses_key_list =  fopen("licenses_key_list.dat", "a");
	char characters_dictionary[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*1234567890";
	const int num_of_segments = 4;
	int index = 0;
	char licenses_key_template[] = "XXXX-XXXX-XXXX-XXXX";
	for (int i = 0; i < num_of_segments; i++) {
		for (int j = 0; j < num_of_segments; j++, index++)
			licenses_key_template[index] = characters_dictionary[rand() % 44]; // add random character to the license key
		index ++;
	}
	licenses_key_item licenses_key;
	strcpy(licenses_key.licenses_key, licenses_key_template);
	fwrite(&licenses_key, sizeof(licenses_key_item), 1, licenses_key_list);
	fclose(licenses_key_list);
	return licenses_key;
}

STATUS licenses_key_validation (char licenes_key_input[]){ // chack if the key exist in the list, if yes remove it
	licenses_key_item temp_licenses_key_item;
	char current_licenes_key[LICENSE_KEY_LENGTH];
	FILE * licenses_key_list;
	licenses_key_list = fopen("licenses_key_list.dat","rb");
	fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenses_key_list);
	while(!feof(licenses_key_list)){
		if(strcmp(temp_licenses_key_item.licenses_key,licenes_key_input) == 0)
			return SUCCESS;
		fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenses_key_list);
	} 
	fclose(licenses_key_list);
	return FAILUR;
}

void remove_license_key (char licenes_key_input[]) {
	licenses_key_item temp_licenses_key_item;
	FILE * licenes_key_list, * temp_licenses_key_list;
	licenes_key_list = fopen("licenses_key_list.dat","rb");
	temp_licenses_key_list = fopen("temp_licenses_key_list.dat","wb");
	fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenes_key_list);
	while(!feof(licenes_key_list)){
		if(strcmp(temp_licenses_key_item.licenses_key,licenes_key_input) != 0 )
			fwrite(&temp_licenses_key_item, sizeof(licenses_key_item), 1, temp_licenses_key_list);
		fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenes_key_list);
	}
	fclose(temp_licenses_key_list);
	fclose(licenes_key_list);
	
	licenes_key_list = fopen("licenses_key_list.dat","wb");
	temp_licenses_key_list = fopen("temp_licenses_key_list.dat","rb");
	
	fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, temp_licenses_key_list);
	while(!feof(temp_licenses_key_list)){
		fwrite(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenes_key_list);
		fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, temp_licenses_key_list);
	}
	fclose(temp_licenses_key_list);
	fclose(licenes_key_list);
}
//* end registration handling  *//


//* log in handling  *//
STATUS add_attacket_to_attackers_list(char username_input[], char hashed_password_input[]){	
	FILE * attackers_list;
	attacker_info attacker;
	
	if(attacker_exsist_in_list(username_input))
		return FAILUR;
	attackers_list = fopen("attackers_list.dat","ab");
	if(attackers_list == NULL)
		return FAILUR;
	
	strcpy(attacker.username, username_input);
	strcpy(attacker.password, salted_hashed_password(hashed_password_input));
	
	attacker.active = 1; // the attacker is active -> not deleted
	fwrite(&attacker, sizeof(attacker_info), 1, attackers_list);
	fclose(attackers_list);
	return SUCCESS;
}

int attacker_exsist_in_list(char username_input[]){
	attacker_info attacker;
	FILE * attackers_list;
	attackers_list = fopen("attackers_list.dat","rb");
	if(attackers_list ==  NULL)
		return 0;
	fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	while(!feof(attackers_list)){
		if(strcmp(attacker.username,username_input) == 0){
			return 1;
		}
		fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	}
	fclose(attackers_list);
	return 0;
}

STATUS log_in_attacker(char username_input[], char hashed_password_input[]){
	attacker_info attacker;
	FILE * attackers_list;
	attackers_list = fopen("attackers_list.dat","rb");
	fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	while(!feof(attackers_list)){
		if((strcmp(attacker.username, username_input) == 0) && (strcmp(attacker.password, salted_hashed_password(hashed_password_input)) == 0))
			return SUCCESS;
		fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	}
	return FAILUR;
	fclose(attackers_list);
}
//* end log in handling  *//

// cross compiler victim payloadcratetor
void crate_and_sand_victim_payload(){
//	char comilation_command[] = "Make";
	system("make -C victim_payload/");
	
}

//* utilities *//
char * salted_hashed_password(char hashed_password_input[]){
	char salted_hashed_password[HASH_LEN + HASH_SALT_LEN];
	strcpy(salted_hashed_password, hashed_password_input);
	strcat(salted_hashed_password, HASH_SALT); // append the salt to the hash
	return md5(salted_hashed_password);
}
void print_all_attackers(){
	attacker_info attacker;
	FILE * attackers_list;
	attackers_list = fopen("attackers_list.dat","rb");
	fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	while(!feof(attackers_list)){
		printf("%s\t%s\t%d\n",attacker.username,attacker.password,attacker.active);
		fread(&attacker, sizeof(attacker_info), 1, attackers_list);
	}
	fclose(attackers_list);
}

void print_all_licenes_key(){
	licenses_key_item temp_licenses_key_item;
	FILE * licenses_key_list;
	licenses_key_list = fopen("licenses_key_list.dat","rb");
	fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenses_key_list);
	while(!feof(licenses_key_list)){
		printf("%s\n", temp_licenses_key_item.licenses_key);
		fflush(stdout);
		fread(&temp_licenses_key_item, sizeof(licenses_key_item), 1, licenses_key_list);
	}
	fclose(licenses_key_list);
}

//* end utilities *//