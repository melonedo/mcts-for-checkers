// Test the random simulation
// For unknown reason, when I play white (up) and there are exactly 1 white king
// and a black king, during random simulations, white *always* wins the game.
// From game statistics, in this condition, black never wins a single simulation

#define MCTS_DEBUG
#include <stdio.h>
#include <inttypes.h>

#include "../checkers/checkers.h"
#include "../test/print.h"

#include "../../pcg-c-basic/pcg_basic.c"
#include "../mcts/msws.c"
#include "../checkers/checker_engine.c"
#include "../mcts/tree.c"
#include "../test/print.c"

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

int start_ply;
ckr_pos_t root_pos;
// Reads the record, stop at given ply
void setup_root(void);

// Random simulation
void simulate(void);

// Test whether the distribution is uniform
void test_uniform(void);

void set_seed()
{
  // pcg32_srandom(135434683,65489222);
  msws_srand();
}

int get_rand(int bound)
{
  return (msws() >> 15) % bound;
  // return pcg32_boundedrand(bound);
}

int main()
{
  set_seed();
  // test_uniform();
  printf("Enter the ply you want to stop: ");
  scanf("%d", &start_ply);
  getchar();// Get rid of '\n'
  root_pos = ckr_starting_pos();
  setup_root();

  while (true)
  {
    simulate();
  }
}

void setup_root(void)
{
  FILE *rec;
  assert(rec = fopen("place.txt", "r"));
  while (root_pos.ply_count < start_ply)
  {
    char buf[256];
    int len;
    char mov_str[64];
    assert(fscanf(rec, "%s %d", buf, &len) == 2);
    assert(len < 64);
    mov_str[len] = 0;
    for (int i = 0; i < len; i++)
    {
      int row, col;
      fscanf(rec, "%d,%d", &row, &col);
      mov_str[i] = 8 * row + col;
    }
    root_pos = ckr_make_move(&root_pos, mov_str);
  }
  print_position(&root_pos);
  fclose(rec);
}

void simulate(void)
{
  ckr_eng_t eng = ckr_eng_new();
  ckr_pos_t pos = root_pos;
  printf("Root: \n");
  print_position(&pos);
  while (pos.ply_count < 120)
  {
    ckr_parse_pos(eng, &pos);
    if (ckr_move_num(eng) == 0)
    {
      break;
    }
    else
    {
      pos = ckr_get_pos(eng, get_rand(ckr_move_num(eng)));
    }
    getchar();
    printf("%d moves\n", ckr_move_num(eng));
    print_position(&pos);
  }
  printf("Game has ended!\n");
  ckr_eng_free(eng);
  getchar();
}

void test_uniform(void)
{
  const int sample_num = 100000;
  for (int i = 2; i <= 5; i++)
  {
    printf("Testing samples %% %d\n", i);
    int count[i];
    memset(count, 0, sizeof *count * i);
    for (int j = 0; j < sample_num; j++)
    {
      count[get_rand(i)]++;
    }
    for (int j = 0; j < i; j++)
    {
      printf("%d ", count[j]);
    }
    putchar('\n');
  }
}
