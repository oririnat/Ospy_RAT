#include <stdio.h>
#include <string.h>
#include "utilities.h"

#define ENTER 13
#define TAB 9
#define BKSP 8

void flush_stdin(){
	fseek(stdin, 0, SEEK_END);
	unsigned int file_length = ftell(stdin);
	rewind(stdin);
	char curr_char;
	if (file_length > 0)
		while(((curr_char = getchar()) != '\n') || curr_char != EOF); // clean buffer
}

void safe_scan(char (* input)[], int max_input_len){
	flush_stdin();
	fgets (*input, max_input_len, stdin);
	strtok(*input, "\n");
}

void press_enter_to_continue(){
	printf("\npress ENTER to continue");
	fflush(stdout);
	flush_stdin();
	getchar();
}

void copytoclipboard(const char * str){
	const char proto_cmd[] = "echo '%s' | pbcopy";
	char cmd[strlen(str) + strlen(proto_cmd) - 1]; // -2 to remove the length of %s in proto cmd and + 1 for null terminator = -1
	sprintf(cmd ,proto_cmd, str);
	system(cmd);
}