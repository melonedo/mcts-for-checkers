// Implementation of move and position generation

#ifdef __cplusplus
extern "C" {
#endif

#include "checker_engine.h"
#include <stdlib.h>
#include <string.h>

#define CHECKER_ILLEGAL_SQUARE 0xAA55AA55AA55AA55ull
#define CHECKER_LEGAL_SQUARE ~0xAA55AA55AA55AA55ull

void ckr_parse_pos(ckr_eng eng, const ckr_pos *pos)
{
  // Load related data
  eng->is_up = (pos->ply_count % 2 != 0);
  eng->ply_count = pos->ply_count + 1;

  if (eng->is_up)
  {
    eng->my = pos->up;
    eng->opn = pos->down;
  }
  else
  {
    eng->my = pos->down;
    eng->opn = pos->up;
  }
  eng->king = pos->king;

  eng->nocp = ~(eng->my | eng->opn | CHECKER_ILLEGAL_SQUARE);

  // Generate the jump board, which will not be used later if no possible jump
  eng->jmp_dir[ckr_dir_dl] = ckr_shift2_ur(eng->nocp) & ckr_shift_ur(eng->opn);
  eng->jmp_dir[ckr_dir_dr] = ckr_shift2_ul(eng->nocp) & ckr_shift_ul(eng->opn);
  eng->jmp_dir[ckr_dir_ul] = ckr_shift2_dr(eng->nocp) & ckr_shift_dr(eng->opn);
  eng->jmp_dir[ckr_dir_ur] = ckr_shift2_dl(eng->nocp) & ckr_shift_dl(eng->opn);
  eng->jmp_all =
    eng->jmp_dir[0] | eng->jmp_dir[1] | eng->jmp_dir[2] | eng->jmp_dir[3];
  eng->jmp_my = eng->jmp_all & eng->my;

  eng->move_num = 0;

  if (eng->jmp_my)
  {
    // Check whether within two units range from one square about to initiate
    // jump, there are another square that can initiate jump
    if(ckr_radius2(eng->jmp_my) & eng->jmp_all)
    {
      // Possible long jumps
      eng->jump_len = 1;
      // "Fast" version is implemented later
      ckr_gen_jump_backup(eng);
    }
    else
    {
      // Long jumps are impossible
      eng->jump_len = 1;
      ckr_gen_single_jump(eng);
    }
  }
  else
  {
    eng->jump_len = 0;
    if (eng->is_up)
    {
      ckr_gen_down_walk(eng, eng->my);
      ckr_gen_up_walk(eng, eng->my & eng->king);
    }
    else
    {
      ckr_gen_up_walk(eng, eng->my);
      ckr_gen_down_walk(eng, eng->my & eng->king);
    }
  }
}

ckr_pos ckr_get_pos(ckr_eng eng, int ind)
{
  // Because ckr_eng does not actually carry a ckr_pos, I do not want to
  // call ckr_apply_move here
  ckr_pos rval;
  uint64_t my = eng->move_list[ind].my, opn = eng->move_list[ind].opn;
  if (eng->is_up)
  {
    rval.up = eng->my ^ my;
    rval.down = eng->opn ^ opn;
  }
  else
  {
    rval.down = eng->my ^ my;
    rval.up = eng->opn ^ opn;
  }

  // Take care of kings
  if (my & eng->king)
    rval.king = my ^ eng->king;
  else if (eng->is_up)
    rval.king = eng->king | (my & 0x5500000000000000ull);
  else
    rval.king = eng->king | (my & 0x00000000000000AAull);

  rval.ply_count = eng->ply_count;
  return rval;
}

ckr_pos ckr_apply_move(const ckr_pos *pos, const struct CheckerMove *mov)
{
  ckr_pos rval;
  if (pos->ply_count % 2 != 0)
  {
    rval.up = pos->up ^ mov->my;
    rval.down = pos->down ^ mov->opn;
  }
  else
  {
    rval.down = pos->down ^ mov->my;
    rval.up = pos->up ^ mov->opn;
  }

  // Take care of kings
  if (mov->my & pos->king)
    rval.king = mov->my ^ pos->king;
  else if (pos->ply_count % 2 != 0)
    rval.king = pos->king | (mov->my & 0x5500000000000000ull);
  else
    rval.king = pos->king | (mov->my & 0x00000000000000AAull);

  rval.ply_count = pos->ply_count + 1;
  return rval;
}

ckr_pos ckr_make_move(ckr_eng, const ckr_pos *pos, const char *mov)
{
  struct CheckerMove c_mov = {};
  if (strlen(mov) == 2 && abs(mov[0] - mov[1]) <= 9)
  {
    // Walk
    c_mov.my = 1ull << mov[0] | 1ull << mov[1];
  }
  else
  {
    // Jump
    c_mov.my = 1ull << mov[0];
    while (*++mov)
    {
      c_mov.opn |= 1ull << (mov[0] - mov[-1]) / 2;
    }
    c_mov.my ^= 1ull << mov[-1];
  }
  return ckr_apply_move(pos, c_mov);
}

int ckr_move_num(ckr_eng eng)
{
  return eng->move_num;
}

const char *ckr_parse_move(ckr_eng eng, const ckr_pos *old, const ckr_pos *new)
{
  uint64_t my, move_my, move_opn;
  if (old->ply_count % 2 != 0)
  {
    my = old->up;
    move_my = old->up ^ new->up;
    move_opn = old->down ^ new->down;
  }
  else
  {
    my = old->down;
    move_my = old->down ^ new->down;
    move_opn = old->up ^ new->up;
  }
  if (move_opn)
  {
    // Jump
    eng->move_str_len = 0;
    uint64_t b;
    if (move_my)
      b = move_my & my;
    else
      b = my;
    while (b)
    {
      uint64_t ls1b = b & (-b);
      if (ckr_parse_jump(eng, ls1b, move_opn))
        break;
      b ^= ls1b;
    }
  }
  else
  {
    // Walk does not affect the opponent
    eng->move_str_len = 2;
    eng->move_str_buf[0] = ckr_index(move_my & my);
    eng->move_str_buf[1] = ckr_index(move_my & ~my);
  }
  eng->move_str_buf[eng->move_str_len] = 0;
  return eng->move_str_buf;
}

bool ckr_parse_jump(ckr_eng eng, uint64_t piece, uint64_t opn)
{
  eng->move_str_buf[eng->move_str_len++] = ckr_index(piece);
  // Check if everything is done
  if (!opn)
    return true;

  uint64_t b;
  b = ckr_shift_dl(piece);
  if (b & opn && ckr_parse_jump(eng, ckr_shift_dl(b), opn ^ b))
    return true;

  b = ckr_shift_dr(piece);
  if (b & opn && ckr_parse_jump(eng, ckr_shift_dr(b), opn ^ b))
    return true;

  b = ckr_shift_ul(piece);
  if (b & opn && ckr_parse_jump(eng, ckr_shift_ul(b), opn ^ b))
    return true;

  b = ckr_shift_ur(piece);
  if (b & opn && ckr_parse_jump(eng, ckr_shift_ur(b), opn ^ b))
    return true;

  eng->move_str_len--;
  return false;
}

ckr_pos ckr_starting_pos()
{
  ckr_pos rval;
  rval.up =   0x0000000000AA55AAull;
  rval.down = 0x55AA550000000000ull;
  rval.king = 0;
  rval.ply_count = 0;
  return rval;
}

void ckr_gen_jump_backup(ckr_eng eng)
{
  uint64_t b = eng->jmp_my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_gen_jump_piecewise(eng, 0, ls1b, eng->opn, ls1b);
    b ^= ls1b;
  }
}

