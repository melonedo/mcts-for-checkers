
// Entire interface of checker logic

#ifndef CHECKERS_CHERCKERS_H_INCLUDED
#define CHECKERS_CHERCKERS_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

//====== Representation of checker positions ======
// ckr_pos_t class is dedicated to 8x8 checkerboard, where the player on
// the lower part always moves first.
struct checker_position_t
{
  // The side initially on the lower and upper part of board
  uint64_t down, up, king;
  // Number of plies already played. Starting position is 0
  int ply_count;
};
typedef struct checker_position_t ckr_pos_t;

// Return starting position of the checkers
ckr_pos_t ckr_starting_pos(void);

// Comparison
bool ckr_pos_equal(const ckr_pos_t *, const ckr_pos_t *);


//====== Move generation ======
// Opaque buffer
struct checker_engine_t;
typedef struct checker_engine_t *ckr_eng_t;

// Allocate and free memory for a checkere engine
ckr_eng_t ckr_eng_new(void);
void ckr_eng_free(ckr_eng_t);

// Set the checker engine  to examine given position, effectively, generate
// future moves of it and store in buffer.
// This method has large overhead.
void ckr_parse_pos(ckr_eng_t, const ckr_pos_t *);

// ckr_move_num and ckr_get_pos should be called after ckr_load_pos

// Retrieve number of possible moves
int ckr_move_num(ckr_eng_t);

// Generate position with regard to specified move
ckr_pos_t ckr_get_pos(ckr_eng_t, int ind);


//====== Serialization of moves ======
// Moves are represented in a compact string-like form:
// ind0, ind1, ind2, ..., 0,
// where ind0, ind1, ind2, ... represent index of squares a piece occupy
// during the move.

// Make a move according to given string of moves
ckr_pos_t ckr_make_move(const ckr_pos_t *, const char *);

// Find one possible move between given positions
// Return heap memory, caller should free it.
char *ckr_find_move(const ckr_pos_t *old, const ckr_pos_t *new);


//====== Serialization of ckr_pos_t ======
// A position is represented by 64(+1) bytes long string
// For each char, ' ' for illegal squares, '.' for empty squares
// 'b', 'B', 'w', 'W' for black men, black kings, white men, whites kings
// The first 64 bytes will be checked.
ckr_pos_t ckr_deserialize_pos(const char *);

// Return heap memory, caller should free it
char *ckr_serialize_pos(const ckr_pos_t *);

#ifdef __cplusplus
}
#endif
#endif
