
#include "print.h"
#include <stdio.h>
#include <assert.h>

void print_position(const struct CheckerPosition *pos)
{
  printf("After %d plies: ", pos->ply_count);
  uint64_t b = 1;
  for (int i = 0; i < 64; i++, b <<= 1)
  {
    if (i % 8 == 0)
      putchar('\n');
    if (b & CHECKER_ILLEGAL_SQUARE)
      printf("  ");
    else if (b & pos->up)
      if (b & pos->king)
        printf("WW");
      else
        printf("ww");
    else if (b & pos->down)
     if (b & pos->king)
      printf("BB");
    else
      printf("bb");
    else
      printf("..");
  }
  putchar('\n');
}

void print_move(const char *move)
{
  while (*move)
  {
    printf("%d,%d ", *move / 8, *move % 8);
    move++;
  }
}
