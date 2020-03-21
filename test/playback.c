#define MCTS_DEBUG
#define MCTS_STATIC_CKR_ENG

#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "../test/print.h"

#ifdef MCTS_STATIC_CKR_ENG
ckr_eng_t eng;
#endif

#include "../mcts/msws.c"
#include "../checkers/checker_engine.c"
#include "../mcts/tree.c"
#include "../test/print.c"

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

void loop(mcts_tree_t);
void place(mcts_tree_t);
void turn(mcts_tree_t);
void wait(mcts_tree_t);

void show_move(const char *);

FILE *rec;
int main()
{
#ifdef MCTS_STATIC_CKR_ENG
  printf("Using static checker engine.\n");
  eng = ckr_eng_new();
#endif
  msws_srand();
  mcts_tree_t root = mcts_tree_new();
  assert(rec = fopen("../test/record.txt", "r"));
  loop(root);
}

void loop(mcts_tree_t t)
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

void place(mcts_tree_t t)
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
  show_move(mov);

  ckr_pos_t pos = ckr_make_move(&t->pos, mov);
  mcts_tree_choose(t, &pos);
  printf("DEBUG turn #%d %d/%d, %f%% winning\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
  msws_srand();
}

void turn(mcts_tree_t t)
{
  printf("DEBUG X %d TURN\n", mcts_rollout_num(t));
  const ckr_pos_t *best_pos = mcts_tree_get_best(t);
  char *mov = ckr_find_move(mcts_tree_get_pos(t), best_pos);
  show_move(mov);
  free(mov);
  mcts_tree_choose(t, best_pos);
  printf("DEBUG turn #%d %d/%d, %f%% losing\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  print_position(&t->pos);
  msws_srand();
}

void wait(mcts_tree_t t)
{
  int end_num;
  clock_t start_time = clock();
  fscanf(rec, "%d", &end_num);
  printf("X %d\n", end_num);
  sim_count[0] = sim_count[-1] = sim_count[1] = 0;
  while (mcts_rollout_num(t) < end_num)
  {
    mcts_tree_rollout(t);
  }
  printf("W: %d D: %d B: %d\n", sim_count[-1], sim_count[0], sim_count[1]);
  // if (t->pos.ply_count == 55) dump_tree(t);
  printf("%ldms, %ld,%04ld nodes in total (%.2fMB).\n", clock() - start_time,
    node_count / 10000, node_count % 10000,
    sizeof(*t) * node_count / 1e6);
}

void show_move(const char *mov)
{
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
}
