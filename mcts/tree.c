
#ifdef __cplusplus
extern "C" {
#endif
#include "tree.h"
#include "../checkers/checkers.h"
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

static struct CheckerEngine eng_, *eng = &eng_;

bool mcts_is_leaf(ckr_tree t)
{
  return t->child_num <= 0;
}

enum CheckerLeafType mcts_leaf_type(ckr_tree t)
{
  return t->child_num;
}

ckr_tree mcts_get_child(ckr_tree t, int ind)
{
  return &t->children[ind];
}

int mcts_retrive(ckr_tree t)
{
  return t->win_num - 1;
}

double mcts_win_freq(ckr_tree t)
{
  return (double)t->win_num / (double)t->total_num;
}

int mcts_rollout_num(ckr_tree t)
{
  return t->total_num / 2;
}

void mcts_free(ckr_tree t)
{
  if (!mcts_is_leaf(t))
  {
    for (int i = 0; i < t->child_num; i++)
    {
      mcts_free(&t->children[i]);
    }
    free(t->children);
  }
}

ckr_tree mcts_root(void)
{
  ckr_tree t = calloc(1, sizeof *t);
  t->child_num = MCTS_NEW_LEAF;
  t->pos = ckr_starting_pos();
  return t;
}

int mcts_rollout(ckr_tree t)
{
  int res;
  if (!mcts_is_leaf(t))
  {
    res = -mcts_rollout(mcts_get_child(t, mcts_select(t)));
  }
  else
  {
    switch (mcts_leaf_type(t)) {
      case MCTS_NEW_LEAF:
      t->child_num = MCTS_OLD_LEAF;
      res = mcts_simulate(t);
      break;

      case MCTS_OLD_LEAF:
      mcts_expand(t);
      // After expanding, t could have children, or found it has ended the game
      if (!mcts_is_leaf(t))
        res = -mcts_rollout(mcts_get_child(t, 0));
      else
        res = mcts_retrive(t);
      break;

      case MCTS_END_GAME:
      res = mcts_retrive(t);
      break;

      default:
      assert(0);
      __builtin_unreachable();
    }
  }

  // Record the result
  t->total_num += 2;
  t->win_num += 1 + res;
  return res;
}

// Fisher-Yates algorithm on a sequence of 0..len-1
void mcts_random_permute(int *arr, int len)
{
  for (int i = 0; i < len; i++)
  {
    arr[i] = i;
  }

  for (int i = len-1; i >= 0; i--)
  {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}

// Positive if lower wins, negative if upper wins, 0 if draw
int mcts_end_game_count(const struct CheckerPosition *pos)
{
  int res = ckr_popcount(pos->down) - ckr_popcount(pos->up) + 3 *
    (ckr_popcount(pos->down & pos->king) - ckr_popcount(pos->up & pos->king));
  if (res > 0)
    return 1;
  else if (res < 0)
    return -1;
  else
    return 0;
}

void mcts_expand(ckr_tree t)
{
  if (t->pos.ply_count >= 120)
  {
    int res = mcts_end_game_count(&t->pos);
    t->child_num = MCTS_END_GAME;
    t->total_num = 2;
    t->win_num = 1 + res;
  }
  else
  {
    ckr_parse_pos(eng, &t->pos);
    int len = ckr_move_num(eng);
    if (len)
    {
      // A random order of 0..len-1, used to fill the children randomly
      int perm[len];
      mcts_random_permute(perm, len);
      t->children = calloc(len, sizeof(struct CheckerTree));
      t->child_num = len;

      for (int i = 0; i < len; i++)
      {
        t->children[i].pos = ckr_get_pos(eng, perm[i]);
        t->children[i].child_num = MCTS_NEW_LEAF;
      }
    }
    else
    {
      int res = mcts_end_game_count(&t->pos);
      t->child_num = MCTS_END_GAME;
      t->total_num = 2;
      t->win_num = 1 + (t->pos.ply_count % 2 != 0 ? -res : res);
    }
  }
}

int mcts_simulate(ckr_tree t)
{
  struct CheckerPosition pos = t->pos;
  while (pos.ply_count < 120)
  {
    ckr_parse_pos(eng, &pos);
    if (ckr_move_num(eng) != 0)
    {
      pos = ckr_get_pos(eng, rand() % ckr_move_num(eng));
    }
    else
    {
      return (pos.ply_count - t->pos.ply_count) % 2 != 0 ? 1 : -1;
    }
  }

  int res = mcts_end_game_count(&t->pos);
  return t->pos.ply_count % 2 != 0 ? -res : res;
}

int mcts_select(ckr_tree t)
{
  double maxv = -1000;
  int maxi = 0;
  for (int i = 0; i < t->child_num; i++)
  {
    if (mcts_leaf_type(mcts_get_child(t, i)) == MCTS_NEW_LEAF)
      return i;
    else
    {
      double eval = mcts_evaluate(t, i);
      if (eval > maxv)
      {
          maxi = i;
          maxv = eval;
      }
    }

  }
  return maxi;
}

double mcts_evaluate(ckr_tree t, int ind)
{
  // https://www.chessprogramming.org/UCT
  ckr_tree child = mcts_get_child(t, ind);
  // Note that wins of the child are loses of the root
  return -mcts_win_freq(child) +
    sqrt(2 * log(mcts_rollout_num(t)) / mcts_rollout_num(child));
}

const char *mcts_extract_best(ckr_tree t)
{
  int best = -10000;
  int best_num = 0;
  for (int i = 0; i < t->child_num; i++)
  {
    if (mcts_rollout_num(mcts_get_child(t, i)) > best_num)
    {
      best_num = mcts_rollout_num(mcts_get_child(t, i));
      best = i;
    }
  }

  const char *res = ckr_parse_move(eng, &t->pos, &mcts_get_child(t, best)->pos);

  mcts_free_except_ind(t, best);
  return res;
}

bool ckr_equal(const struct CheckerPosition *l, const struct CheckerPosition *r)
{
  return l->up == r->up && l->down == r->down && l->king == r->king
    && l->ply_count == r->ply_count;
}

void mcts_free_except(ckr_tree t, const struct CheckerPosition *pos)
{
  // Be aware of that sometimes there are identical positions
  // In that case, leave the most visited branch
  int chosen = -1;
  for (int i = 0; i < t->child_num; i++)
  {
    if (ckr_equal(&mcts_get_child(t, i)->pos, pos))
    {
      if (chosen == -1 || mcts_rollout_num(mcts_get_child(t, i)) >
        mcts_rollout_num(mcts_get_child(t, chosen)))
      {
        chosen = i;
      }
    }
  }
  assert(chosen != -1);
  mcts_free_except_ind(t, chosen);
}

void mcts_free_except_ind(ckr_tree t, int ind)
{
  for (int i = 0; i < t->child_num; i++)
  {
    if (i != ind)
    {
      mcts_free(mcts_get_child(t, i));
    }
  }

  // Move best node to the place of the root
  ckr_tree child_list = t->children;
  *t = child_list[ind];
  free(child_list);

}

#ifdef __cplusplus
}
#endif
