// The whole interface of monte-carlo tree search

#ifndef CHECKER_MCTS_H_INCLUDED
#define CHECKER_MCTS_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

struct CheckerTree;
typedef struct CheckerTree *ckr_tree;

// Initilize the root node
// It is allocated on the heap
ckr_tree mcts_root(void);

// See mcts.txt
// 1, 0, -1 for win, draw, lose each
int mcts_rollout(ckr_tree);

// Deallocate the tree, leaving only the given branch
void mcts_free_except(ckr_tree, const struct CheckerPosition *);

// Return the best move serialized (in internal buffer)
// Free other branches
const char *mcts_extract_best(ckr_tree);

#include "tree.h"
#ifdef __cplusplus
}
#endif
#endif
