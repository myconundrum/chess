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


char g_outBuffer[1024];

void ui_out(const char * format, ...) {

	va_list args;
	va_start(args,format);
	vsprintf(g_outBuffer,format,args);
	va_end(args);
	puts(g_outBuffer);
	#ifdef DEBUG
	fprintf(g_log,"OUT: %s",g_outBuffer);

	#endif
	
}

void cmd_quit(char * str) {g_running = false;}
void cmd_dumpPosition(char * str) {printPosition(eng_curPosition());}
void cmd_dumpFEN(char *str) {positionToFEN(eng_curPosition(),false);}
void cmd_dumpFENURL(char * str) {positionToFEN(eng_curPosition(),true);}

void cmd_testMove(char *str) {movegen_generate(eng_curPosition());}

void cmd_help(char * str);

void cmd_uci_uci(char * str) {
	/*
	"uci"
	tell engine to use the uci (universal chess interface),
	this will be send once as a first command after program boot
	to tell the engine to switch to uci mode.
	After receiving the uci command the engine must identify itself with the "id" command
	and sent the "option" commands to tell the GUI which engine settings the engine supports if any.
	After that the engine should sent "uciok" to acknowledge the uci mode.
	If no uciok is sent within a certain time period, the engine task will be killed by the GUI.
	*/

	ui_out("id name Chess Engine 0.1\n");
	ui_out("id author Marc Whitten\n");

	// Send engine options here...

	ui_out("uciok\n");	
}

void cmd_uci_stop(char *str) {
	/*
	"stop"
	stop calculating as soon as possible,
	don't forget the "bestmove" and possibly the "ponder" token when finishing the search
	*/

	// should send bestmove and ponder here.

}

void cmd_uci_position(char *str) {
	/*
	"position [fen  | startpos ]  moves  .... "
	set up the position described in fenstring on the internal board and
	play the moves on the internal chess board.
	if the game was played  from the start position the string "startpos" will be sent
	Note: no "new" command is needed. However, if this position is from a different game than
	the last position sent to the engine, the GUI should have sent a "ucinewgame" inbetween.
	*/
	char *p;
	p= strchr (str,' ');
  	if (p) {
  		*p++ = 0;
  	}
  	if (!strcmp(p,"startpos")) {
  		p = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
  	}
  	eng_loadFEN(p);
  	//debug_printPosition(eng_curPosition());

}

void cmd_uci_go(char *str) {
/*
	* go
	start calculating on the current position set up with the "position" command.
	There are a number of commands that can follow this command, all will be sent in the same string.
	If one command is not send its value should be interpreted as it would not influence the search.
	* searchmoves  .... 
		restrict search to this moves only
		Example: After "position startpos" and "go infinite searchmoves e2e4 d2d4"
		the engine should only search the two moves e2e4 and d2d4 in the initial position.
	* ponder
		start searching in pondering mode.
		Do not exit the search in ponder mode, even if it's mate!
		This means that the last move sent in in the position string is the ponder move.
		The engine can do what it wants to do, but after a "ponderhit" command
		it should execute the suggested move to ponder on. This means that the ponder move sent by
		the GUI can be interpreted as a recommendation about which move to ponder. However, if the
		engine decides to ponder on a different move, it should not display any mainlines as they are
		likely to be misinterpreted by the GUI because the GUI expects the engine to ponder
	   on the suggested move.
	* wtime 
		white has x msec left on the clock
	* btime 
		black has x msec left on the clock
	* winc 
		white increment per move in mseconds if x > 0
	* binc 
		black increment per move in mseconds if x > 0
	* movestogo 
      there are x moves to the next time control,
		this will only be sent if x > 0,
		if you don't get this and get the wtime and btime it's sudden death
	* depth 
		search x plies only.
	* nodes 
	   search x nodes only,
	* mate 
		search for a mate in x moves
	* movetime 
		search exactly x mseconds
	* infinite
		search until the "stop" command. Do not exit the search without being told so in this mode!
*/
	// need to evaluate all of these commands. Not implemented yet.
	
	MOVE * m = eng_bestMove();

	ui_out("info score cp 13  depth 1 nodes 13 time 15 pv %s%s\n",g_squareNames[m->from],g_squareNames[m->to]);
	ui_out("bestmove %s%s\n",g_squareNames[m->from],g_squareNames[m->to]);

}




