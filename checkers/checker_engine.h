// Implementation of move and position generation

#ifndef CHECKERS_CHECKER_ENGINE_H_INCLUDED
#define CHECKERS_CHECKER_ENGINE_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

struct checker_move_t
{
  uint64_t my, opn;
};

#define CHECKER_MAX_MOVE_NUM 64

struct checker_engine_t
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

  // For use in ckr_gen_jump(_fast), record the starting place
  uint64_t start_place;

  // Number of possible moves
  int move_num;
  // Number of pieces captured during the longest jump
  int jump_len;
  // Buffer for all moves
  struct checker_move_t move_list[CHECKER_MAX_MOVE_NUM];
};

// Generate single jumps
void ckr_gen_single_jump(ckr_eng_t);

// Generate (possibly) multiple jumps, using jmp_dir,
// Faster, but flawed
void ckr_gen_jump_fast(ckr_eng_t);

// Backup move generation without using jmp_dir
void ckr_gen_jump(ckr_eng_t);

// Do depth-first search to find all the jumps
// len: length of the jump
// piece: the the piece being moved
// opn: the remaining piecs of the opponent
// start: the starting place for jumps
void ckr_gen_jump_iter(ckr_eng_t, int len, uint64_t piece, uint64_t opn);

// Generate upward or downward walk
// pieces refer to the pieces to test moving
void ckr_gen_up_walk(ckr_eng_t, uint64_t pieces);
void ckr_gen_down_walk(ckr_eng_t, uint64_t pieces);

// Insert a move
void ckr_ins_move(ckr_eng_t, uint64_t my, uint64_t opn);

// Insert a jump when jump length is maximized
// Clear previous moves after a longer jump is inserted
// Ignore a move if shorter than previous jumps
void ckr_ins_jump(ckr_eng_t, uint64_t my, uint64_t opn, int jump);

// Do depth-first search to find suitable move, return true if suceed
// piece: the piece to be examined
// opn: pieces about to capture
bool ckr_parse_jump(ckr_eng_t, uint64_t piece, uint64_t opn);

// Apply the given move
static inline ckr_pos_t ckr_apply_move(
  uint64_t old_my, uint64_t old_opn, uint64_t old_king,
  uint64_t mov_my, uint64_t mov_opn, int new_ply_count, bool is_up);

// DFS to find the suitable move
bool ckr_find_move_jump(char *move_buf, int *move_len,
   uint64_t piece, uint64_t opn);

// ====== The geometry of the board ======
// The layout of the checker board:
// ..01..03..05..07
// 08..10..12..14..
// ..17..19..21..23
// 24..26..28..30..
// ..33..35..37..39
// 40..42..44..46..
// ..49..51..53..55
// 56..58..60..62..

// Helper functions dealing with 4 directions
// The order of directions is ul, ur, dl, ur
enum {ckr_dir_ul = 0, ckr_dir_ur, ckr_dir_dl, ckr_dir_dr};
uint64_t ckr_shift_ul(uint64_t);
uint64_t ckr_shift_ur(uint64_t);
uint64_t ckr_shift_dl(uint64_t);
uint64_t ckr_shift_dr(uint64_t);
uint64_t ckr_shift2_ul(uint64_t);
uint64_t ckr_shift2_ur(uint64_t);
uint64_t ckr_shift2_dl(uint64_t);
uint64_t ckr_shift2_dr(uint64_t);

// Extract least significant bit
uint64_t ckr_ls1b(uint64_t);

// Get the index of least significant bit
int ckr_index(uint64_t);

// Shift all squares to four directions
uint64_t ckr_radius2(uint64_t);

#ifdef __cplusplus
}
#endif
#endif
