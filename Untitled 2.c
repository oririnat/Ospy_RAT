#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main() {
	char cwd[40];
	getcwd(cwd, 40);
	printf("Current working dir: %s\n", cwd);
	
	return 0;
}