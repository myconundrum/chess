#include "chess.h"


uint64_t g_kingMoves[64];
uint64_t g_knightMoves[64];

uint64_t g_rookMoves[64];
// Implementing Kingergarten Multiplication for our slider move generators.
uint8_t g_rankMoves[8][256];

MOVELIST g_moveList;

uint64_t g_kCastleMasks[2];
uint64_t g_qCastleMasks[2];

void movegen_clearMoves(POSITION *pos) {

	memset(pos->moves,0,sizeof(pos->moves));
	pos->moveCount = 0;
}

void movegen_addCastleMove(POSITION *pos, int color, bool kSide) {

	pos->moves[pos->moveCount].color = color;
	pos->moves[pos->moveCount].kCastle = kSide;
	pos->moves[pos->moveCount].from = color == WHITE ? E1 : E8;
	
	if (kSide) {
		pos->moves[pos->moveCount].to = color == WHITE ? G1 : G8; 
	} else {
		pos->moves[pos->moveCount].to = color == WHITE ? C1 : C8; 
	}

	pos->moves[pos->moveCount++].qCastle = !kSide;

}

void movegen_addMove(POSITION *pos, int color, int piece, int from, int to, bool capture) {

	pos->moves[pos->moveCount].color = color;
	pos->moves[pos->moveCount].piece = piece;
	pos->moves[pos->moveCount].from = from;
	pos->moves[pos->moveCount].to = to;
	
	//
	// if this is a capture, find the captured piece.
	// Note: EP captures show up as a bool flag (epcapture) vs a capture in the move list.
	//
	if (capture) {
		pos = eng_curPosition();		
		for (PIECES p = PAWN ; p < PMAX; p++) {
			if (SQUAREMASKS[to] & pos->pieces[OPPONENT(color)][p]) {
				pos->moves[pos->moveCount].capture = p;
				break;
			}
		}
	}
	
	pos->moveCount++;
}

void movegen_addPawnMove(POSITION *pos, int color, int piece, int from, int to, bool capture, bool epCapture, bool epMove) {
	//
	// Check to see if this is a pawn promotion, and generate all promotion possibilities if so.
	//
	if (RANKMASKS[color == WHITE ? 7 : 0] & SQUAREMASKS[to]) {
		for (PIECES p = KNIGHT; p < KING; p++) {
			pos->moves[pos->moveCount].promotion = p;
			movegen_addMove(pos,color,piece,from,to,capture);
		}
	} else {
		pos->moves[pos->moveCount].epCapture = epCapture;
		pos->moves[pos->moveCount].epMove = epMove;
		movegen_addMove(pos,color,piece,from,to,capture);
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
			movegen_addMove(pos,pos->toMove,KING,from,to,SQUAREMASKS[to] & opp);
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
			movegen_addMove(pos,pos->toMove,KNIGHT,from,to,SQUAREMASKS[to] & opp);
		}
	}
}

uint64_t movegen_getFileMoves(POSITION * pos, uint8_t sq) {

	uint8_t  file = FILEFROMSQUARE(sq);
	uint64_t occ = AFILE_MASK & (pos->all >> file);
	uint64_t m;

	// map file to the 1st rank.
	occ = (occ * A1H8_MASK) >> 56;

	// map the rook position to the position on the 1st rank and look up the rank.
	m = g_rankMoves[(sq ^ 56) >> 3][occ];

	// map back to the right file and return
	return (HFILE_MASK & (m * A1H8_MASK)) >> (file ^ 7); 
}


uint64_t movegen_getRankMoves(POSITION *pos, uint8_t sq) {

   uint8_t file = sq & 7;
   uint8_t rank = sq >> 3;

   return ((uint64_t) g_rankMoves[file][(uint8_t )(pos->all >> (8*rank))] )<< (8*rank);
}

uint64_t movegen_getDiagonalMoves(POSITION *pos, uint8_t sq) {

 	uint8_t file = sq & 7;

 	// map diagonal in question to first rank.
 	uint64_t occ = ((DIAGONALMASKS[sq] & pos->all) * AFILE_MASK) >> 56;
 	// look up on first rank moves and then transform back to diagonal.
 	uint64_t md = g_rankMoves[file][occ] * AFILE_MASK;
 	uint64_t mad;
 	occ = ((ANTIDIAGONALMASKS[sq] & pos->all) * AFILE_MASK) >> 56;
 	mad = g_rankMoves[file][occ] * AFILE_MASK;

 	return   (md  & DIAGONALMASKS[sq]) | (mad & ANTIDIAGONALMASKS[sq]);

}

