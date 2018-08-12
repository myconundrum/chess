#include "chess.h"


uint64_t FRMASKS[8][8];

uint64_t RANKMASKS[8] = 
	{RANK1_MASK,RANK2_MASK,RANK3_MASK,RANK4_MASK,RANK5_MASK,RANK6_MASK,RANK7_MASK,RANK8_MASK};
uint64_t FILEMASKS[8] = 
	{HFILE_MASK,GFILE_MASK,FFILE_MASK,EFILE_MASK,DFILE_MASK,CFILE_MASK,BFILE_MASK,AFILE_MASK};
uint64_t SQUAREMASKS[64];

const int g_pieceValues[PMAX] = {1,3,3,5,9,999};

POSITION g_position;
POSITION * eng_curPosition() {return &g_position;}

void eng_loadFEN(char *fen) {

	char pieces[100];
	char toMove[5];
	char castle[20];
	char ep[20];
	int  halfMoves;
	int  fullMoves;
	char *p;
	int sq = 63;
	int epFile;
	int epRank;

	if(!fen) {return;}

	sscanf(fen,"%s %s %s %s %d %d",pieces,toMove,castle,ep,&halfMoves,&fullMoves);

	/** Manage Pieces **/
	p = pieces;
	while (sq > -1 && p && *p) {
		if (isdigit(*p)) {
			sq -= *p - '0';
		} else if (isalpha(*p)) {
			switch (*p) {
				case 'p': g_position.pieces[BLACK][PAWN] |= SQUAREMASKS[sq--]; break;
				case 'P': g_position.pieces[WHITE][PAWN] |= SQUAREMASKS[sq--]; break;
				case 'n': g_position.pieces[BLACK][KNIGHT] |= SQUAREMASKS[sq--]; break;
				case 'N': g_position.pieces[WHITE][KNIGHT] |= SQUAREMASKS[sq--]; break;
				case 'r': g_position.pieces[BLACK][ROOK] |= SQUAREMASKS[sq--]; break;
				case 'R': g_position.pieces[WHITE][ROOK] |= SQUAREMASKS[sq--]; break;
				case 'b': g_position.pieces[BLACK][BISHOP] |= SQUAREMASKS[sq--]; break;
				case 'B': g_position.pieces[WHITE][BISHOP] |= SQUAREMASKS[sq--]; break;
				case 'q': g_position.pieces[BLACK][QUEEN] |= SQUAREMASKS[sq--]; break;
				case 'Q': g_position.pieces[WHITE][QUEEN] |= SQUAREMASKS[sq--]; break;
				case 'k': g_position.pieces[BLACK][KING] |= SQUAREMASKS[sq--]; break;
				case 'K': g_position.pieces[WHITE][KING] |= SQUAREMASKS[sq--]; break;
				break; 
				default: printf("invalid fen char %c.\n",*p);
				break;
			}
		}
		p++;
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
		epFile = 'h' - ep[0];
		epRank = ep[1] - '0';
		epRank--;
		g_position.ep = FRMASKS[epFile][epRank];
	}

	g_position.halfMoves = halfMoves;
	g_position.fullMoves = fullMoves;

}


void eng_initPosition() {

	//eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	//eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/1p1p1p1p/PPPPPPPP/RNBQKBNR");
	//eng_loadFEN("8/8/8/8/8/5p2/4P3/8");
	//eng_loadFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
	//eng_loadFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	//eng_loadFEN("2r3k1/1q1nbppp/r3p3/3pP3/pPpP4/P1Q2N2/2RN1PPP/2R4K b - b3 0 23");
	eng_loadFEN("4qk2/1PPP3P/8/8/8/8/8/8 w - - 0 23");
		

	
}

void eng_init() {
	
	for (FILES f = H; f < FMAX; f++) {
		for (int r = 0; r < 8; r++) {
			FRMASKS[f][r] = BFR(f,r);
		}
	}

	for (int i = 0; i < 64; i++) {
		SQUAREMASKS[i] = BPOS(i);
	}
}