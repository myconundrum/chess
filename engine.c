#include "chess.h"


uint64_t FRMASKS[8][8];

uint64_t RANKMASKS[8] = 
	{RANK1_MASK,RANK2_MASK,RANK3_MASK,RANK4_MASK,RANK5_MASK,RANK6_MASK,RANK7_MASK,RANK8_MASK};
uint64_t FILEMASKS[8] = 
	{AFILE_MASK,BFILE_MASK,CFILE_MASK,DFILE_MASK,EFILE_MASK,FFILE_MASK,GFILE_MASK,HFILE_MASK};
uint64_t SQUAREMASKS[64];

uint64_t DIAGONALMASKS[64];
uint64_t ANTIDIAGONALMASKS[64];

const int g_pieceValues[PMAX] = {1,3,3,5,9,999};

POSITION g_position;
POSITION * eng_curPosition() {return &g_position;}

void eng_resetPosition() {
	memset(&g_position,0,sizeof(g_position));
}




void eng_loadFEN(char *fen) {

	char buf[256];
	char * p;
	char * end;
	char * ranks[8];
	int i = 7;
	char toMove[5];
	char castle[20];
	char ep[20];
	int  halfMoves;
	int  fullMoves;
	int  epFile;
	int  epRank;

	eng_resetPosition();

	strcpy(buf,fen);
	
	if (!fen) {return;} 


	p = buf;
	do {
		end = strchr(p,'/');
		if (end) {
			*end = 0;
			ranks[i--] = p;
			p = ++end;
		} else {
			end = strchr(p,' ');
			if (end) {
				*end = 0;
				ranks[i--] = p;
				p = ++end;
				end = 0;
			}
		}
	} while (end != 0);

	sscanf(p,"%s %s %s %d %d",toMove,castle,ep,&halfMoves,&fullMoves);

	for (int r = 0; r < 8; r++) {
		p = ranks[r];
		int f = 0;

		while (p && *p) {
			
			if (isdigit(*p)) {
				f += (*p - '0');
			} else if (isalpha(*p)) {
				switch(*p) {
					printf("%c at %d %d\n",*p,f,r);
					case 'p': g_position.pieces[BLACK][PAWN] |= FRMASKS[f++][r]; break;
					case 'P': g_position.pieces[WHITE][PAWN] |= FRMASKS[f++][r]; break;
					case 'n': g_position.pieces[BLACK][KNIGHT] |= FRMASKS[f++][r]; break;
					case 'N': g_position.pieces[WHITE][KNIGHT] |= FRMASKS[f++][r]; break;
					case 'r': g_position.pieces[BLACK][ROOK] |= FRMASKS[f++][r]; break;
					case 'R': g_position.pieces[WHITE][ROOK] |= FRMASKS[f++][r]; break;
					case 'b': g_position.pieces[BLACK][BISHOP] |= FRMASKS[f++][r]; break;
					case 'B': g_position.pieces[WHITE][BISHOP] |= FRMASKS[f++][r]; break;
					case 'q': g_position.pieces[BLACK][QUEEN] |= FRMASKS[f++][r]; break;
					case 'Q': g_position.pieces[WHITE][QUEEN] |= FRMASKS[f++][r]; break;
					case 'k': g_position.pieces[BLACK][KING] |= FRMASKS[f++][r]; break;
					case 'K': g_position.pieces[WHITE][KING] |= FRMASKS[f++][r]; break;
					default: printf("invalid fen char %c.\n",*p);
					break;
				}
			}
			p++;
		}
	}

	for (PIECES piece = PAWN; piece < PMAX; piece++) {
		g_position.sides[BLACK] |= g_position.pieces[BLACK][piece];
		g_position.sides[WHITE] |= g_position.pieces[WHITE][piece];
	}
	g_position.all = g_position.sides[BLACK] | g_position.sides[WHITE];

	/**Manage To Move **/
	g_position.toMove = (*toMove == 'b') ? BLACK : WHITE;

	/**Manage castling **/
	p = castle;
	while (*p) {
		switch(*p) {
			case 'Q': g_position.qCastle[WHITE] = true;break;
			case 'q': g_position.qCastle[BLACK] = true;break;
			case 'K': g_position.kCastle[WHITE] = true;break;
			case 'k': g_position.kCastle[BLACK] = true;break;
		}
		p++;
	}

	/**Manage ep squares**/
	if (*ep && *ep != '-') {
		epFile = ep[0] - 'a';
		epRank = ep[1] - '0';
		epRank--;
		g_position.ep = FRMASKS[epFile][epRank];
	}

	g_position.halfMoves = halfMoves;
	g_position.fullMoves = fullMoves;
}


void eng_initPosition() {

	eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");
	//eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 0");
	
	//eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/1p1p1p1p/PPPPPPPP/RNBQKBNR b KQkq - 0 0");
	//eng_loadFEN("K1K4N/2N5/3K4/3n2R1/8/3pKp2/4pP2/8 w - - 0 23 ");
	//eng_loadFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
	//eng_loadFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	//eng_loadFEN("2r3k1/1q1nbppp/r3p3/3pP3/pPpP4/P1Q2N2/2RN1PPP/2R4K b - b3 0 23");
	//eng_loadFEN("8/8/8/2Pp4/8/8/8/8 2 - d6 0 23");
	//eng_loadFEN("4qk2/1PPP3P/8/8/8/8/8/8 w - - 0 23");
	//eng_loadFEN("8/8/3r4/3R2rr/8/8/8/3R4 w - - 0 23");
		
}



MOVE * eng_bestMove() {
	

	POSITION * pos = eng_curPosition();
	movegen_generate(pos);
	
	DEBUG_PRINT("after getmoves\n");


	DEBUG_PRINT("%d moves\n",pos->moveCount);


	return &pos->moves[0];

}

/*
	used for validation tests during development.
*/
void eng_test() {

}


void eng_init() {
	
	memset(DIAGONALMASKS,0,sizeof(DIAGONALMASKS));
	memset(SQUAREMASKS,0,sizeof(SQUAREMASKS));
	memset(FRMASKS,0,sizeof(FRMASKS));

	for (FILES f = A; f < FMAX; f++) {
		for (int r = 0; r < 8; r++) {
			FRMASKS[f][r] = BFR(f,r);
		}
	}

	for (int i = 0; i < 64; i++) {
		int j;
		SQUAREMASKS[i] = BFR(FILEFROMSQUARE(i),RANKFROMSQUARE(i));
		j = i;
		do {
			DIAGONALMASKS[i] |= BPOS(j);
			j += NORTHEAST;
		} while (j < 64 && DISTANCE(j,j-NORTHEAST) == 1);
		j = i;
		do {
			ANTIDIAGONALMASKS[i] |= BPOS(j);
			j += NORTHWEST;
		} while (j < 64 && DISTANCE(j,j-NORTHWEST) == 1);
		j = i;
		do {
			DIAGONALMASKS[i] |= BPOS(j);
			j += SOUTHWEST;
		} while (j >= 0 && DISTANCE(j,j-SOUTHWEST) == 1);
		j = i;
		do {
			ANTIDIAGONALMASKS[i] |= BPOS(j);
			j += SOUTHEAST;
		} while (j >= 0 && DISTANCE(j,j-SOUTHEAST) == 1);

	}


	movegen_init();
	eng_test();
}