#include <stdio.h>
#include "logging/logging.h"

int main(void) {
	log_init("redmagicfancontrold.log");
	log_write(LOG_INFO, "RedMagic Fan Control Daemon started");

	//start lifecycle of the daemon here

	// exit gracefully
	return 0;
}
