// Test the overhead of using pthread

#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "../test/print.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../test/print.c"
#include "../mcts/tree.c"

#include <stdio.h>
#include <pthread.h>
#include <time.h>

int main()
{
  const int rollout_num = 200000;
  printf("%d rollouts without mutex: \n", rollout_num);
  for (int i = 0; i < 3; i++)
  {
    clock_t start = clock();
    struct CheckerTree *t = mcts_root();
    for (int j = 0; j < rollout_num; j++)
    {
      mcts_rollout(t);
    }
    printf("%dms ", clock() - start);
  }
  printf("\n");

  printf("%d rollouts with mutex: \n", rollout_num);
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);
  for (int i = 0; i < 3; i++)
  {
    clock_t start = clock();
    struct CheckerTree *t = mcts_root();
    for (int j = 0; j < rollout_num; j++)
    {
      pthread_mutex_lock(&mutex);
      mcts_rollout(t);
      pthread_mutex_unlock(&mutex);
    }
    printf("%dms ", clock() - start);
  }
  printf("\n");

  printf("%d rollouts without mutex, with packs: \n", rollout_num);
  const int pack_size = 10;
  for (int i = 0; i < 3; i++)
  {
    clock_t start = clock();
    struct CheckerTree *t = mcts_root();
    for (int j = 0; j < rollout_num / pack_size; j++)
    {
      pthread_mutex_lock(&mutex);
      for (int k = 0; k < pack_size; k++)
        mcts_rollout(t);
      pthread_mutex_unlock(&mutex);
    }
    printf("%dms ", clock() - start);
  }

  pthread_mutex_destroy(&mutex);
}
