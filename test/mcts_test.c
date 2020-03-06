
#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "print.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../mcts/tree.c"
#include "print.c"

#include <time.h>

int main()
{
  struct CheckerTree *tree = mcts_root();
  srand(12345);
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++)
  {
    mcts_rollout(tree);
    // printf("%d ", tree->win_num);
  }
  printf("%lldms\n", clock() - start);
  print_tree(tree);
  getchar();
  mcts_free(tree);
}
