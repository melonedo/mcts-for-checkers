int black_win = 0;
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
#include <pthread.h>
#include <Windows.h>

pthread_t search_thread;
// Protect the game tree
pthread_mutex_t game_tree_mutex;

void loop(struct CheckerTree *);
void start(struct CheckerTree *);
void place(struct CheckerTree *);
void turn(struct CheckerTree *);

// Search the tree
void *search(void *);

int main()
{
  srand(1234);
  struct CheckerTree *root = mcts_root();
  pthread_mutex_init(&game_tree_mutex, NULL);
  pthread_create(&search_thread, NULL, search, root);
  loop(root);
  pthread_mutex_destroy(&game_tree_mutex);
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
  struct CheckerPosition pos = ckr_make_move(eng, &t->pos, mov);
  // print_move(mov);putchar('\n');
  // print_tree(t);
  mcts_free_except(t, &pos);
  pthread_mutex_unlock(&game_tree_mutex);
  // print_tree(t);
}

void turn(struct CheckerTree *t)
{
  // Do not explicitly search, simply wait
  clock_t end = clock();
  if (t->pos.ply_count > 90)
  {
    end += 1000;
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
  const char *mov = mcts_extract_best(t);
  printf("%zd", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
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
  while (true)
  {
    // Executed in packs to avoid large overhead locking and unlocking
    pthread_mutex_lock(&game_tree_mutex);
    for (int i = 0; i < pack_size; i++)
      mcts_rollout(tree);
    pthread_mutex_unlock(&game_tree_mutex);
  }
}
