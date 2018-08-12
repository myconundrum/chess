#include "chess.h"


const char g_pieceNames[PMAX+1] = {'*','p','n','b','r','q','k'};



const char * g_squareNames[] = {
	"h1","g1","f1","e1","d1","c1","b1","a1",
	"h2","g2","f2","e2","d2","c2","b2","a2",
	"h3","g3","f3","e3","d3","c3","b3","a3",
	"h4","g4","f4","e4","d4","c4","b4","a4",
	"h5","g5","f5","e5","d5","c5","b5","a5",
	"h6","g6","f6","e6","d6","c6","b6","a6",
	"h7","g7","f7","e7","d7","c7","b7","a7",
	"h8","g8","f8","e8","d8","c8","b8","a8"
};


void printBitboard(uint64_t bb) {

	uint64_t mask = 0x01ull << 63;
	while (mask) {
		printf("%c",mask & bb ? '1' : '0');
		mask >>= 1;
	}
	puts("");
}



const char getPieceNameAtBit(POSITION *pos, uint64_t bit) {

	for (int i = 0; i < 2; i++) {
		for (PIECES p = PAWN; p < PMAX; p++) {
			if (pos->pieces[i][p] & bit) {
				return i == WHITE ? toupper(g_pieceNames[p]) : g_pieceNames[p];
			}
		}
	}

	return ' ';
}

const char getPieceNameAtFileAndRank(POSITION * pos, int file, int rank) {
	return getPieceNameAtBit(pos,FRMASKS[file][rank]);
}

void positionToFEN(POSITION * p, bool asURL) {
	
	uint64_t mask = 0x1ull << 63;
	int i = 0, emptyCount = 0, bufPos = 0;
	char buf[256];
	memset(buf,0,sizeof(buf));
	

	while (mask) {
		
		// get name of piece if occupied, otherwise count up on empty squares.
		if (p->all & mask) {

			if (emptyCount) {
				buf[bufPos++] = '0' + emptyCount;
				emptyCount = 0;
			}
			buf[bufPos++] = getPieceNameAtBit(p,mask);

		} else {
			emptyCount++;
		}

		// rank splits
		if (++i%8 == 0) {
			if (emptyCount) {
				buf[bufPos++] = '0'+emptyCount;
			}
			if (!asURL) {
				buf[bufPos++] = '/';
			} else {
				buf[bufPos++] = '%';
				buf[bufPos++] = '2';
				buf[bufPos++] = 'F';
			}	
			emptyCount = 0;
		}
		mask >>= 1;
	}

	if (asURL) {
		printf("http://www.ee.unb.ca/cgi-bin/tervo/fen.pl?select=");
	}
	
	printf("%s\n",buf);
}


int fileFromIndex(int square) {return square & 7;}
int rankFromIndex(int square) {return square >> 3;}

const int g_index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};
/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
int bitScanForward(uint64_t bb) {
   const uint64_t debruijn64 = 0x03f79d71b4cb0a89ull;
  
   return g_index64[((bb & -bb) * debruijn64) >> 58];
}

void printMoves(POSITION *pos, MOVELIST * moves) {
	
	for (int i = 0; moves && i < moves->count; i++) {

		if (moves->moves[i].piece != PAWN) {
			printf("%c",toupper(g_pieceNames[moves->moves[i].piece]));
		}
		printf("%s",g_squareNames[moves->moves[i].from]);
		if (moves->moves[i].capture || moves->moves[i].epCapture) {
			printf("x");
		}
		printf("%s",g_squareNames[moves->moves[i].to]);
		if (moves->moves[i].epCapture) {
			printf("ep");
		}
		if (moves->moves[i].promotion) {
			printf("=%c",toupper(g_pieceNames[moves->moves[i].promotion]));
		}
		printf("\n");
	}
}


void printPosition(POSITION *p) {

	printf("**POSITION DUMP**\n");
	printf("%s to move.\n",p->toMove == WHITE ? "White" : "Black");
	if (p->kCastle[BLACK]) {
		printf("Black can castle kingside.\n");
	}
	if (p->qCastle[BLACK]) {
		printf("Black can castle queenside.\n");
	}
	if (p->qCastle[WHITE]) {
		printf("White can castle queenside.\n");
	}
	if (p->kCastle[WHITE]) {
		printf("White can castle kingside.\n");
	}
	printf("%d half moves and %d full moves.\n",p->halfMoves,p->fullMoves);
	if (p->ep) {
		printf("%s is en passant square\n",g_squareNames[bitScanForward(p->ep)]);
	}


	printf("\n    ");
	for (int f = A; f >= H; f--) {
		printf(" %c |",'H' - f);
	}
	printf("\n");
	for (int r = 7; r >=0 ; r--) {
		printf("%d: |",r+1);
		for (int f = A; f >= H; f--) {

			if (p->all & FRMASKS[f][r]) {
				printf(" %c |",getPieceNameAtFileAndRank(p,f,r));
			} else {
				printf("   |");
			}
		}
		printf("\n");
	}	
}