void ckr_gen_jump_piecewise(ckr_eng eng, int len,
    uint64_t piece, uint64_t opn, uint64_t start)
{
  // ckr_ins_jump will ignore short or immature moves
  ckr_ins_jump(eng, start ^ piece, eng->opn ^ opn, len);

  uint64_t next;
  len++;

  // Examine the four directions one by one
  next = ckr_shift2_dl(piece);
  if (next & ckr_shift_dl(opn) & eng->nocp)
    ckr_gen_jump_piecewise(eng, len, next, opn ^ ckr_shift_dl(piece), start);

  next = ckr_shift2_dr(piece);
  if (next & ckr_shift_dr(opn) & eng->nocp)
   ckr_gen_jump_piecewise(eng, len, next, opn ^ ckr_shift_dr(piece), start);

  next = ckr_shift2_ul(piece);
  if (next & ckr_shift_ul(opn) & eng->nocp)
  ckr_gen_jump_piecewise(eng, len, next, opn ^ ckr_shift_ul(piece), start);

  next = ckr_shift2_ur(piece);
  if (next & ckr_shift_ur(opn) & eng->nocp)
   ckr_gen_jump_piecewise(eng, len, next, opn ^ ckr_shift_ur(piece), start);

}

// Simply read jump_dir and insert moves
void ckr_gen_single_jump(ckr_eng eng)
{
  uint64_t b;
  b = eng->jmp_dir[ckr_dir_dl] & eng->my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift2_dl(ls1b), ckr_shift_dl(ls1b));
    b ^= ls1b;
  }

  b = eng->jmp_dir[ckr_dir_dr] & eng->my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift2_dr(ls1b), ckr_shift_dr(ls1b));
    b ^= ls1b;
  }

  b = eng->jmp_dir[ckr_dir_ul] & eng->my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift2_ul(ls1b), ckr_shift_ul(ls1b));
    b ^= ls1b;
  }

  b = eng->jmp_dir[ckr_dir_ur] & eng->my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift2_ur(ls1b), ckr_shift_ur(ls1b));
    b ^= ls1b;
  }
}

