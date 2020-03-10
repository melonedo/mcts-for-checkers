
#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "../test/print.h"
#include "../mcts/random.c"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../test/print.c"
#include "../mcts/tree.c"

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

void loop(struct CheckerTree *);
void place(struct CheckerTree *);
void turn(struct CheckerTree *);
void wait(struct CheckerTree *);

int main()
{
  // srand(12345679);
  struct CheckerTree *root = mcts_root();
  loop(root);
}

void loop(struct CheckerTree *t)
{
  while (true)
  {
    char buf[256];
    scanf("%s", buf);
    switch (buf[0])
    {
      case 'P':
      place(t);
      break;

      case 'T':
      turn(t);
      break;

      case 'X':
      wait(t);
    }
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
  printf("DEBUG X %d PLACE %d", mcts_rollout_num(t), len);
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
  static struct CheckerEngine eng_, *eng = &eng_;
  struct CheckerPosition pos = ckr_make_move(eng, &t->pos, mov);
  mcts_free_except(t, &pos);
  printf("DEBUG turn #%d %d/%d, %f%% winning\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
}

void turn(struct CheckerTree *t)
{
  printf("DEBUG X %d TURN\n", mcts_rollout_num(t));
  const char *mov = mcts_extract_best(t);
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
  printf("DEBUG turn #%d %d/%d, %f%% losing\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
}

void wait(struct CheckerTree *t)
{
  int end_num;
  scanf("%d", &end_num);
  while (mcts_rollout_num(t) < end_num)
  {
    mcts_rollout(t);
  }
  print_threshold = 5000;
  // print_tree(t);
}
