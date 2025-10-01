#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	double x;
	double y;

} math_vector2;


math_vector2 str_to_vec2(char* str);
double calculate_distance(math_vector2 a, math_vector2 b);

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		printf("invalid arguments count! Expected 2. in format 5.33:5.14 6.23:6.88\n");
		exit(1);
	}	

	char* vec_str_a = argv[1];
	char* vec_str_b = argv[2];
	
	math_vector2 vec_a = str_to_vec2(vec_str_a);
	math_vector2 vec_b = str_to_vec2(vec_str_b);
	
	printf("Input a: (%f, %f), b: (%f, %f)\n\n", vec_a.x, vec_a.y, vec_b.x, vec_b.y );
	
	double distance = calculate_distance(vec_a, vec_b);

	printf("Distance between them: %d\n", distance);

	exit(0);
}

math_vector2 str_to_vec2(char* str)
{
	char* endptr;
	double x = strtod( str , &endptr );
	
	if(*endptr != ':')
	{
		fprintf(stderr, "Format error: expected ':'\n");
		exit(1);
	}	

	double y = strtod( endptr + 1, NULL);

	math_vector2 output;
	output.x = x;
	output.y = y;
	
	return output;	
}

double calculate_distance(math_vector2 a, math_vector2 b)
{
	double diff_x = b.x - a.x;
	double diff_y = b.y - a.y;

	double squared_diff_x = pow(diff_x, 2);
	double squared_diff_y = pow(diff_y, 2);

	double distance = sqrt(squared_diff_x + squared_diff_y);

	return distance;
}
