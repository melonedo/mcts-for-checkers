
// Entire interface of checker logic

#ifndef CHECKERS_CHERCKERS_H_INCLUDED
#define CHECKERS_CHERCKERS_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

////////////////////////////////////Classes/////////////////////////////////////
// Any point between a gameplay
// CheckerPosition class is dedicated to 8x8 checkerboard, where the player on
// the lower part always moves first.
struct CheckerPosition
{
  // The side initially on the lower and upper part of board
  uint64_t down, up;
  // Number of plies already played. Starting position is 0
  int ply_count;
};

// Opaque
struct CheckerEngine;

typedef struct CheckerPosition ckr_pos;
typedef struct CheckerEngine *ckr_eng; // only a reference
typedef const char *ckr_mov; // see ckr_get_move

///////////////////////////////////Functions////////////////////////////////////
// Return starting position of the checkers
ckr_pos ckr_starting_pos();

// Initialized CheckerEngine
void ckr_init_engine(ckr_eng);

//---All below should be called after ckr_init_engine

// Free CheckerEngine
void ckr_free_engine(ckr_eng);

// Set CheckerEngine to examine given position, namely, generate future
// moves of it.
// This method has large overhead.
void ckr_load_pos(ckr_eng, const ckr_pos *);

// ckr_move_num and ckr_get_pos should be called after ckr_load_pos

// Retrieve number of possible moves
int ckr_move_num(ckr_eng);

// Generate position with regard to specified move
ckr_pos ckr_get_pos(ckr_eng, int ind);

// Return a null-terminated string of moves, representing successive squares
// that the piece passes
// Note: return a pointer to internal buffer, do not free it.
ckr_mov ckr_get_move(ckr_eng, ckr_pos old, ckr_pos new);

#ifdef __cplusplus
}
#endif
#endif
