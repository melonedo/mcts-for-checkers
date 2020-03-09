
#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "../test/print.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../test/print.c"
#include "../mcts/tree.c"

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>


void loop(struct CheckerTree *);
void start(struct CheckerTree *);
void place(struct CheckerTree *);
void turn(struct CheckerTree *);
int main()
{
  srand(1234);
  struct CheckerTree *root = mcts_root();
  // print_threshold = 10000;
  loop(root);
}

void loop(struct CheckerTree *t)
{
  while (true)
  {
    char buf[256];
    scanf("%s", buf);
    switch (buf[0]) {
      case 'S':
      start(t);
      break;

      case 'P':
      place(t);
      break;

      case 'T':
      turn(t);
      break;
    }
    printf("DEBUG [%s] %d of %d, %f%%\n", buf,
    t->win_num / 2, t->total_num / 2, 100*(1.0-1.0*t->win_num/t->total_num));
    // print_position(&t->pos);
    fflush(stdout);
  }
}

void place(struct CheckerTree *t)
{
  int len;
  scanf("%d", &len);
  char mov[len + 1];
  mov[len] = 0;
  for (int i = 0; i < len; i++)
  {
    int row, col;
    scanf("%d,%d", &row, &col);
    mov[i] = 8 * row + col;
  }
  static struct CheckerEngine eng_, *eng = &eng_;
  struct CheckerPosition pos = ckr_make_move(eng, &t->pos, mov);
  // print_move(mov);putchar('\n');
  // print_tree(t);
  mcts_free_except(t, &pos);
  // print_tree(t);
}

void turn(struct CheckerTree *t)
{
  clock_t start = clock();
  int num = 100000;
  while (num--)
  {
    mcts_rollout(t);
  }
  // print_tree(t);

  const char *mov = mcts_extract_best(t);
  printf("DEBUG %ldms\n", clock() - start);
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
}

void start(struct CheckerTree *t)
{
  int num = 100;
  while (num--)
  {
    mcts_rollout(t);
  }
  scanf("%*d");
  printf("OK\n");
}
