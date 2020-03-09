

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
  while (true)
  {
    print_position(&pos);
    ckr_parse_pos(&eng, &pos);
    int len;
    scanf("%*s %d", &len);
    char mov[len + 1];
    mov[len] = 0;
    for (int i = 0; i < len; i++)
    {
      int row, col;
      scanf("%d,%d", &row, &col);
      mov[i] = 8 * row + col;
    }
    pos = ckr_make_move(&eng, &pos, mov);
  }
}