void movegen_sliderDiagonal(POSITION *pos, PIECES piece) {

	uint64_t sliders = pos->pieces[pos->toMove][piece];
	uint8_t from, to;
	uint64_t m;
	uint64_t opp  = pos->sides[OPPONENT(pos->toMove)];
	uint64_t self = pos->sides[pos->toMove]; 

	while (sliders) {

		from = bitScanForward(sliders); 
		sliders ^= SQUAREMASKS[from];

		m = movegen_getDiagonalMoves(pos,from);
		while (m) {
			
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];

			if ((SQUAREMASKS[to] & self) == 0) {
				movegen_addMove(pos,pos->toMove,piece,from,to,SQUAREMASKS[to] & opp);
			}
		}
	}
}

void movegen_castling(POSITION *pos) {


	if (pos->kCastle[pos->toMove] && ((g_kCastleMasks[pos->toMove] & pos->all) == 0)) {
		movegen_addCastleMove(pos,pos->toMove,true);
	}

	if (pos->qCastle[pos->toMove] && ((g_qCastleMasks[pos->toMove] & pos->all) == 0)) {
		movegen_addCastleMove(pos,pos->toMove,false);
	}
}


void movegen_sliderFileRank(POSITION *pos,PIECES piece) {

	uint64_t sliders = pos->pieces[pos->toMove][piece];
	uint8_t from, to;
	uint64_t m;
	uint64_t opp  = pos->sides[OPPONENT(pos->toMove)];
	uint64_t self = pos->sides[pos->toMove]; 

	while (sliders) {

		from = bitScanForward(sliders); 
		sliders ^= SQUAREMASKS[from];

		m = movegen_getRankMoves(pos,from);
		while (m) {
			
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];

			if ((SQUAREMASKS[to] & self) == 0) {
				movegen_addMove(pos,pos->toMove,piece,from,to,SQUAREMASKS[to] & opp);
			}
		}

		m = movegen_getFileMoves(pos,from);
		while (m) {
			
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];

			if ((SQUAREMASKS[to] & self) == 0) {
				movegen_addMove(pos,pos->toMove,piece,from,to,SQUAREMASKS[to] & opp);
			}
		}
	}
}


void movegen_queen(POSITION *pos) {
	movegen_sliderDiagonal(pos,QUEEN);
	movegen_sliderFileRank(pos,QUEEN);
}

void movegen_bishop(POSITION *pos) {movegen_sliderDiagonal(pos,BISHOP);}
void movegen_rook(POSITION *pos) {movegen_sliderFileRank(pos,ROOK);}

void movegen_blackPawn(POSITION *pos) {

	// potential 1 forward moves
	uint64_t m1 = SHIFTSOUTH(pos->pieces[BLACK][PAWN]);
	// potential 2 forward moves
	uint64_t m2;
	// potential left attacks (and EP left attacks)
	uint64_t al 	= SHIFTEAST(m1 & ~FILEMASKS[H]);
	uint64_t alep   = al;

	// potential right attacks (and EP right attacks)
	uint64_t ar 	= SHIFTWEST(m1 & ~FILEMASKS[A]);
	uint64_t arep   = ar;
	uint8_t sq; 

	// only allow forward moves if it is not occupied.
	m1 = m1 & ~pos->all;
	// now check for possible two forward moves (both squares must not be occupied)
	m2 = SHIFTSOUTH(m1 & RANKMASKS[5]) & ~pos->all;

	// check for occupied squared for attacks.
	ar &= pos->sides[WHITE];
	arep &= pos->ep;
	al &= pos->sides[WHITE];
	alep &= pos->ep;

	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTH, sq, false, false, false);
	}
	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTH + NORTH, sq, false, false, false);
	}
	while(al) {
		sq = bitScanForward(al);
		al ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTHWEST, sq, true, false, false);
	}
	while(ar) {
		sq = bitScanForward(ar);
		ar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTHEAST, sq, true, false, false);
	}
	while(alep) {
		sq = bitScanForward(alep);
		alep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTHWEST, sq, false, true, false);
	}
	while(arep) {
		sq = bitScanForward(arep);
		arep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,BLACK,PAWN,sq + NORTHEAST, sq, false, true, false);
	}
}



