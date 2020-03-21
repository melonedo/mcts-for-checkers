// The tree structure to record the game

#ifndef CKR_TREE_H_INCLUDED
#define CKR_TREE_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include "mcts.h"

struct mcts_tree_t_
{
  struct mcts_tree_t_ *children;
  int child_num;
  // Twice the numbers of winning total rollouts
  // And the difference between the numbers of winning moves and losing ones
  int win_num, total_num;
  // Position of current game
  ckr_pos_t pos;
};

enum CheckerLeafType
{
  // Newly generated, call mcts_simulate
  MCTS_NEW_LEAF = 0,
  // Evaluated once, call mcts_expand
  MCTS_OLD_LEAF = -1,
  // Ends the game decisively, and already evaluated, call mstd_retrive
  MCTS_END_GAME = -2
};


// Expand the given node, order of children is random
// Previous contents of the tree will be cleared
void mcts_expand(mcts_tree_t);

// Evaluate the given node of a tree, here UCT (Upper Confidence bounds applied
// to Trees) is used
// Intergreated into mcts_select to avoid a few computation
double mcts_evaluate(mcts_tree_t, int);

// Next 3 methods below do not modify the tree

// Select one of the children and return the index
int mcts_select(mcts_tree_t);

// Play till the end, return result relative to the given node
// 1, 0, -1 for win, draw, lose each
int mcts_simulate(mcts_tree_t);

// Retrive previous result
int mcts_retrive(mcts_tree_t);

// Test if the node has not been expanded yet
bool mcts_is_leaf(mcts_tree_t);

// Get type leaf fo dispatch, also safe if not called with leaf
enum CheckerLeafType mcts_leaf_type(mcts_tree_t);

// Number of rollouts
int mcts_rollout_num(mcts_tree_t);

// Frequency of winning rollouts
double mcts_win_freq(mcts_tree_t);

// Number of children
int mcts_child_num(mcts_tree_t);

// Get the child with given index
mcts_tree_t mcts_get_child(mcts_tree_t, int);

// Free all other branches
void mcts_free_except_ind(mcts_tree_t, int);

// Calculate the meterial balance after 120 plies
int mcts_end_game_count(const ckr_pos_t *pos);

// Free all branches of the given node
// The node itself is allocated together with sibling nodes as an array,
// So only call free on t->children
void mcts_free_children(mcts_tree_t);

// Return the sign of given integer.
// 1 for positive, -1 for negative, 0 for zero.
static inline int mcts_get_sign(int);

// Population count, count 1 bits
static inline int mcts_popcount(uint64_t);

#ifdef __cplusplus
}
#endif
#endif
