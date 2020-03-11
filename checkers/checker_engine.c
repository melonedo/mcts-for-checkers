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
      ckr_gen_jump(eng);
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
  if (eng->is_up)
  {
    return ckr_apply_move(eng->opn, eng->my, eng->king, eng->move_list[ind].opn,
      eng->move_list[ind].my, eng->ply_count, true);
  }
  else
  {
    return ckr_apply_move(eng->my, eng->opn, eng->king, eng->move_list[ind].my,
      eng->move_list[ind].opn, eng->ply_count, false);
  }
}

static inline ckr_pos ckr_apply_move(
  uint64_t old_down, uint64_t old_up, uint64_t old_king,
  uint64_t mov_down, uint64_t mov_up, int new_ply_count, bool is_up)
{
  ckr_pos rval;
  rval.down = old_down ^ mov_down;
  rval.up = old_up ^ mov_up;
  rval.ply_count = new_ply_count;
  rval.king = old_king;

  // Take care of kings
  if (is_up)
  {
    if (mov_up & old_king)
      rval.king ^= mov_up;
    else
      rval.king |= (rval.up & 0xff00000000000000ull);
    rval.king &= ~mov_down;
  }
  else
  {
    if (mov_down & old_king)
      rval.king ^= mov_down;
    else
      rval.king |= (rval.down & 0x00000000000000ffull);
    rval.king &= ~mov_up;
  }

  return rval;
}

ckr_pos ckr_make_move(ckr_eng eng, const ckr_pos *pos, const char *mov_str)
{
  struct CheckerMove mov = {};
  if (strlen(mov_str) == 2 && abs(mov_str[0] - mov_str[1]) <= 9)
  {
    // Walk
    mov.my = 1ull << mov_str[0] | 1ull << mov_str[1];
  }
  else
  {
    // Jump
    mov.my = 1ull << mov_str[0];
    while (*++mov_str)
    {
      mov.opn |= 1ull << (mov_str[0] + mov_str[-1]) / 2;
    }
    mov.my ^= 1ull << mov_str[-1];
  }

  if (pos->ply_count % 2 != 0)
  {
    return ckr_apply_move(pos->down, pos->up, pos->king, mov.opn, mov.my,
      pos->ply_count + 1, true);
  }
  else
  {
    return ckr_apply_move(pos->down, pos->up, pos->king, mov.my, mov.opn,
      pos->ply_count + 1, false);
  }
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

void ckr_gen_jump(ckr_eng eng)
{
  uint64_t b = eng->jmp_my;
  while (b)
  {
    uint64_t ls1b = b & (-b);
    // Temporarily remove this piece from the board
    eng->nocp ^= ls1b;
    ckr_gen_jump_piecewise(eng, 0, ls1b, eng->opn, ls1b);
    eng->nocp ^= ls1b;
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
#define _CKR_GEN_SINGLE_JUMP(dir) \
b = eng->jmp_dir[ckr_dir_##dir] & eng->my; \
while (b) \
{ \
  uint64_t ls1b = b & (-b); \
  ckr_ins_move(eng, ls1b | ckr_shift2_##dir(ls1b), ckr_shift_##dir(ls1b)); \
  b ^= ls1b; \
}
void ckr_gen_single_jump(ckr_eng eng)
{
  uint64_t b;
  _CKR_GEN_SINGLE_JUMP(dl);
  _CKR_GEN_SINGLE_JUMP(dr);
  _CKR_GEN_SINGLE_JUMP(ul);
  _CKR_GEN_SINGLE_JUMP(ur);
}
#undef _CKR_GEN_SINGLE_JUMP

// Check which piece is not obstructed in one direction, then insert
#define _CKR_GEN_WALK(dir, ops_dir) \
b = ckr_shift_##ops_dir(eng->nocp) & pieces; \
while (b) \
{ \
  uint64_t ls1b = b & (-b); \
  ckr_ins_move(eng, ls1b | ckr_shift_##dir(ls1b), 0); \
  b ^= ls1b; \
}
void ckr_gen_up_walk(ckr_eng eng, uint64_t pieces)
{
  uint64_t b = ckr_shift_dl(eng->nocp) & pieces;
  _CKR_GEN_WALK(ur, dl);
  _CKR_GEN_WALK(ul, dr);
}

void ckr_gen_down_walk(ckr_eng eng, uint64_t pieces)
{
  uint64_t b;
  _CKR_GEN_WALK(dr, ul);
  _CKR_GEN_WALK(dl, ur);
}
#undef _CKR_GEN_WALK

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

#ifdef __cplusplus
}
#endif
