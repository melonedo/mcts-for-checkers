// The whole interface of monte-carlo tree search

#ifndef CHECKER_MCTS_H_INCLUDED
#define CHECKER_MCTS_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif
#include "../checkers/checkers.h"

struct mcts_tree_t_;
typedef struct mcts_tree_t_ *mcts_tree_t;

// Initilize the root node
// It is allocated on the heap
mcts_tree_t mcts_tree_new(void);

// Free the whole tree
void mcts_tree_free(mcts_tree_t);

// Do a single rollout
// A dispatch function, recursively chooses the matching behavior depend on the
// type of node.
int mcts_tree_rollout(mcts_tree_t);

// Get the current position
const ckr_pos_t *mcts_tree_get_pos(mcts_tree_t);

// Return the game position of the best child
const ckr_pos_t *mcts_tree_get_best(mcts_tree_t);

// Deallocate the tree, leaving only the given branch
void mcts_tree_choose(mcts_tree_t, const ckr_pos_t *);

#ifdef __cplusplus
}
#endif
#endif
