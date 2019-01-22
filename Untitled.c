#include <stdio.h>


int vuln() {
	char buf[32];

	printf("Enter your favorite color: ");
	gets(buf);

	int good = 0;
	
	for (int i = 0; buf[i]; i++) {
		good &= buf[i] ^ buf[i];
		printf("i = %d : %d,    good = %d\n", i, buf[i], good);
	}

	return good;
}

int main(int argc, char *argv[]) {
	while (1)
		printf("%d\n", vuln());
}
