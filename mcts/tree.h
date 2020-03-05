// The tree structure to record the game

#ifndef CKR_TREE_H_INCLUDED
#define CKR_TREE_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct CheckerTree
{
  struct CheckerTree *children;
  int child_num;
  // Twice the numbers of winning and total rollouts
  // (to make room for draws)
  int win_num, total_num;
  // Position of current game
  struct CheckerPosition pos;
};

// Expand the given node, order of children is random
// Previous contents of the tree will be cleared
void mcts_expand(ckr_tree);

// Play till the end, return result relative to the given node
// 1, 0, -1 for win, draw, lose each
double mcts_simulate(ckr_tree);

// Test if the node has not been expanded yet
bool mcts_is_leaf(ckr_tree);

// Test if the node has no rollout
bool mcts_is_new(ckr_tree);

// Number of rollouts
int mcts_rollout_num(ckr_tree);

// Frequency of winning rollouts
double mcts_win_freq(ckr_tree);

// Number of children
int mcts_child_num(ckr_tree);

// Get the child with given index
ckr_tree mcts_get_child(int);

#ifdef __cplusplus
}
#endif
#endif
