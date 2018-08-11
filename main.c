#include "chess.h"



int main (int argc, char **argv) {

	printf("starting Chess engine...\n");
	
	eng_init();
	eng_initPosition();

	ui_init();

	while(ui_running()) {
		ui_update();
	}

	return 0;
}