
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


/*
  ==bitboard mapping convention==

    A  B  C  D  E  F  G  H 
    -  -  -  -  -  -  -  - 
8 | 63 62 61 60 59 58 57 56

7 | 55 54 53 52 51 50 49 48

6 | 47 46 45 44 43 42 41 40

5 | 39 38 37 36 35 34 33 32

4 | 31 30 29 28 27 26 25 24

3 | 23 22 21 20 19 18 17 16 

2 | 15 14 13 12 11 10 09 08

1 | 07 06 05 04 03 02 01 00

*/

typedef enum {
	H1,G1,F1,E1,D1,C1,B1,A1,
	H2,G2,F2,E2,D2,C2,B2,A2,
	H3,G3,F3,E3,D3,C3,B3,A3,
	H4,G4,F4,E4,D4,C4,B4,A4,
	H5,G5,F5,E5,D5,C5,B5,A5,
	H6,G6,F6,E6,D6,C6,B6,A6,
	H7,G7,F7,E7,D7,C7,B7,A7,
	H8,G8,F8,E8,D8,C8,B8,A8
} SQUARES;

// From white's perspective
#define NORTH(sq) 			sq << 8
#define SOUTH(sq) 			sq >> 8
#define EAST(sq)  			sq >> 1
#define WEST(sq)  			sq << 1
#define NORTHEAST(sq)       sq << 7
#define NORTHWEST(sq) 		sq << 9
#define SOUTHEAST(sq)       sq >> 9
#define SOTHWEST(sq) 		sq >> 7


#define RANK1_MASK 0x00000000000000FFull
#define RANK2_MASK 0x000000000000FF00ull
#define RANK3_MASK 0x0000000000FF0000ull
#define RANK4_MASK 0x00000000FF000000ull
#define RANK5_MASK 0x000000FF00000000ull
#define RANK6_MASK 0x0000FF0000000000ull
#define RANK7_MASK 0x00FF000000000000ull
#define RANK8_MASK 0xFF00000000000000ull

#define AFILE_MASK 0x8080808080808080ull
#define BFILE_MASK 0x4040404040404040ull
#define CFILE_MASK 0x2020202020202020ull
#define DFILE_MASK 0x1010101010101010ull
#define EFILE_MASK 0x0808080808080808ull
#define FFILE_MASK 0x0404040404040404ull
#define GFILE_MASK 0x0202020202020202ull
#define HFILE_MASK 0x0101010101010101ull

extern uint64_t FILEMASKS[8];
extern uint64_t RANKMASKS[8];
extern uint64_t SQUAREMASKS[64];
extern uint64_t FRMASKS[8][8];

#define WHITE 0
#define BLACK 1

typedef enum {H=0,G=1,F=2,E=3,D=4,C=5,B=6,A=7,FMAX=8} FILES;
typedef enum {PAWN = 1,KNIGHT,BISHOP,ROOK,QUEEN,KING,PMAX} PIECES;

#define BFR(f,r) 0x01ull << (f + 8*r);
#define BPOS(p)  0x01ull << p;

typedef struct {

	uint64_t pieces[2][PMAX];
	uint64_t sides[2];
	uint64_t all;

} POSITION;



typedef enum {MT_NORMAL=0,MT_ENPASSANT=1,MT_CAPTURE=2} MOVETYPES;

typedef struct {

	uint8_t from 	: 6;
	uint8_t to 		: 6; 
	uint8_t color 	: 1;
	uint8_t piece 	: 3;
	uint8_t type 	: 3;

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
void movegen_generate();


void eng_initPosition();
void eng_init();
POSITION * eng_curPosition();


//util apis
void printMoves(POSITION *pos, MOVELIST * moves);
int bitScanForward(uint64_t bb);
void printBitboard(uint64_t bb);
const char getPieceNameAtBit(POSITION *pos, uint64_t bit);
const char getPieceNameAtFileAndRank(POSITION * pos, int file, int rank);
void positionToFEN(POSITION * p, bool asURL);
void printPosition(POSITION *p);