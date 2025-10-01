#include <stdio.h>
#include "mymathlib.h"

int main(void)
{
	int a = 5;
	int b = 10;
	
	printf("sum(%d,%d) = %d\n", a, b, sum(a,b));
	printf("mult(%d,%d) = %d\n", a, b, mult(a,b));

	return 0;
}
