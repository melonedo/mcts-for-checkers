# Only one large file is accepted, I use a scipt to combine them
files = [
    'checkers/checkers.h',
    'checkers/checker_engine.h',
    'checkers/checker_util.h',
    'mcts/mcts.h',
    'mcts/tree.h',
    'mcts/msws.h',
    'checkers/checker_util.c',
    'checkers/checker_engine.c',
    'mcts/tree.c',
    'mcts/msws.c',
    'oj/oj.c']
outfile = 'combined.c'
preamble = """/*
 * This file is automatically combined to be submitted to oj.
 * From my repo:
 * https://github.com/melonedo/mcts-for-checkers
 */\n\n\n
"""

origin_format = "// ------ from {} ------\n"

import re
with open(outfile, 'wt') as outf:
    outf.write(preamble);
    for dir in files:
        with open(dir, 'rt') as inf:
            outf.write(origin_format.format(dir))
            for line in inf:
                if re.fullmatch('#include ".*"\n', line) is None:
                    outf.write(line)
print("Done")
