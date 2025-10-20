#ifndef CHECKERS_BITBOARD_H
#define CHECKERS_BITBOARD_H

#include <stdint.h>

typedef uint64_t U64;

/* Basic bit operations */
unsigned int SetBit(unsigned int value, int position);
unsigned int ClearBit(unsigned int value, int position);
unsigned int ToggleBit(unsigned int value, int position);
int GetBit(unsigned int value, int position);

/* Counting and finding */
int CountBits(unsigned int value);

/* Shift operations */
unsigned int ShiftLeft(unsigned int value, int positions);
unsigned int ShiftRight(unsigned int value, int positions);

/* Display functions */
void PrintBinary(unsigned int value);
void PrintHex(unsigned int value);

/* Game API */
typedef struct {
    U64 p1;
    U64 p1k;
    U64 p2;
    U64 p2k;
    int turn;
} GameState;

void init_game(GameState *g);
void print_board(GameState *g);
int coord_to_index(const char *s);
int apply_move(GameState *g, int from, int to);
int any_capture_available(GameState *g, int player);
int check_win(GameState *g);
void run_phase1_tests(void);
void run_game_interactive(void);

/* Save/load */
int save_game(const char *filename, GameState *g);
int load_game(const char *filename, GameState *g);

#endif /* CHECKERS_BITBOARD_H */