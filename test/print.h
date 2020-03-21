// Visualize several data structures

#ifndef CKR_PRINT_H_INCLUDED
#define CKR_PRINT_H_INCLUDED

#include "../checkers/checkers.h"

// "  " for unreachable space, ".." for reachable space,
// "bb" and "ww" for black and white men
// "BB" and "WW" for black and white king
// One line title indicates the ply count
void print_position(const ckr_pos_t *);

// Print parameter as a checker move
// Indices are translated to the format row,column (0-indexed)
// No \n is printed
void print_move(const char *);

// Similar to print_move, but simple return the result
char *repr_move(const char *);

// Print the game tree
// Format:
// 500/1000 - [5,6 4,5]100/300
//          - [3,5 3,6]200/700
void print_tree(const struct CheckerTree *);

// Print the memory as-is
void print_mem(const void *, int len);

// Dump the tree into json (stored in tree.txt)
void dump_tree(const struct CheckerTree *);
#endif
