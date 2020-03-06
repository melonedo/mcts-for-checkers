
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

// Append space_num spaces before printing the tree
void print_tree_iter(const struct CheckerTree *, int space_num);
void print_tree(const struct CheckerTree *t)
{
  print_tree_iter(t, 0);
}

void print_tree_iter(const struct CheckerTree *t, int space_num)
{
  int len = snprintf(NULL, 0, "%d/%d - ", t->win_num / 2, t->total_num / 2);
  printf("%d/%d - ", t->win_num / 2, t->total_num / 2);
  space_num += len;

  if (t->child_num > 0)
  {
    bool is_first = true;
    for (int i = 0; i < t->child_num; i++)
    {
      if (t->children[i].total_num <= 200)
        continue;
      if (is_first)
      {
        is_first = false;
      }
      else
      {
        putchar('\n');
        for (int i = 0; i < space_num; i++)
        putchar(' ');
      }
      print_tree_iter(&t->children[i], space_num);
    }
  }
}
