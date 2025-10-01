#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	char* custom_env = getenv("MY_ENV");
	char* home_env = getenv("HOME");
	char* user_env = getenv("USER");

	printf("'MY_ENV' value: %s\n", custom_env);
	printf("'HOME' value: %s\n", home_env);
	printf("'USER' value: %s\n", user_env);

	exit(1);
}
