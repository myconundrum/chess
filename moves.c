#include "chess.h"


uint64_t g_kingMoves[64];
uint64_t g_knightMoves[64];


uint64_t g_rookMoves[64];
// Implementing Kingergarten Multiplication for our slider move generators.
uint8_t g_rankMoves[8][256];

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

/*
uint64_t movegen_getFileAttacks(POSITION *pos, uint8_t sq) {

   uint8_t file = sq & 7;
   uint8_t occ = AFILE_MASK & (pos->all >> file);
   occ = (B1H7_MASK *  occ) >> 58;
   occ = A1H8_MASK * g_rankMoves[(sq^56)>>3][occ];
   return  (HFILE_MASK &  occ) >> (file^7);
}
*/


/*
def get_file_moves_bb(i, occ):
    """
    i is index of square
    occ is the combined occupancy of the board
    """
    f = i & np.uint8(7)
    # Shift to A file
    occ = tables.FILES[File.A] & (occ >> f)
    # Map occupancy and index to first rank
    occ = (tables.A1H8_DIAG * occ) >> np.uint8(56)
    first_rank_index = (i ^ np.uint8(56)) >> np.uint8(3)
    # Lookup moveset and map back to H file
    occ = tables.A1H8_DIAG * tables.FIRST_RANK_MOVES[first_rank_index][occ]
    # Isolate H file and shift back to original file
    return (tables.FILES[File.H] & occ) >> (f ^ np.uint8(7))
*/
/*
uint64_t movegen_getFileAttacks(POSITION *pos, uint8_t sq) {

	uint8_t file = sq & 7;
	uint8_t firstRank;
	uint64_t occ = AFILE_MASK & (pos->all << (file ^ 7));



	occ = (occ * A1H8_MASK) >> 56;


	firstRank = 7^((sq ^ 56) >> 3);

	printf("1st rank...\n");
	printBitboard(g_rankMoves[firstRank][occ]);
	printf("times diag...\n");
	printBitboard(A1H8_MASK);
	printf("equals...\n");
	printBitboard(B1H7_MASK  * g_rankMoves[firstRank][occ]);

	occ = A1H8_MASK * (uint64_t) g_rankMoves[firstRank][occ];






	occ = (occ & HFILE_MASK & occ) << (file);
	printf("well...\n");
	printBitboard(occ);

	return occ;


}

*/
/*


uint64_t movegen_getFileAttacksOld(POSITION *pos, uint8_t sq) {

	uint8_t file = sq & 7;
	uint64_t occ = AFILE_MASK & (pos->all << (file^7));
	uint64_t moves;

	printf ("file: %d\n",file);
	printBitboard(FILEMASKS[file] & pos->all);

	printf ("shifted to A file...\n");
	printBitboard(occ);

	occ = (occ * A1H8_MASK) >> 56;
	printf ("shifted to rank 1\n");
	printBitboard(occ);

	printf("index on rank %d\n",7 - ((sq^56) >> 3));
	printf("occ %x\n",(uint8_t) occ);
	
	moves =g_rankMoves[7 ^ ((sq^56) >> 3)][occ];

	printf("moves from lookup\n");
	printBitboard(moves);

	moves = (moves * A1H8_MASK) << 56;

	printf("garbage\n");
	printBitboard(moves);

	printf("masked to h file\n"); 
	printBitboard(moves & HFILE_MASK);


	moves = (HFILE_MASK & moves) << file;
	printf("moves transformed");
	printBitboard(moves);


	return moves;
}
*/
uint64_t movegen_getRankAttacks(POSITION *pos, uint8_t sq) {

   uint8_t file = sq & 7;
   uint8_t rank = sq >> 3;

   return ((uint64_t) g_rankMoves[file][(uint8_t )(pos->all >> (8*rank))] )<< (8*rank);
}


