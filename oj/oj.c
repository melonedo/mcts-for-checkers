
#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "../test/print.h"
#include "../mcts/msws.c"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../test/print.c"
#include "../mcts/tree.c"

#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <Windows.h>

pthread_t search_thread;
// Protect the game tree
pthread_mutex_t game_tree_mutex;
bool game_ended = false;

void loop(struct CheckerTree *);
void start(struct CheckerTree *);
void place(struct CheckerTree *);
void turn(struct CheckerTree *);
void end(struct CheckerTree *);
// Search the tree
void *search(void *);

int main()
{
  // srand(12345679);
  struct CheckerTree *root = mcts_root();
  pthread_mutex_init(&game_tree_mutex, NULL);
  pthread_create(&search_thread, NULL, search, root);
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

      case 'E':
      end(t);
      return;
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
  static struct CheckerEngine eng_, *eng = &eng_;

  pthread_mutex_lock(&game_tree_mutex);
  // Record
  printf("DEBUG X %d PLACE %d", mcts_rollout_num(t), len);
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');

  struct CheckerPosition pos = ckr_make_move(eng, &t->pos, mov);
  mcts_free_except(t, &pos);

  // Print result for checking
  printf("DEBUG turn #%d %d/%d, %f%% winning\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);

  pthread_mutex_unlock(&game_tree_mutex);
}

void turn(struct CheckerTree *t)
{
  // Do not explicitly search, simply wait
  clock_t end = clock(), start = end;
  if (t->pos.ply_count > 90)
  {
    end += 900;
  }
  else
  {
    end += 1600;
  }
  while (clock() < end)
  {
    Sleep(10);
  }

  pthread_mutex_lock(&game_tree_mutex);
  // Record
  printf("DEBUG X %d TURN\n", mcts_rollout_num(t));

  const char *mov = mcts_extract_best(t);
  printf("DEBUG %ldms\n", clock() - start);
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');

  // Print result
  printf("DEBUG turn #%d %d/%d, %f%% losing\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);
  fflush(stdout);

  pthread_mutex_unlock(&game_tree_mutex);
}

void start(struct CheckerTree *t)
{
  pthread_mutex_lock(&game_tree_mutex);
  int num = 100;
  while (num--)
  {
    mcts_rollout(t);
  }
  scanf("%*d");
  printf("OK\n");
  fflush(stdout);
  pthread_mutex_unlock(&game_tree_mutex);
}

void *search(void *t)
{
  struct CheckerTree *tree = t;
  const int pack_size = 10;
  while (!game_ended)
  {
    // Executed in packs to avoid large overhead locking and unlocking
    pthread_mutex_lock(&game_tree_mutex);
    for (int i = 0; i < pack_size; i++)
      mcts_rollout(tree);
    pthread_mutex_unlock(&game_tree_mutex);
  }
  pthread_exit(NULL);
  __builtin_unreachable();
}

void end(struct CheckerTree *t)
{
  // End the searcher thread
  game_ended = true;
  pthread_join(search_thread, NULL);

  // Free the mutex
  pthread_mutex_lock(&game_tree_mutex);
  pthread_mutex_unlock(&game_tree_mutex);
  pthread_mutex_destroy(&game_tree_mutex);

  // Free the tree
  mcts_free(t);
  free(t);
  printf("DEBUG game ended.\n");
}
