
#include <assert.h>
#include "../checkers/checkers.h"
#include "../checkers/checker_util.c"
#include "../checkers/checker_engine.c"
#include "../mcts/mcts.h"
#include "print.h"
#include "print.c"

int main()
{
  struct CheckerEngine eng;
  struct CheckerPosition pos = ckr_starting_pos();
  FILE *rec = fopen("place.txt", "r");
  assert(rec);
  while (true)
  {
    print_position(&pos);
    print_mem(&pos, 24);putchar('\n');
    assert((pos.king & ~(pos.up | pos.down)) == 0);
    ckr_parse_pos(&eng, &pos);
    int len;
    while (true)
    {
      char buf[10];
      fscanf(rec, "%s", buf);
      if (buf[0] == 'P')
      {
        fscanf(rec, "%d", &len);
        break;
      }
      else if (buf[0] == 'X')
      {
        fscanf(rec, "%*d");
      }
    }
    char mov[len + 1];
    mov[len] = 0;
    for (int i = 0; i < len; i++)
    {
      int row, col;
      fscanf(rec, "%d,%d", &row, &col);
      mov[i] = 8 * row + col;
    }
    pos = ckr_make_move(&eng, &pos, mov);
  }
}
