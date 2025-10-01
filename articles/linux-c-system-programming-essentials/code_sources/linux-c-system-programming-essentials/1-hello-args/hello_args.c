#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("invalid arguments! sample:\nprogram -h intfloatbool\n");
		return 1;
	}

	char* name = argv[2];
	int opt = -1;
	while( ( opt = getopt( argc, argv, "hgm"  ) ) != -1 )
	{
		switch(opt)
		{
			case 'h':
			{
				printf("hello %s !!!\n", name);
				return 0;
			}
			case 'g':
			{
				printf("greetings %s !!!\n", name);
				return 0;
			}
			case 'm':
			{
				printf("mashala %s !!!\n", name);
				return 0;
			}
			default:
			{	
				printf("invalid argument! Consider using: -h, -g, -m.\n");
				return 1;
			}

		}
	}
}
