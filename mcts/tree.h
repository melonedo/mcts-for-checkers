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
void mcts_expand(ckr_tree);

// Evaluate the given node of a tree, here UCT (Upper Confidence bounds applied
// to Trees) is used
// Intergreated into mcts_select to avoid a few computation
double mcts_evaluate(ckr_tree, int);

// Next 3 methods below do not modify the tree

// Select one of the children and return the index
int mcts_select(ckr_tree);

// Play till the end, return result relative to the given node
// 1, 0, -1 for win, draw, lose each
int mcts_simulate(ckr_tree);

// Retrive previous result
int mcts_retrive(ckr_tree);

// Test if the node has not been expanded yet
bool mcts_is_leaf(ckr_tree);

// Get type leaf fo dispatch, also safe if not called with leaf
enum CheckerLeafType mcts_leaf_type(ckr_tree);

// Number of rollouts
int mcts_rollout_num(ckr_tree);

// Frequency of winning rollouts
double mcts_win_freq(ckr_tree);

// Number of children
int mcts_child_num(ckr_tree);

// Get the child with given index
ckr_tree mcts_get_child(ckr_tree, int);

// Free all other branches
void mcts_free_except_ind(ckr_tree, int);

// Calculate the meterial balance after 120 plies
int mcts_end_game_count(const ckr_pos_t *pos);

// Free all branches of the given node
// The node itself is allocated together with sibling nodes as an array,
// So only call free on t->children
void mcts_free(ckr_tree);

// Return the sign of given integer.
// 1 for positive, -1 for negative, 0 for zero.
static inline int mcts_get_sign(int);

#ifdef __cplusplus
}
#endif
#endif
