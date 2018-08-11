#include "chess.h"


uint64_t g_bitPositions[8][8];
uint64_t g_bitRanks[8];
uint64_t g_bitFiles[8];
uint64_t g_bitMasks[64];

const int g_pieceValues[PMAX] = {1,3,3,5,9,999};

POSITION g_position;

uint64_t eng_bitFile(int file) 					{return g_bitFiles[file];}
uint64_t eng_bitRank(int rank) 					{return g_bitRanks[rank];}
uint64_t eng_bitFileRank(int file, int rank) 	{return g_bitPositions[file][rank];}
uint64_t eng_bitPos(int pos) 					{return g_bitMasks[pos];}


POSITION * eng_curPosition() 					{return &g_position;}


void eng_loadFEN(char * fen) {

	memset(&g_position,0,sizeof(g_position));
	int sq = 63;
	char *p = fen;

	while (sq > -1 && p && *p) {
		if (isdigit(*p)) {
			sq -= *p - '0';
		} else if (isalpha(*p)) {
			switch (*p) {
				case 'p': g_position.pieces[BLACK][PAWN] |= g_bitMasks[sq--]; break;
				case 'P': g_position.pieces[WHITE][PAWN] |= g_bitMasks[sq--]; break;
				case 'n': g_position.pieces[BLACK][KNIGHT] |= g_bitMasks[sq--]; break;
				case 'N': g_position.pieces[WHITE][KNIGHT] |= g_bitMasks[sq--]; break;
				case 'r': g_position.pieces[BLACK][ROOK] |= g_bitMasks[sq--]; break;
				case 'R': g_position.pieces[WHITE][ROOK] |= g_bitMasks[sq--]; break;
				case 'b': g_position.pieces[BLACK][BISHOP] |= g_bitMasks[sq--]; break;
				case 'B': g_position.pieces[WHITE][BISHOP] |= g_bitMasks[sq--]; break;
				case 'q': g_position.pieces[BLACK][QUEEN] |= g_bitMasks[sq--]; break;
				case 'Q': g_position.pieces[WHITE][QUEEN] |= g_bitMasks[sq--]; break;
				case 'k': g_position.pieces[BLACK][KING] |= g_bitMasks[sq--]; break;
				case 'K': g_position.pieces[WHITE][KING] |= g_bitMasks[sq--]; break;
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
	
}

void eng_init() {
	
	uint64_t mask = 0xFFull;

	for (FILES f = H; f < FMAX; f++) {
		for (int r = 0; r < 8; r++) {
			g_bitPositions[f][r] = BFR(f,r);
		}
	}

	for (int i = 0; i < 8; i++) {
		g_bitRanks[i] = mask;
		mask <<= 8; 
	}

	mask = 0x0101010101010101ull;

	for (int i = 7; i >= 0; i--) {
		g_bitFiles[i] = mask;
		mask <<= 1; 
	}

	for (int i = 0; i < 64; i++) {
		g_bitMasks[i] = BPOS(i);
	}
}