void movegen_rook(POSITION *pos) {

	uint64_t rooks = pos->pieces[pos->toMove][ROOK];
	uint8_t from, to;
	uint64_t m;
	uint64_t opp  = pos->sides[OPPONENT(pos->toMove)];
	uint64_t self = pos->sides[pos->toMove]; 

	while (rooks) {

		from = bitScanForward(rooks); 
		rooks ^= SQUAREMASKS[from];

		m = movegen_getRankAttacks(pos,from);
		while (m) {
			
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];

			if ((SQUAREMASKS[to] & self) == 0) {
				movegen_addMove(pos->toMove,ROOK,from,to,SQUAREMASKS[to] & opp);
			}
		}

		//m = movegen_getFileAttacks(pos,from);
			while (m) {
			
			to = bitScanForward(m);
			m ^= SQUAREMASKS[to];

			if ((SQUAREMASKS[to] & self) == 0) {
				movegen_addMove(pos->toMove,ROOK,from,to,SQUAREMASKS[to] & opp);
			}
		}
	}
}

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
		movegen_addPawnMove(BLACK,PAWN,sq + NORTH, sq, false, false, false);
	}
	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(BLACK,PAWN,sq + NORTH + NORTH, sq, false, false, false);
	}
	while(al) {
		sq = bitScanForward(al);
		al ^= SQUAREMASKS[sq];
		movegen_addPawnMove(BLACK,PAWN,sq + NORTHWEST, sq, true, false, false);
	}
	while(ar) {
		sq = bitScanForward(ar);
		ar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(BLACK,PAWN,sq + NORTHEAST, sq, true, false, false);
	}
	while(alep) {
		sq = bitScanForward(alep);
		alep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(BLACK,PAWN,sq + NORTHWEST, sq, false, true, false);
	}
	while(arep) {
		sq = bitScanForward(arep);
		arep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(BLACK,PAWN,sq + NORTHEAST, sq, false, true, false);
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
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTH, sq, false, false, false);
	}
	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTH + SOUTH, sq, false, false, false);
	}
	while(al) {
		sq = bitScanForward(al);
		al ^= SQUAREMASKS[sq];
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTHEAST, sq, true, false, false);
	}
	while(ar) {
		sq = bitScanForward(ar);
		ar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTHWEST, sq, true, false, false);
	}
	while(alep) {
		sq = bitScanForward(alep);
		alep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTHEAST, sq, false, true, false);
	}
	while(arep) {
		sq = bitScanForward(arep);
		arep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(WHITE,PAWN,sq + SOUTHWEST, sq, false, true, false);
	}
}

void movegen_pawn(POSITION *pos) {
	if (pos->toMove == WHITE) {
		movegen_whitePawn(pos);
	} else {
		movegen_blackPawn(pos);
	}
}

void movegen_pawn_old(POSITION *pos) {

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
	m1 = pos->toMove == WHITE ? SHIFTNORTH(m1) : SHIFTSOUTH(m1);
	mal = m1 & ~FILEMASKS[pos->toMove==WHITE ? A : H];
	mar = m1 & ~FILEMASKS[pos->toMove==WHITE ? H : A];

	// only if the new square is unoccupied
	m1 &= ~pos->all;
	
	// check for candidate pawns that could do a starting move double push. These are pawns
	// that could move forward one (m1) and are now on the 3rd rank.
	m2 = m1 & RANKMASKS[pos->toMove==WHITE ? 2 : 5];
	m2 = pos->toMove == WHITE ? SHIFTNORTH(m2) : SHIFTSOUTH(m2);
	m2 &= ~pos->all;


	// check for left attacks (cleared out the leftmost file above)
	mal = SHIFTWEST(mal);
	malep = mal;
	mal &= pos->sides[pos->toMove ? 0 : 1];
	printf("malep before mask\n");
	printBitboard(malep);
	malep &= pos->ep;
	printf("malep\n");
	printBitboard(malep);
	
	// check for right attacks (cleared out the rightmost file above)
	mar = SHIFTEAST(mar);
	marep = mar;
	mar &= pos->sides[pos->toMove ? 0 : 1];
	printf("marep before mask\n");
	printBitboard(marep);
	marep &= pos->ep;
	printf("marep\n");
	printBitboard(marep);
	
	// add candidate moves to movelist.
	while(m1) {
		sq = bitScanForward(m1);
		m1 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTH : sq + NORTH, sq, false, false, false);
	}

	while(m2) {
		sq = bitScanForward(m2);
		m2 ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTH + SOUTH: sq + NORTH+ NORTH, sq, false, false, true);
	}	
	
	while(mal) {
		sq = bitScanForward(mal);
		mal ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTHEAST: sq + NORTHWEST, sq, true,false,false);
	}	

	while(mar) {
		sq = bitScanForward(mar);
		mar ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTHWEST : sq + NORTHEAST, sq,true,false,false);
	}	

	while(malep) {
		sq = bitScanForward(malep);
		malep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTHEAST : sq + NORTHWEST, sq, false,true,false);
	}

	while(marep) {
		sq = bitScanForward(marep);
		marep ^= SQUAREMASKS[sq];
		movegen_addPawnMove(pos->toMove,PAWN,pos->toMove == WHITE ? sq + SOUTHWEST: sq + NORTHEAST, sq,false,true,false);
	}		
}



 
 /*
U64 diagonalAttacks(U64 occ, enumSquare sq) {
   const U64 aFile = C64(0x0101010101010101);
   const U64 bFile = C64(0x0202020202020202);
 
   unsigned int f = sq & 7;
   occ  =  diagonalMaskEx[sq] & occ;
   occ  = (bFile * occ ) >> 58;
   occ  =  aFile *  firstRankAttacks[f][occ];
   return  diagonalMaskEx[sq] & occ;
}

*/

MOVELIST * movegen_getMoves() {
	return &g_moveList;
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
	movegen_initRankMoves();
}

void movegen_generate() {

	movegen_clearMoves();
	movegen_pawn(eng_curPosition());
	movegen_king(eng_curPosition());
	movegen_knight(eng_curPosition());
	movegen_rook(eng_curPosition());
	printMoves(eng_curPosition(),&g_moveList);
}


