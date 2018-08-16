
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


/** 
 bitboard mapping convention for our chess engine is LERF mapping (Little Endian Rank File Mapping)

	Rank 1 to Rank 7 is 0 -> 7
	File A to File H is 0 -> 7

   	A  B  C  D  E  F  G  H 
    -  -  -  -  -  -  -  - 
8 | 56 57 58 59 60 61 62 63
7 | 48 49 50 51 52 53 54 55
6 | 40 41 42 43 44 45 46 47
5 | 32 33 34 35 36 37 38 39
4 | 24 25 26 27 28 29 30 31
3 | 16 17 18 19 20 21 22 23
2 | 08 09 10 11 12 13 14 15
1 | 00 01 02 03 04 05 06 07


a-file             0x0101010101010101
h-file             0x8080808080808080
1st rank           0x00000000000000FF
8th rank           0xFF00000000000000
a1-h8 diagonal     0x8040201008040201
h1-a8 antidiagonal 0x0102040810204080
light squares      0x55AA55AA55AA55AA
dark squares       0xAA55AA55AA55AA55


**/

typedef enum {

	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3, 
	A4, B4, C4, D4, E4, F4, G4, H4, 
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7, 
	A8, B8, C8, D8, E8, F8, G8, H8

} SQUARES;



#define RANK1_MASK 0x00000000000000FFull
#define RANK2_MASK 0x000000000000FF00ull
#define RANK3_MASK 0x0000000000FF0000ull
#define RANK4_MASK 0x00000000FF000000ull
#define RANK5_MASK 0x000000FF00000000ull
#define RANK6_MASK 0x0000FF0000000000ull
#define RANK7_MASK 0x00FF000000000000ull
#define RANK8_MASK 0xFF00000000000000ull



#define AFILE_MASK 0x0101010101010101ull
#define BFILE_MASK 0x0202020202020202ull
#define CFILE_MASK 0x0404040404040404ull
#define DFILE_MASK 0x0808080808080808ull
#define EFILE_MASK 0x1010101010101010ull
#define FFILE_MASK 0x2020202020202020ull
#define GFILE_MASK 0x4040404040404040ull
#define HFILE_MASK 0x8080808080808080ull
#define A1H8_MASK  0x8040201008040201ull


typedef enum {NORTH=8,SOUTH=-8,EAST=1,WEST=-1,NORTHEAST=9,NORTHWEST=7,SOUTHEAST=-7,SOUTHWEST=-9} CARDINALS;
#define SHIFTNORTH(bb) 		((bb) << 8)
#define SHIFTNORTHEAST(bb)	((bb) << 9)
#define SHIFTNORTHWEST(bb)  ((bb) << 7)
#define SHIFTWEST(bb)		((bb) >> 1)
#define SHIFTEAST(bb) 		((bb) << 1)
#define SHIFTSOUTH(bb)		((bb) >> 8)
#define SHIFTSOUTHWEST(bb)  ((bb) >> 9)
#define SHIFTSOUTHEAST(bb)  ((bb) >> 7)
	


typedef enum {A,B,C,D,E,F,G,H,FMAX} FILES;
typedef enum {PAWN = 1,KNIGHT,BISHOP,ROOK,QUEEN,KING,PMAX} PIECES;

#define WHITE 0
#define BLACK 1

#define OPPONENT(c) (c == WHITE ? BLACK : WHITE)
#define FILEFROMSQUARE(sq)  ((sq) & 7)
#define RANKFROMSQUARE(sq)  ((sq) >> 3)


#define BFR(f,r) (0x01ull << ((f) + 8*(r)))
#define BPOS(p)  (0x01ull << (p))	



extern uint64_t FILEMASKS[8];
extern uint64_t RANKMASKS[8];
extern uint64_t SQUAREMASKS[64];
extern uint64_t FRMASKS[8][8];





typedef struct {

	uint64_t pieces[2][PMAX];  	//bitboard by piece
	uint64_t sides[2];		   	//bitboard by side
	uint64_t all;			   	//all pieces bitboard
	uint64_t ep;			  	//en passant

	uint8_t  toMove;			//side to move
	bool 	 kCastle[2];		//flag for kingside castle avail
	bool	 qCastle[2];		//flag for queenside castle avail
	uint16_t halfMoves;			//half moves in the position				
	uint16_t fullMoves; 		//full moves in the position

} POSITION;

typedef struct {

	uint8_t from 			: 6; // from square
	uint8_t to 				: 6; // to square
	uint8_t color 			: 1; // color making the move
	uint8_t piece 			: 3; // piece moving
	uint8_t capture         : 3; // zero if no capture, otherwise piece
	uint8_t epCapture		: 1; // captured en passant
	uint8_t epMove			: 1; // en passant move
	uint8_t promotion 	    : 3; // zero if not a promotion, otherwise promo piece

} MOVE;


#define MAX_MOVES 1024
typedef struct {

	MOVE moves[MAX_MOVES];
	int count;

} MOVELIST;

//ui apis
bool ui_running();
void ui_init();
void ui_update();

//movegen apis
void movegen_init();
void movegen_generate();


void eng_initPosition();
void eng_init();
POSITION * eng_curPosition();


//util apis

extern const char * g_squareNames[];



 #define MAX(a,b)  (((a) > (b)) ? (a) : (b))
 #define MIN(a,b)  (((a) < (b)) ? (a) : (b))
 #define FILEDISTANCE(a,b) abs(((a)&7) - ((b)&7))
 #define RANKDISTANCE(a,b) abs(((a)>>3) - ((b)>>3))
 #define DISTANCE(a,b) MAX(FILEDISTANCE(a,b),RANKDISTANCE(a,b))


int distance(int sq1, int sq2);
void printMoves(POSITION *pos, MOVELIST * moves);
int bitScanForward(uint64_t bb);
void printBitboard(uint64_t bb);
const char getPieceNameAtBit(POSITION *pos, uint64_t bit);
const char getPieceNameAtFileAndRank(POSITION * pos, int file, int rank);
void positionToFEN(POSITION * p, bool asURL);
void printPosition(POSITION *p);