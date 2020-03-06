// Randomly play games to test

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../checkers/checkers.h"
#include "../mcts/mcts.h"
#include "print.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "print.c"
struct CheckerEngine eng_, *eng = &eng_;

long total_ply = 0;

int random_buf[32];
// Shuffle first len elements of random_buf randomly
void random_shuffle(int len);

// Play the game until it ends.
// Return 1 if current side wins, -1 if it loses
// 0 if draw (in 120 plies)
int rollout(const struct CheckerPosition *);

// Positive if lower wins, negative if upper wins, 0 if draw
int end_game_count(const struct CheckerPosition *);

int main()
{
  printf("Enter a number as seed:\n");
  int seed;
  scanf("%d", &seed);
  srand(seed);
  getchar();

  // Play 1000 games
  struct CheckerPosition pos = ckr_starting_pos();
  int counter = 0;
  const int test_num = 1000000;
  clock_t start_clock = clock();
  for (int i = 0; i < test_num; i++)
  {
    int res = rollout(&pos);
    // if (res == 1)
    //   printf("+ ");
    // else if (res == -1)
    //   printf("- ");
    // else
    //   printf(". ");
    counter += res;
  }
  printf("%lf%%, %ld plies, %ld ms", 100.0 * counter / test_num,
    total_ply, clock() - start_clock);
}

void random_shuffle(int len)
{
  for (int i = len - 1; i >= 0; i--)
  {
    int j = rand() % (i + 1);
    int temp = random_buf[i];
    random_buf[i] = random_buf[j];
    random_buf[j] = temp;
  }
}

int rollout(const struct CheckerPosition *pos_)
{
  struct CheckerPosition pos = *pos_;

  while (pos.ply_count < 120)
  {
    ckr_parse_pos(eng, &pos);
    if (ckr_move_num(eng) == 0)
    {
      total_ply += pos.ply_count;
      // print_position(&pos);getchar();
      if ((pos.ply_count - pos_->ply_count) % 2 != 0)
        return 1;
      else
        return -1;
    }
    struct CheckerPosition old = pos;

    pos = ckr_get_pos(eng, rand() % ckr_move_num(eng));

    assert(*ckr_parse_move(eng, &old, &pos));
    // putchar('\n');
    // print_position(&pos);
    // getchar();
  }
  total_ply += 120;
  int end_count = end_game_count(&pos);
  // putchar('\n');print_position(&pos);getchar();
  if (end_count > 0)
    return pos_->ply_count % 2 != 0 ? -1 : 1;
  else if (end_count < 0)
    return pos_->ply_count % 2 != 0 ? 1 : -1;
  else
    return 0;
}


int end_game_count(const struct CheckerPosition *pos)
{
  return ckr_popcount(pos->down) - ckr_popcount(pos->up) + 3 *
    (ckr_popcount(pos->down & pos->king) - ckr_popcount(pos->up & pos->king));
}
