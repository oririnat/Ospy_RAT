#include <stdio.h>
#include "start_menu.h"

#define TRUE 1
#define FALSE 0
#define MAX_VICTIMS_PER_ATTACKER 10 // dear hacker, changing this number wont let you to get to have more victims, the server decide it
#define MAX_INPUT_LEN 40

char selected_victim_name[MAX_INPUT_LEN];

void choose_attack_menu ();
void choose_victim ();