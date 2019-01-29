#include <stdio.h>
#include <stdbool.h>
#include "start_menu.h"

#define MAX_VICTIMS_PER_ATTACKER 10 // dear hacker, changing this number won't let you get to have more victims, the server decides it
#define MAX_USER_NAME_LEN 32

char selected_victim_name[MAX_USER_NAME_LEN];

void choose_attack_menu ();
void choose_victim ();