// Check which piece is not obstructed in one direction, then insert
void ckr_gen_up_walk(ckr_eng eng, uint64_t pieces)
{
  uint64_t b = ckr_shift_dl(eng->nocp) & pieces;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift_ur(ls1b), 0);
    b ^= ls1b;
  }

  b = ckr_shift_dr(eng->nocp) & pieces;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift_ul(ls1b), 0);
    b ^= ls1b;
  }
}

void ckr_gen_down_walk(ckr_eng eng, uint64_t pieces)
{
  uint64_t b;
  b = ckr_shift_ul(eng->nocp) & pieces;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift_dr(ls1b), 0);
    b ^= ls1b;
  }

  b = ckr_shift_ur(eng->nocp) & pieces;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    ckr_ins_move(eng, ls1b | ckr_shift_dl(ls1b), 0);
    b ^= ls1b;
  }
}

void ckr_ins_move(ckr_eng eng, uint64_t my, uint64_t opn)
{
  eng->move_list[eng->move_num].my = my;
  eng->move_list[eng->move_num].opn = opn;
  eng->move_num++;
}

void ckr_ins_jump(ckr_eng eng, uint64_t my, uint64_t opn, int jump)
{
  if (jump == eng->jump_len)
  {
    ckr_ins_move(eng, my, opn);
  }
  else if (jump > eng->jump_len)
  {
    eng->jump_len = jump;
    eng->move_num = 0;
    ckr_ins_move(eng, my, opn);
  }
  else
  {
    // jump < eng->jump_len, do nothing
    return;
  }
}

// // Two macros for iterations based on individual bits
// // x is uint64_t, the bit array to be iterated on
// // exports one bit of x at one time, ls1b
// #define BITWISE_FOR_BEGIN(x) while (x) { uint64_t ls1b = x & (-x)
// #define BITWISE_FOR_END(x) x ^= ~ls1b; }

#ifdef __cplusplus
}
#endif
