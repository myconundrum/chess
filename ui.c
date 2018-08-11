#include "chess.h"

#define MAX_BUFFER 256
char g_buffer[MAX_BUFFER];
bool g_running = true;
int g_maxCommands;

typedef void (*cmd_callback)(char *);

typedef struct {

	const char * match;
	const char * desc;
	cmd_callback fn;

} CMD;



void cmd_quit(char * str) {g_running = false;}
void cmd_dumpPosition(char * str) {printPosition(eng_curPosition());}
void cmd_dumpFEN(char *str) {positionToFEN(eng_curPosition(),false);}
void cmd_dumpFENURL(char * str) {positionToFEN(eng_curPosition(),true);}

void cmd_help(char * str);

CMD g_commands[] = {
	{"quit","Quit the chess engine.",cmd_quit},
	{"dpos","Print the current position.",cmd_dumpPosition},
	{"dfen","Print the current position as a FEN string.",cmd_dumpFEN},
	{"durl","Print the current position as a FEN URL.",cmd_dumpFENURL},
	{"help","Show list of commands.",cmd_help}
};

void cmd_help(char * str) {

	for (int i = 0; i < g_maxCommands; i++) {
		printf("%s : %s\n",g_commands[i].match,g_commands[i].desc);
	}
}



bool ui_running() {
	return g_running;
}

void ui_init() {
	g_running = true;

	g_maxCommands = sizeof(g_commands) / sizeof(g_commands[0]);
}

void ui_update() {

	char *p;

	printf("> ");
	fgets(g_buffer,MAX_BUFFER,stdin);
	g_buffer[strlen(g_buffer)-1] = 0;


  	p= strchr (g_buffer,' ');
  	if (p) {
  		*p++ = 0;
  	}

  	for (int i = 0; i < g_maxCommands; i++) {
  		if (strcmp(g_commands[i].match,g_buffer) == 0) {
  			g_commands[i].fn(p);
  		}
  	}

  	printf("\n");
}





