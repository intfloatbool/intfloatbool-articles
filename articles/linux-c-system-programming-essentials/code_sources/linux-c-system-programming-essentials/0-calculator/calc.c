#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	
	if(argc != 4) 
	{
		printf("arguments fail, example:\n$ ./calc.out 10 * 10\n");
		return 1;
	}
	
	int result = -1;

	int left = atoi(argv[1]);
	int right = atoi(argv[3]);
	char* operator = argv[2];
	
	if(strcmp(operator, "x") == 0)
	{
		result = left * right;
	}
	if(strcmp(operator, ":") == 0)
	{
		result = left / right;
	}
	if(strcmp(operator, "+") == 0)
	{
		result = left + right;
	}
	if(strcmp(operator, "-") == 0)
	{
		result = left - right;
	}
	
	if(result == -1)
	{
		printf("arguments fail, invalid operator '%s' , expected: [ x , : , +, - ]\n", operator);
		return 1;
	}

	printf("result: %d %s %d = %d\n", left, operator, right, result);
	
	return 0;	
}
