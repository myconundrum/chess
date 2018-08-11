#include "chess.h"


MOVELIST g_moveList;

void movegen_clearMoves() {

	memset(&g_moveList,0,sizeof(g_moveList));
	g_moveList.count = 0;
}

void movegen_addMove(int color, int piece, int from, int to, int type) {

	g_moveList.moves[g_moveList.count].color = color;
	g_moveList.moves[g_moveList.count].piece = piece;
	g_moveList.moves[g_moveList.count].from = from;
	g_moveList.moves[g_moveList.count].to = to;
	g_moveList.moves[g_moveList.count].type = type;
	g_moveList.count++;
}

void movegen_pawnpush(POSITION *pos,int color) {

	// m1 -- candidate 1 move pawn pushes
	// m2 -- candidate 2 move pawn pushes
	// mal -- candidate left attacks
	// mar -- candidate right attacks

	// remove pawns that are on the last rank, so that we don't overflow.
	uint64_t m1 = pos->pieces[color][PAWN] & ~RANKMASKS[color == WHITE ? 7 : 0];
	uint64_t m2,mal,mar;
	int sq;

	// 1 move forward pawn pushes.
	m1 = color == WHITE ? NORTH(m1) : SOUTH(m1);
	mal = m1 & ~FILEMASKS[color==WHITE ? A : H];
	mar = m1 & ~FILEMASKS[color==WHITE ? H : A];
	
	// only if the new square is unoccupied
	m1 &= ~pos->all;
	
	// check for candidate pawns that could do a starting move double push. These are pawns
	// that could move forward one (m1) and are now on the 3rd rank.
	m2 = m1 & RANKMASKS[color==WHITE ? 2 : 5];
	m2 = color == WHITE ? NORTH(m2) : SOUTH(m2);
	m2 &= ~pos->all;
	
	// check for left attacks (clear out the leftmost file)
	mal = WEST(mal);
	mal &= pos->sides[color ? 0 : 1];
	
	// check for right attacks (clear out the rightmost file)
	mar = EAST(mar);
	mar &= pos->sides[color ? 0 : 1];
	
	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= SQUAREMASKS[sq];
		movegen_addMove(color,PAWN,color == WHITE ? sq - 8 : sq + 8, sq, MT_NORMAL);
	}

	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addMove(color,PAWN,color == WHITE ? sq - 16: sq + 16, sq, MT_ENPASSANT);
	}	
	
	while(mal) {
		sq = bitScanForward(mal);
		mal ^= SQUAREMASKS[sq];
		movegen_addMove(color,PAWN,color == WHITE ? sq - 9: sq + 9, sq, MT_CAPTURE);
	}	

	while(mar) {
		sq = bitScanForward(mar);
		mar ^= SQUAREMASKS[sq];
		movegen_addMove(color,PAWN,color == WHITE ? sq - 7: sq + 7, sq, MT_CAPTURE);
	}	

	// 
	// BUGBUG: does not generate en passant captures yet.
	//
}


MOVELIST * movegen_getMoves() {
	return &g_moveList;
}

void movegen_generate() {
	movegen_clearMoves();
	movegen_pawnpush(eng_curPosition(),WHITE);
	printMoves(eng_curPosition(),&g_moveList);
}


