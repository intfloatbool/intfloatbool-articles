#include <stdio.h>

void log_error(char* context, char* message)
{
	printf("ERROR -> context: %s > > > message: %s\n", context, message);
}
void log_message(char* context, char* message)
{
	printf("MESSAGE -> context: %s > > > message: %s\n", context, message);

}
void log_warning(char* context, char* message)
{
	printf("WARNING -> context: %s > > > message: %s\n", context, message);

}

