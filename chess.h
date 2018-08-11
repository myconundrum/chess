
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

//ui apis
bool ui_running();
void ui_init();
void ui_update();


//engine apis



uint64_t eng_bitFile(int file);
uint64_t eng_bitRank(int rank);
uint64_t eng_bitFileRank(int file, int rank);

void eng_initPosition();
void eng_init();
POSITION * eng_curPosition();



//util apis
void printBitboard(uint64_t bb);
const char getPieceNameAtBit(POSITION *pos, uint64_t bit);
const char getPieceNameAtFileAndRank(POSITION * pos, int file, int rank);
void positionToFEN(POSITION * p, bool asURL);
void printPosition(POSITION *p);