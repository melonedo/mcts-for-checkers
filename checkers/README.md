Modules for the logic of checkers.
Written in C to improve extensibility.

Interface include:
CheckerEngine class, for storing temporary (variables) which would otherwise be copied over and over again;
CheckerPosition class, for storing relavent information at any point of a game.

Workflow:
Initialize a (global) CheckerEngine. Do not free it until when the program is about to terminate.
To generate later positions from a given position, call ckr_load_pos, then call ckr_move_num and ckr_get_pos to get later positions.
To find the possible move between two positions, call ckr_get_move.
