#define MCTS_DEBUG

#include "../checkers/checkers.h"
ckr_eng_t eng;
#include "../mcts/mcts.h"
#include "../test/print.h"
#include "../mcts/msws.c"
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

FILE *rec;
int main()
{
  msws_srand();
  eng = ckr_new_eng();
  struct CheckerTree *root = mcts_root();
  assert(rec = fopen("../test/record.txt", "r"));
  loop(root);
}

void loop(struct CheckerTree *t)
{
  while (true)
  {
    char buf[256];
    if (fscanf(rec, "%s", buf) != 1)
    {
      printf("No more records!\n");
      return;
    }
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
  fscanf(rec, "%d", &len);
  char mov[len + 1];
  mov[len] = 0;
  for (int i = 0; i < len; i++)
  {
    int row, col;
    fscanf(rec, "%d,%d", &row, &col);
    mov[i] = 8 * row + col;
  }
  printf("DEBUG X %d PLACE %d", mcts_rollout_num(t), len);
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
  ckr_pos_t pos = ckr_make_move(&t->pos, mov);
  mcts_free_except(t, &pos);
  printf("DEBUG turn #%d %d/%d, %f%% winning\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
  msws_srand();
}

void turn(struct CheckerTree *t)
{
  printf("DEBUG X %d TURN\n", mcts_rollout_num(t));
  char *mov = mcts_extract_best(t);
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
  free(mov);
  printf("DEBUG turn #%d %d/%d, %f%% losing\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
  msws_srand();
}

void wait(struct CheckerTree *t)
{
  int end_num;
  clock_t start_time = clock();
  fscanf(rec, "%d", &end_num);
  printf("X %d\n", end_num);
  sim_count[0] = sim_count[-1] = sim_count[1] = 0;
  while (mcts_rollout_num(t) < end_num)
  {
    mcts_rollout(t);
  }
  printf("W: %d D: %d B: %d\n", sim_count[-1], sim_count[0], sim_count[1]);
  // if (t->pos.ply_count == 55) dump_tree(t);
  printf("%dms, %d,%04d nodes in total (%.2fMB).\n", clock() - start_time,
    node_count / 10000, node_count % 10000,
    sizeof(struct CheckerTree) * node_count / 1e6);
}
