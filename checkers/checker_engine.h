// Implementation of move and position generation

#ifndef CHECKERS_CHECKER_ENGINE_H_INCLUDED
#define CHECKERS_CHECKER_ENGINE_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

struct CheckerMove
{
  uint64_t my, opn;
};

#define CHECKER_MAX_MOVE_NUM 64

struct CheckerEngine
{
  // my for the player about to move
  // opn for his opponent
  // nocp for legal squares that are not occupied
  uint64_t my, opn, nocp, king;
  // Number of plies after next move, i.e., 1 + current ply count
  int ply_count;

  // true if my is up, false if my is down
  bool is_up;

  // jmp_dir contain all squares that could start a jump in specific direction
  // jmp_all contain all squares that could start a jump in whatever directions
  // jmp_my contain those jumps on my side
  uint64_t jmp_all, jmp_my, jmp_dir[4];

  // Number of possible moves
  int move_num;
  // Number of pieces captured during the longest jump
  int jump_len;
  // Buffer for all moves
  struct CheckerMove move_list[CHECKER_MAX_MOVE_NUM];
  // Buffer for result of ckr_parse_move
  char move_str_buf[32];
  int move_str_len;
};

// Generate single jumps
void ckr_gen_single_jump(ckr_eng);

// Generate (possibly) multiple jumps, using jmp_dir,
// Faster, but flawed
void ckr_gen_jump(ckr_eng);


// Backup move generation without using jmp_dir
void ckr_gen_jump_backup(ckr_eng);

// Do depth-first search to find all the jumps
// len: length of the jump
// piece: the the piece being moved
// opn: the remaining piecs of the opponent
// start: the starting place for jumps
void ckr_gen_jump_piecewise(ckr_eng, int len,
  uint64_t piece, uint64_t opn, uint64_t start);

// Generate upward or downward walk
// pieces refer to the pieces to test moving
void ckr_gen_up_walk(ckr_eng, uint64_t pieces);
void ckr_gen_down_walk(ckr_eng, uint64_t pieces);

// Insert a move
void ckr_ins_move(ckr_eng, uint64_t my, uint64_t opn);

// Insert a jump when jump length is maximized
// Clear previous moves after a longer jump is inserted
// Ignore a move if shorter than previous jumps
void ckr_ins_jump(ckr_eng, uint64_t my, uint64_t opn, int jump);

// Do depth-first search to find suitable move, return true if suceed
// piece: the piece to be examined
// opn: pieces about to capture
bool ckr_parse_jump(ckr_eng, uint64_t piece, uint64_t opn);

// Apply the given move
ckr_pos ckr_apply_move(const ckr_pos *, const struct CheckerMove);

#ifdef __cplusplus
}
#endif
#endif
