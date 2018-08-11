#include "chess.h"





uint64_t g_bitPositions[8][8];
uint64_t g_bitRanks[8];
uint64_t g_bitFiles[8];

const int g_pieceValues[PMAX] = {1,3,3,5,9,999};


POSITION g_position;



uint64_t eng_bitFile(int file) {return g_bitFiles[file];}
uint64_t eng_bitRank(int rank) {return g_bitFiles[rank];}
uint64_t eng_bitFileRank(int file, int rank) {return g_bitPositions[file][rank];}



POSITION * eng_curPosition() {
	return &g_position;
}

void eng_initPosition() {

	memset(&g_position,0,sizeof(g_position));

	g_position.pieces[WHITE][PAWN] |= g_bitRanks[1];
	g_position.pieces[BLACK][PAWN] |= g_bitRanks[6];

	g_position.pieces[WHITE][ROOK] |= g_bitPositions[H][0];
	g_position.pieces[WHITE][ROOK] |= g_bitPositions[A][0];
	g_position.pieces[BLACK][ROOK] |= g_bitPositions[H][7];
	g_position.pieces[BLACK][ROOK] |= g_bitPositions[A][7];

	g_position.pieces[WHITE][KNIGHT] |= g_bitPositions[G][0];
	g_position.pieces[WHITE][KNIGHT] |= g_bitPositions[B][0];
	g_position.pieces[BLACK][KNIGHT] |= g_bitPositions[G][7];
	g_position.pieces[BLACK][KNIGHT] |= g_bitPositions[B][7];


	g_position.pieces[WHITE][BISHOP] |= g_bitPositions[F][0];
	g_position.pieces[WHITE][BISHOP] |= g_bitPositions[C][0];
	g_position.pieces[BLACK][BISHOP] |= g_bitPositions[F][7];
	g_position.pieces[BLACK][BISHOP] |= g_bitPositions[C][7];


	g_position.pieces[WHITE][QUEEN] |= g_bitPositions[D][0];
	g_position.pieces[BLACK][QUEEN] |= g_bitPositions[D][7];

	g_position.pieces[WHITE][KING] |= g_bitPositions[E][0];
	g_position.pieces[BLACK][KING] |= g_bitPositions[E][7];

	for (int i = 0; i < PMAX; i++) {
		g_position.sides[WHITE] |= g_position.pieces[WHITE][i];
		g_position.sides[BLACK] |= g_position.pieces[BLACK][i];
	}

	g_position.all = g_position.sides[WHITE] | g_position.sides[BLACK];
}



POSITION g_position;

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
}