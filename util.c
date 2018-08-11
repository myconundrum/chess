#include "chess.h"


const char g_pieceNames[PMAX+1] = {'*','p','n','b','r','q','k'};


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
	return getPieceNameAtBit(pos,eng_bitFileRank(file,rank));
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


void printPosition(POSITION *p) {

	printf("    ");
	for (int f = A; f >= H; f--) {
		printf(" %c |",'H' - f);
	}
	printf("\n");
	for (int r = 7; r >=0 ; r--) {
		printf("%d: |",r);
		for (int f = A; f >= H; f--) {

			if (p->all & eng_bitFileRank(f,r)) {
				printf(" %c |",getPieceNameAtFileAndRank(p,f,r));
			} else {
				printf("   |");
			}
		}
		printf("\n");
	}	
}