
#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "print.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../mcts/tree.c"
#include "print.c"

#include <time.h>
#include <inttypes.h>
int main()
{
  struct CheckerTree *tree = mcts_root();
  srand(12345);
  clock_t start = clock();
  for (int i = 0; i < 100000; i++)
  {
    mcts_rollout(tree);
    // printf("%d ", tree->win_num);
  }
  printf("%ldms\n", clock() - start);
  print_move(mcts_extract_best(tree));
  putchar('\n');
  print_tree(tree);
  getchar();
}