void cmd_uci_ucinewgame(char *str) {
	/*

   "ucinewgame"
   this is sent to the engine when the next search (started with "position" and "go") will be from
   a different game. This can be a new game the engine should play or a new game it should analyse but
   also the next position from a testsuite with positions only.
   If the GUI hasn't sent a "ucinewgame" before the first "position" command, the engine shouldn't
   expect any further ucinewgame commands as the GUI is probably not supporting the ucinewgame command.
   So the engine should not rely on this command even though all new GUIs should support it.
   As the engine's reaction to "ucinewgame" can take some time the GUI should always send "isready"
   after "ucinewgame" to wait for the engine to finish its operation.
   */

	// reset engine parameters here if necessary.

}

void cmd_uci_isready(char *str) {
	/*
	"isready"
	this is used to synchronize the engine with the GUI. When the GUI has sent a command or
	multiple commands that can take some time to complete,
	this command can be used to wait for the engine to be ready again or
	to ping the engine to find out if it is still alive.
	E.g. this should be sent after setting the path to the tablebases as this can take some time.
	This command is also required once before the engine is asked to do any search
	to wait for the engine to finish initializing.
	This command must always be answered with "readyok" and can be sent also when the engine is calculating
	in which case the engine should also immediately answer with "readyok" without stopping the search.
	*/
	ui_out("readyok\n");
}

CMD g_commands[] = {
	{"go","UCI go command",cmd_uci_go},
	{"position","UCI position command",cmd_uci_position},
	{"isready","UCI isready command",cmd_uci_isready},
	{"ucinewgame","UCI ucinewgame command",cmd_uci_ucinewgame},
	{"stop","UCI stop command",cmd_uci_stop},
	{"uci","UCI command",cmd_uci_uci},
	{"quit","Quit the chess engine.",cmd_quit},
	{"dpos","Print the current position.",cmd_dumpPosition},
	{"dfen","Print the current position as a FEN string.",cmd_dumpFEN},
	{"durl","Print the current position as a FEN URL.",cmd_dumpFENURL},
	{"tm","Test movegen.",cmd_testMove},
	{"help","Show list of commands.",cmd_help}
};

void cmd_help(char * str) {

	for (int i = 0; i < g_maxCommands; i++) {
		ui_out("%s : %s\n",g_commands[i].match,g_commands[i].desc);
	}
}




bool ui_running() {
	return g_running;
}

void ui_init() {

	// unbuffer stdout for engine communication with GUI engines.
	setbuf(stdout, NULL);

	g_running = true;
	g_maxCommands = sizeof(g_commands) / sizeof(g_commands[0]);

}

char * ui_in() {
	fgets(g_buffer,MAX_BUFFER,stdin);
	g_buffer[strlen(g_buffer)-1] = 0;



	// Ugly use of global variable and #defines here. Fix.
	#ifdef DEBUG
	fprintf(g_log,"IN: %s\n",g_buffer);
	#endif

	return g_buffer;

}

void ui_evalCommand(char * cmd,char *args) {

	  for (int i = 0; i < g_maxCommands; i++) {
  		if (strcmp(g_commands[i].match,cmd) == 0) {
  			g_commands[i].fn(args);
  		}
  	}
}

void ui_update() {

	char *p;
	char *buf;
	
	buf = ui_in();
  	p= strchr (buf,' ');
  	if (p) {
  		*p++ = 0;
  	}

  	ui_evalCommand(buf,p);
}