void movegen_whitePawn(POSITION *pos) {

	// potential 1 forward moves
	uint64_t m1 = SHIFTNORTH(pos->pieces[WHITE][PAWN]);
	// potential 2 forward moves
	uint64_t m2;
	// potential left attacks (and EP left attacks)
	uint64_t al 	= SHIFTWEST(m1 & ~FILEMASKS[A]);
	uint64_t alep   = al;

	// potential right attacks (and EP right attacks)
	uint64_t ar 	= SHIFTEAST(m1 & ~FILEMASKS[H]);
	uint64_t arep   = ar;
	uint8_t sq; 

	// only allow forward moves if it is not occupied.
	m1 = m1 & ~pos->all;
	// now check for possible two forward moves (both squares must not be occupied)
	m2 = SHIFTNORTH(m1 & RANKMASKS[2]) & ~pos->all;

	// check for occupied squared for attacks.
	ar &= pos->sides[BLACK];
	arep &= pos->ep;
	al &= pos->sides[BLACK];
	alep &= pos->ep;

	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTH, sq, false, false, false);
	}
	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTH + SOUTH, sq, false, false, false);
	}
	while(al) {
		sq = bitScanForward(al);
		al ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTHEAST, sq, true, false, false);
	}
	while(ar) {
		sq = bitScanForward(ar);
		ar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTHWEST, sq, true, false, false);
	}
	while(alep) {
		sq = bitScanForward(alep);
		alep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTHEAST, sq, false, true, false);
	}
	while(arep) {
		sq = bitScanForward(arep);
		arep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos,WHITE,PAWN,sq + SOUTHWEST, sq, false, true, false);
	}
}

void movegen_pawn(POSITION *pos) {
	if (pos->toMove == WHITE) {
		movegen_whitePawn(pos);
	} else {
		movegen_blackPawn(pos);
	}
}

 


void movegen_initRankMoves() {

	uint8_t test;
	memset(g_rankMoves,0,sizeof(g_rankMoves));

	for (int i = 0; i < 8; i++) {
		for (int occ = 0; occ < 256; occ++) {
			
			// test left.
			test = 0x1 << i;
			while (test != 0x80) {
				test <<= 1;
				g_rankMoves[i][occ] |= test;
				
				if (test & occ) {
					break;
				}
			}

			// test right.
			test = 0x1 << i;
			while (test) {
				test >>= 1;
				g_rankMoves[i][occ] |= test;
				
				if (test & occ) {
					break;
				}
			}
		}
	}
}


void movegen_init() {

	memset(g_kingMoves,0,sizeof(g_kingMoves));
	memset(g_knightMoves,0,sizeof(g_knightMoves));

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			if (DISTANCE(i,j) == 1) {
				g_kingMoves[i] |= SQUAREMASKS[j];
			}
			if ((RANKDISTANCE(i,j) == 1 && FILEDISTANCE(i,j) == 2 )|| 
				(RANKDISTANCE(i,j) == 2 && FILEDISTANCE(i,j) == 1 )) {
				g_knightMoves[i] |= SQUAREMASKS[j];
			}

			if ((RANKDISTANCE(i,j) == 0 || FILEDISTANCE(i,j) == 0) && i != j) {
				g_rookMoves[i] |= SQUAREMASKS[j];
			}
		}
	}

	g_kCastleMasks[WHITE] = BPOS(F1) | BPOS(G1);
	g_kCastleMasks[BLACK] = BPOS(F8) | BPOS(G8);
	g_qCastleMasks[WHITE] = BPOS(B1) | BPOS(C1) | BPOS(D1);
	g_qCastleMasks[BLACK] = BPOS(B8) | BPOS(C8) | BPOS(D8);

	movegen_initRankMoves();
}

void movegen_generate(POSITION *pos) {

	movegen_clearMoves(pos);

	movegen_king(pos);
	movegen_queen(pos);
	movegen_rook(pos);
	movegen_bishop(pos);
	movegen_knight(pos);
	movegen_pawn(pos);

	movegen_castling(pos);
}


