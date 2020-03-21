// #define MCTS_STATIC_CKR_ENG

#include "../checkers/checkers.h"
#include "../mcts/mcts.h"

#ifdef MCTS_STATIC_CKR_ENG
ckr_eng_t eng;
#endif

#include "../mcts/msws.c"
#include "../checkers/checker_engine.c"
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

void loop(mcts_tree_t);
void start(mcts_tree_t);
void place(mcts_tree_t);
void turn(mcts_tree_t);
void end(mcts_tree_t);
// Search the tree
void *search(void *);

// Print the move string
void show_move(const char *);

int main()
{
  #ifdef MCTS_STATIC_CKR_ENG
    printf("DEBUG Using static checker engine.\n");
    eng = ckr_eng_new();
  #endif
    msws_srand();
    mcts_tree_t root = mcts_tree_new();
    pthread_mutex_init(&game_tree_mutex, NULL);
    pthread_create(&search_thread, NULL, search, root);
    loop(root);
}

void loop(mcts_tree_t t)
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

void place(mcts_tree_t t)
{
  // Read
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

  pthread_mutex_lock(&game_tree_mutex);
  // Log
  printf("DEBUG X %d PLACE ", mcts_rollout_num(t));
  show_move(mov);

  // Real logic
  ckr_pos_t pos = ckr_make_move(&t->pos, mov);
  mcts_tree_choose(t, &pos);

  // Log again
  printf("DEBUG turn #%d %d/%d, %f%% winning\n", t->pos.ply_count,
  t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);

  // Done
  msws_srand();
  pthread_mutex_unlock(&game_tree_mutex);
}

void turn(mcts_tree_t t)
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
  while (clock() < end && mcts_rollout_num(t) < 3000000)
  {
    Sleep(10);
  }

  pthread_mutex_lock(&game_tree_mutex);
  // Log
  printf("DEBUG X %d TURN\n", mcts_rollout_num(t));

  // Real output
  const ckr_pos_t *best_pos = mcts_tree_get_best(t);
  char *mov = ckr_find_move(mcts_tree_get_pos(t), best_pos);
  show_move(mov);
  fflush(stdout);

  // Free everything
  free(mov);
  mcts_tree_choose(t, best_pos);

  // Log again
  printf("DEBUG turn #%d %ldms, %d/%d, %f%% losing\n",
    t->pos.ply_count, clock() - start,
    t->win_num, t->total_num / 2, 100.0 * t->win_num / t->total_num);

  // Done
  msws_srand();
  pthread_mutex_unlock(&game_tree_mutex);
}

void start(mcts_tree_t t)
{
  pthread_mutex_lock(&game_tree_mutex);
  // Explicitly search for a few times
  int num = 100;
  while (num--)
  {
    mcts_tree_rollout(t);
  }

  scanf("%*d");
  printf("OK\n");
  fflush(stdout);
  pthread_mutex_unlock(&game_tree_mutex);
}

void *search(void *t)
{
  const int pack_size = 10;
  while (!game_ended)
  {
    // Executed in packs to avoid large overhead locking and unlocking
    pthread_mutex_lock(&game_tree_mutex);
    for (int i = 0; i < pack_size; i++)
      mcts_tree_rollout(t);
    pthread_mutex_unlock(&game_tree_mutex);
  }
  pthread_exit(NULL);
  __builtin_unreachable();
}

void end(mcts_tree_t t)
{
  // End the searcher thread
  game_ended = true;
  pthread_join(search_thread, NULL);

  // Free the mutex
  pthread_mutex_lock(&game_tree_mutex);
  pthread_mutex_unlock(&game_tree_mutex);
  pthread_mutex_destroy(&game_tree_mutex);

  // Free the tree
  mcts_tree_free(t);
  printf("DEBUG game ended.\n");
}

void show_move(const char *mov)
{
  printf("%d", strlen(mov));
  for (int i = 0; mov[i]; i++)
    printf(" %d,%d", mov[i] / 8, mov[i] % 8);
  putchar('\n');
}
