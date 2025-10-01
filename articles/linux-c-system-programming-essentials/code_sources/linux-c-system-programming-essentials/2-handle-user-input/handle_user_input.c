#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 256

int main(void)
{
	char secret_word[] = "intfloatbool";

	char user_input[BUFF_SIZE];
        
	while( fgets( user_input, sizeof(user_input), stdin ) != NULL )
	{
		user_input[ strcspn( user_input , "\n") ] = '\0';

		if( strcmp(user_input, secret_word) == 0 )
		{
			printf("Wow! You know the secret word! Congrats! \n");
			exit(0);
		}

		printf("your message: %s\n", user_input);		
		printf("number representation: ");
		
		for(int i = 0; i < sizeof(user_input); i++)
		{
			char c = user_input[i];
			if(c == '\0') 
			{
				break;
			}
			printf("%d ", user_input[i]);
		}

		printf("\n");
	}	
	
	exit(0);
}
