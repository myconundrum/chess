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


void eng_loadFEN(char * fen) {

	memset(&g_position,0,sizeof(g_position));
	int sq = 63;
	char *p = fen;

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
}

void eng_initPosition() {

	//eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	eng_loadFEN("rnbqkbnr/pppppppp/8/8/8/1p1p1p1p/PPPPPPPP/RNBQKBNR");
	//eng_loadFEN("8/8/8/8/8/5p2/4P3/8");
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