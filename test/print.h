// Visualize several data structures

#ifndef CKR_PRINT_H_INCLUDED
#define CKR_PRINT_H_INCLUDED

#include "../checkers/checkers.h"

// "  " for unreachable space, ".." for reachable space,
// "bb" and "ww" for black and white men
// "BB" and "WW" for black and white king
// One line title indicates the ply count
void print_position(const struct CheckerPosition *);

// Print parameter as a checker move
// Indices are translated to the format row,column (0-indexed)
// No \n is printed
void print_move(const char *);

// Print the game tree
// Format:
// 500/1000 - 100/300
//          - 200/700
void print_tree(const struct CheckerTree *);
#endif
