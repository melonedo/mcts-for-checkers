
#include "print.h"
#include "../checkers/checkers.h"
#include <stdio.h>
#include <assert.h>
int print_threshold = 100;
void print_position(const ckr_pos_t *pos)
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

char *repr_move(const char *mov)
{
  char *str = malloc(4 * 64); // CHECKER_MAX_MOVE_NUM
  int len = 0;
  for (int i = 0; mov[i]; i++)
  {
    str[4 * i] = mov[i] / 8 + '0';
    str[4 * i + 1] = ',';
    str[4 * i + 2] = mov[i] % 8 + '0';
    str[4 * i + 3] = ' ';
    len++;
  }
  if (len == 0)
  {
    *str = '\0';
  }
  else
  {
    str[4 * len - 1] = '\0';
  }
  return str;
}

// Append space_num spaces before printing the tree
void print_tree_iter(const struct CheckerTree *, int space_num, const char *move);
void print_tree(const struct CheckerTree *t)
{
  print_tree_iter(t, 0, "root");
  putchar('\n');
}

void print_tree_iter(const struct CheckerTree *t, int space_num, const char *move)
{
  if (space_num > 1400)
  {
    printf("...");
    return;
  }
  if (space_num != 0)
  {
    putchar('[');
    for (int i = 0; move[i]; i++)
    {
      if (i != 0)
      {
        putchar(' ');
      }
      printf("%d,%d", move[i] / 8, move[i] % 8);
    }
    putchar(']');
    space_num += 2 + strlen(move) * 4 - 1;
  }
  int len = snprintf(NULL, 0, "%d/%d - ", t->win_num, t->total_num / 2);
  printf("%d/%d - ", t->win_num, t->total_num / 2);
  space_num += len;

  if (t->child_num > 0)
  {
    bool is_first = true;
    for (int i = 0; i < t->child_num; i++)
    {
      if (t->children[i].total_num <= 2 * print_threshold)
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
      char *last_move = ckr_find_move(&t->pos, &t->children[i].pos);
      print_tree_iter(&t->children[i], space_num, last_move);
      free(last_move);
    }
  }
  else if (t->child_num == MCTS_END_GAME)
  {
    int end = mcts_end_game_count(&t->pos);
    if (end > 0)
    {
      printf("BLACK");
    }
    else if (end < 0)
    {
      printf("WHITE");
    }
    else
    {
      printf("DRAW");
    }
  }
}

void print_mem(const void *m, int len)
{
  const unsigned char *mem = m;
  for (int i = 0; i < len; i++)
  {
    if (i % 8 == 0)
      printf(": ");
    printf("%.2x ", mem[i]);
  }
}

FILE *tree_file;

void dump_tree_iter(const struct CheckerTree *t, const char *move);

void dump_tree(const struct CheckerTree *t)
{
  printf("Dumping the tree...");
  assert(tree_file = fopen("tree.txt", "w"));
  char *board = ckr_serialize_pos(&t->pos);
  printf(" [%s] ", board);
  fprintf(tree_file, " {\"pos\": \"%s\", \"ply\": %d, \"root\": ",
    board, t->pos.ply_count);
  // fprintf(tree_file, "{\"root\": ");
  free(board);
  dump_tree_iter(t, "root");
  fprintf(tree_file, "}  ");
  // fprintf(tree_file, "\n\n\n\n");
  fclose(tree_file);
  printf("Tree dumped.\n");
}

void dump_tree_iter(const struct CheckerTree *t, const char *move)
{
  const char *type_str = NULL;
  if (t->child_num > 0)
  {
    type_str = "INTERIOR";
  }
  else if (t->child_num == MCTS_END_GAME)
  {
    if (mcts_end_game_count(&t->pos) > 0)
      type_str = "BLACK WIN";
    else if (mcts_end_game_count(&t->pos) < 0)
      type_str = "WHITE WIN";
    else
      type_str = "DRAW";
  }
  else if (t->child_num == MCTS_NEW_LEAF)
  {
    type_str = "NEW LEAF";
  }
  else if (t->child_num == MCTS_OLD_LEAF)
  {
    type_str = "OLD LEAF";
  }
  else
  {
    assert(0);
  }
  fprintf(tree_file, "{\"move\": \"%s\", \"type\": \"%s\", \"win_num\": %d, "
  "\"rollout_num\": %d, \"children\": [",
  move, type_str, t->win_num, t->total_num / 2);
  bool is_first = true;
  for (int i = 0; i < t->child_num; i++)
  {
    if (is_first)
    {
      is_first = false;
    }
    else
    {
      fprintf(tree_file, ", ");
    }
    char *move_buf = ckr_find_move(&t->pos, &t->children[i].pos);
    char *move_str = repr_move(move_buf);
    free(move_buf);
    dump_tree_iter(&t->children[i], move_str);
    free(move_str);
  }
  fprintf(tree_file, "]}");
}
