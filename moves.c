#include "chess.h"


uint64_t g_kingMoves[64];
uint64_t g_knightMoves[64];

MOVELIST g_moveList;

void movegen_clearMoves() {

	memset(&g_moveList,0,sizeof(g_moveList));
	g_moveList.count = 0;
}

void movegen_addMove(int color, int piece, int from, int to, bool capture) {
	
	POSITION * pos;

	g_moveList.moves[g_moveList.count].color = color;
	g_moveList.moves[g_moveList.count].piece = piece;
	g_moveList.moves[g_moveList.count].from = from;
	g_moveList.moves[g_moveList.count].to = to;
	
	//
	// if this is a capture, find the captured piece.
	// Note: EP captures show up as a bool flag (epcapture) vs a capture in the move list.
	//
	if (capture) {
		pos = eng_curPosition();		
		for (PIECES p = PAWN ; p < PMAX; p++) {
			if (SQUAREMASKS[to] & pos->pieces[OPPONENT(color)][p]) {
				g_moveList.moves[g_moveList.count].capture = p;
				break;
			}
		}
	}
	
	g_moveList.count++;
}

void movegen_addPawnMove(int color, int piece, int from, int to, bool capture, bool epCapture, bool epMove) {
	//
	// Check to see if this is a pawn promotion, and generate all promotion possibilities if so.
	//
	if (RANKMASKS[color == WHITE ? 7 : 0] & SQUAREMASKS[to]) {
		for (PIECES p = KNIGHT; p < KING; p++) {
			g_moveList.moves[g_moveList.count].promotion = p;
			movegen_addMove(color,piece,from,to,capture);
		}
	} else {
		g_moveList.moves[g_moveList.count].epCapture = epCapture;
		g_moveList.moves[g_moveList.count].epMove = epMove;
		movegen_addMove(color,piece,from,to,capture);
	}
}

void movegen_king(POSITION *pos) {

	int from; 
	int to;
	uint64_t m;
	uint64_t kings = pos->pieces[pos->toMove][KING];
	uint64_t opp = pos->sides[OPPONENT(pos->toMove)];

	while (kings) {
		//
		// normally, there would only be one king, however some puzzles and tests
		// will have more than one king on the board.
		//
		from = bitScanForward(kings);
		kings ^= SQUAREMASKS[from];

		m = g_kingMoves[from] & ~pos->sides[pos->toMove]; 	
		
		// add candidate moves to movelist.
		while(m) {
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];
			movegen_addMove(pos->toMove,KING,from,to,SQUAREMASKS[to] & opp);
		}
	}
}


void movegen_knight(POSITION *pos) {

	int from; 
	int to;
	uint64_t m;
	uint64_t knights = pos->pieces[pos->toMove][KNIGHT];
	uint64_t opp = pos->sides[OPPONENT(pos->toMove)];

	while (knights) {
		from = bitScanForward(knights);
		knights ^= SQUAREMASKS[from];

		m = g_knightMoves[from] & ~pos->sides[pos->toMove]; 	
		
		// add candidate moves to movelist.
		while(m) {
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];
			movegen_addMove(pos->toMove,KNIGHT,from,to,SQUAREMASKS[to] & opp);
		}
	}
}

void movegen_pawn(POSITION *pos) {

	// m1 -- candidate 1 move pawn pushes
	// m2 -- candidate 2 move pawn pushes
	// mal -- candidate left attacks
	// mar -- candidate right attacks
	// mal -- candidate left ep attacks
	// mar -- candidate right ep attacks

	// remove pawns that are on the last rank, so that we don't overflow.
	uint64_t m1 = pos->pieces[pos->toMove][PAWN] & ~RANKMASKS[pos->toMove == WHITE ? 7 : 0];
	uint64_t m2,mal,mar,malep,marep;
	int sq;

	// 1 move forward pawn pushes.
	m1 = pos->toMove == WHITE ? NORTH(m1) : SOUTH(m1);
	mal = m1 & ~FILEMASKS[pos->toMove==WHITE ? H : A];
	mar = m1 & ~FILEMASKS[pos->toMove==WHITE ? A : H];

	// only if the new square is unoccupied
	m1 &= ~pos->all;
	
	// check for candidate pawns that could do a starting move double push. These are pawns
	// that could move forward one (m1) and are now on the 3rd rank.
	m2 = m1 & RANKMASKS[pos->toMove==WHITE ? 2 : 5];
	m2 = pos->toMove == WHITE ? NORTH(m2) : SOUTH(m2);
	m2 &= ~pos->all;


	// check for left attacks (cleared out the leftmost file above)
	mal = WEST(mal);
	malep = mal;
	mal &= pos->sides[pos->toMove ? 0 : 1];
	malep &= pos->ep;
	
	// check for right attacks (cleared out the rightmost file above)
	mar = EAST(mar);
	marep = mar;
	mar &= pos->sides[pos->toMove ? 0 : 1];
	marep &= pos->ep;
	
	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 8 : sq + 8, sq, false, false, false);
	}

	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 16: sq + 16, sq, false, false, true);
	}	
	
	while(mal) {
		sq = bitScanForward(mal);
		mal ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 9: sq + 9, sq, true,false,false);
	}	

	while(mar) {
		sq = bitScanForward(mar);
		mar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 7: sq + 7, sq,true,false,false);
	}	

	while(malep) {
		sq = bitScanForward(malep);
		malep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 9: sq + 9, sq, false,true,false);
	}

	while(marep) {
		sq = bitScanForward(marep);
		marep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq - 7: sq + 7, sq,false,true,false);
	}		
}


MOVELIST * movegen_getMoves() {
	return &g_moveList;
}


void movegen_init() {

	memset(g_kingMoves,0,sizeof(g_kingMoves));

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			if (DISTANCE(i,j) == 1) {
				g_kingMoves[i] |= SQUAREMASKS[j];
			}
			if ((RANKDISTANCE(i,j) == 1 && FILEDISTANCE(i,j) == 2 )|| 
				(RANKDISTANCE(i,j) == 2 && FILEDISTANCE(i,j) == 1 )) {
				g_knightMoves[i] |= SQUAREMASKS[j];
			}
		}
	}

	printBitboard(g_knightMoves[F3]);
}

void movegen_generate() {

	movegen_clearMoves();
	movegen_pawn(eng_curPosition());
	movegen_king(eng_curPosition());
	movegen_knight(eng_curPosition());
	printMoves(eng_curPosition(),&g_moveList);
}


