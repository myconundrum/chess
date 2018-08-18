#include "chess.h"



int main (int argc, char **argv) {

	printf("starting Chess engine...\n");


	DEBUG_OPEN();

	DEBUG_PRINT("argc: %d\n",argc);
	
	eng_init();
	eng_initPosition();
	ui_init();

	while(ui_running()) {
		ui_update();
	}

	DEBUG_CLOSE();
	return 0;
}