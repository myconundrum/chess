#include "chess.h"


#define MAX_MOVES 1024
MOVE g_moveList[MAX_MOVES];
int g_moveCount = 0;

void movegen_clearMoves() {

	memset(g_moveList,0,sizeof(g_moveList));
	g_moveCount = 0;
}

void movegen_addMove(int color, int piece, int from, int to, int type) {

	g_moveList[g_moveCount].color = color;
	g_moveList[g_moveCount].piece = piece;
	g_moveList[g_moveCount].from = from;
	g_moveList[g_moveCount].to = to;
	g_moveList[g_moveCount].type = type;
	g_moveCount++;
}

void movegen_pawnpush(POSITION *pos,int color) {

	// m1 -- candidate 1 move pawn pushes
	// m2 -- candidate 2 move pawn pushes
	// mal -- candidate left attacks
	// mar -- candidate right attacks

	// remove pawns that are on the last rank, so that we don't overflow.
	uint64_t m1 = pos->pieces[color][PAWN] & ~eng_bitRank(color == WHITE ? 7 : 0);
	uint64_t m2,mal,mar;
	int sq;

	// 1 move forward pawn pushes.
	m1 = color == WHITE ? m1 << 8 : m1 >> 8;
	
	// only if the new square is unoccupied
	m1 &= ~pos->all;
	
	// check for candidate pawns that could do a starting move double push. These are pawns
	// that could move forward one (m1) and are now on the 3rd rank.
	m2 = m1 & eng_bitRank(color==WHITE ? 2 : 5);
	m2 = color == WHITE ? m2 << 8 : m2 >> 8;
	m2 &= ~pos->all;
	
	// check for left attacks (clear out the leftmost file)
	mal = m1 & ~eng_bitFile(color==WHITE ? A : H);
	mal &= pos->sides[color ? 0 : 1];

	// check for right attacks (clear out the rightmost file)
	mar = m1 & ~eng_bitFile(color==WHITE ? H : A);
	mal &= pos->sides[color ? 0 : 1];
	
	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= eng_bitPos(sq);
		movegen_addMove(color,PAWN,color == WHITE ? sq - 8 : sq + 8, sq, MT_NORMAL);
	}

	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= eng_bitPos(sq);
		movegen_addMove(color,PAWN,color == WHITE ? sq - 16: sq + 16, sq, MT_ENPASSANT);
	}	
	
	while(mal) {
		sq = bitScanForward(mal);
		mal ^= eng_bitPos(sq);
		movegen_addMove(color,PAWN,color == WHITE ? sq - 9: sq + 9, sq, MT_CAPTURE);
	}	
	while(mar) {
		sq = bitScanForward(mar);
		mar ^= eng_bitPos(sq);
		movegen_addMove(color,PAWN,color == WHITE ? sq - 7: sq + 7, sq, MT_CAPTURE);
	}	

	// 
	// BUGBUG: does not generate en passant captures yet.
	//
}



void movegen_generate() {
	movegen_clearMoves();
	movegen_pawnpush(eng_curPosition(),WHITE);

	printf("moves: \n");
	for (int i = 0; i < g_moveCount; i++) {
		printf("from: %d to: %d color: %s piece: %d\n",
			g_moveList[i].from,
			g_moveList[i].to,
			g_moveList[i].color == WHITE ? "white" : "black",
			g_moveList[i].piece );
	}
}


