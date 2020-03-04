// Visualize several data structures

#ifndef CKR_PRINT_H_INCLUDED
#define CKR_PRINT_H_INCLUDED

#include "../checkers/checkers.h"

// "  " for unreachable space, ".." for reachable space,
// "bb" and "ww" for black and white men
// "BB" and "WW" for black and white king
// One line title indicates the ply count
void print_position(const struct CheckerPosition *);


#endif
