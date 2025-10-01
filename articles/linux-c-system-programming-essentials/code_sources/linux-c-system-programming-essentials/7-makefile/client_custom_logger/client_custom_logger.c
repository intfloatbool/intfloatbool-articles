#include "include/custom_logger.h"

int main(void)
{
	log_error("MAIN_CTX", "MY ERROR IS HERE.");
	log_message("MAIN_CTX", "MY MESSAGE IS HERE.");
	log_warning("MAIN_CTX", "MY WARNING IS HERE.");

	return 0;
}
