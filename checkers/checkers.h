
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
  uint64_t down, up, king;
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

// Allocate and free memory for a CheckerEngine
ckr_eng ckr_new_eng(void);
void ckr_free_eng(ckr_eng);

// Set CheckerEngine to examine given position, namely, generate future
// moves of it.
// This method has large overhead.
void ckr_parse_pos(ckr_eng, const ckr_pos *);

// ckr_move_num and ckr_get_pos should be called after ckr_load_pos

// Retrieve number of possible moves
int ckr_move_num(ckr_eng);

// Generate position with regard to specified move
ckr_pos ckr_get_pos(ckr_eng, int ind);

// Return a null-terminated string of moves, representing successive squares
// that the piece passes
// Note: return a pointer to internal buffer, do not free it.
const char *ckr_parse_move(ckr_eng, const ckr_pos *old, const ckr_pos *new);

// Make a move according to given string of moves
ckr_pos ckr_make_move(ckr_eng, const ckr_pos *, const char *);

// Find one possible move between given positions
// Similar to ckr_parse_move, but return heap memory, caller should free it
char *ckr_find_move(const ckr_pos *old, const ckr_pos *new);

// Serialization/deserialization support
// For the string representation, ' ' for illegal squares, '.' for empty squares
// 'b', 'B', 'w', 'W' for black men, black kings, white men, whites kings
// The first 64 bytes will be checked
ckr_pos ckr_deserialize_pos(const char *);
// Return heap memory, caller should free it
char *ckr_serialize_pos(const ckr_pos *);
#include "checker_engine.h"
#include "checker_util.h"

#ifdef __cplusplus
}
#endif
#endif
