#include <stdio.h>

int main(int argc, char *argv[]) {
	int first = 75;
	int * ptr = &first; //0x7ffee7dff7fc
	* ptr = 80;
	printf("%d", first);

// & תביא לי כתובת של

//	char sec = 'K';
////	string ori = "ori";
//	char thr[10];
//	char foo[4] = "noa\0";
//	
//	printf("noa %s", foo);
}

//0x7ffee7dff7fc

//bases :

//0x hex
//0123456789abcdef

// d
//0123456789

// b
//01