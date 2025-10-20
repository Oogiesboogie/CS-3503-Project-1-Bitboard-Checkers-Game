// checkers_bitboard.c
#include "checkers_bitboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------- Phase 1: Bit manipulation helpers ---------------- */

/* Basic bit operations */
unsigned int SetBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value | (1u << position);
}

unsigned int ClearBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value & ~(1u << position);
}

unsigned int ToggleBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return value;
    return value ^ (1u << position);
}

int GetBit(unsigned int value, int position) {
    if (position < 0 || position >= 32) return 0;
    return (int)((value >> position) & 1u);
}

/* Counting and finding */
int CountBits(unsigned int value) {
    int c = 0;
    while (value) { c += value & 1u; value >>= 1u; }
    return c;
}

/* Shift operations */
unsigned int ShiftLeft(unsigned int value, int positions) {
    if (positions < 0 || positions >= 32) return 0;
    return value << positions;
}

unsigned int ShiftRight(unsigned int value, int positions) {
    if (positions < 0 || positions >= 32) return 0;
    return value >> positions;
}

/* Display functions */
void PrintBinary(unsigned int value) {
    for (int i = 31; i >= 0; --i) {
        putchar((value >> i) & 1u ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}

void PrintHex(unsigned int value) {
    printf("0x%08X\n", value);
}

/* -------------- Phase 2: Checkers using 64-bit bitboards -------------- */

static U64 make_dark_mask(void) {
    U64 m = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if ((r + c) % 2 == 1)
                m |= (1ULL << (r * 8 + c));
    return m;
}

void init_game(GameState *g) {
    memset(g, 0, sizeof(*g));
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 8; ++c) {
            int idx = r * 8 + c;
            if ((r + c) % 2 == 1) g->p2 |= (1ULL << idx);
        }
    for (int r = 5; r < 8; ++r)
        for (int c = 0; c < 8; ++c) {
            int idx = r * 8 + c;
            if ((r + c) % 2 == 1) g->p1 |= (1ULL << idx);
        }
    g->turn = 1;
}

/* Utilities */
static int on_board(int idx) { return idx >= 0 && idx < 64; }
static int occupied(GameState *g, int idx) {
    U64 m = (1ULL << idx);
    return ((g->p1 | g->p2) & m) != 0;
}

int is_player_piece(GameState *g, int player, int idx) {
    U64 m = (1ULL << idx);
    return (player == 1) ? ((g->p1 & m) != 0) : ((g->p2 & m) != 0);
}

int is_king(GameState *g, int player, int idx) {
    U64 m = (1ULL << idx);
    return (player == 1) ? ((g->p1k & m) != 0) : ((g->p2k & m) != 0);
}

void remove_piece(GameState *g, int player, int idx) {
    U64 mask = ~(1ULL << idx);
    if (player == 1) { g->p1 &= mask; g->p1k &= mask; }
    else { g->p2 &= mask; g->p2k &= mask; }
}

void place_piece(GameState *g, int player, int idx, int king) {
    U64 m = (1ULL << idx);
    if (player == 1) { g->p1 |= m; if (king) g->p1k |= m; else g->p1k &= ~m; }
    else { g->p2 |= m; if (king) g->p2k |= m; else g->p2k &= ~m; }
}

void print_board(GameState *g) {
    puts("  +-------------------------------+");
    for (int r = 0; r < 8; ++r) {
        printf("%d |", 8 - r);
        for (int c = 0; c < 8; ++c) {
            int idx = r * 8 + c;
            if ((r + c) % 2 == 0) {
                printf("  ");
                continue;
            }
            char ch = '.';
            U64 m = (1ULL << idx);
            if (g->p1 & m) ch = is_king(g, 1, idx) ? 'R' : 'r';
            else if (g->p2 & m) ch = is_king(g, 2, idx) ? 'B' : 'b';
            printf(" %c", ch);
        }
        printf(" |\n");
    }
    puts("  +-------------------------------+");
    puts("    a b c d e f g h");
    printf("Turn: Player %d (%s)\n\n", g->turn, g->turn == 1 ? "r/R" : "b/B");
}

int coord_to_index(const char *s) {
    if (!s || !s[0]) return -1;
    if (s[0] >= '0' && s[0] <= '9') {
        int v = atoi(s);
        if (v >= 0 && v < 64) return v;
    }
    if ((s[0] >= 'a' && s[0] <= 'h') && s[1]) {
        int file = s[0] - 'a';
        int rank = s[1] - '0';
        if (rank < 1 || rank > 8) return -1;
        int r = 8 - rank;
        return r * 8 + file;
    }
    if ((s[0] >= 'A' && s[0] <= 'H') && s[1]) {
        int file = s[0] - 'A';
        int rank = s[1] - '0';
        if (rank < 1 || rank > 8) return -1;
        int r = 8 - rank;
        return r * 8 + file;
    }
    return -1;
}

static void idx_to_rc(int idx, int *r, int *c) { *r = idx / 8; *c = idx % 8; }

int valid_simple_move(GameState *g, int player, int from, int to) {
    if (!on_board(from) || !on_board(to)) return 0;
    if (occupied(g, to)) return 0;
    if (!is_player_piece(g, player, from)) return 0;
    int fr, fc, tr, tc;
    idx_to_rc(from, &fr, &fc);
    idx_to_rc(to, &tr, &tc);
    if ((tr + tc) % 2 == 0) return 0;
    int dr = tr - fr;
    int dc = tc - fc;
    if (abs(dc) != 1) return 0;
    if (is_king(g, player, from)) {
        return abs(dr) == 1;
    } else {
        return (player == 1) ? (dr == -1) : (dr == 1);
    }
}

int valid_capture(GameState *g, int player, int from, int to, int *captured_idx) {
    if (!on_board(from) || !on_board(to)) return 0;
    if (occupied(g, to)) return 0;
    if (!is_player_piece(g, player, from)) return 0;
    int fr, fc, tr, tc;
    idx_to_rc(from, &fr, &fc);
    idx_to_rc(to, &tr, &tc);
    if ((tr + tc) % 2 == 0) return 0;
    int dr = tr - fr;
    int dc = tc - fc;
    if (abs(dc) != 2 || abs(dr) != 2) return 0;
    int midr = fr + dr / 2;
    int midc = fc + dc / 2;
    if (midr < 0 || midr >= 8 || midc < 0 || midc >= 8) return 0;
    int mididx = midr * 8 + midc;
    int opponent = (player == 1) ? 2 : 1;
    if (!is_player_piece(g, opponent, mididx)) return 0;
    if (!is_king(g, player, from)) {
        if (player == 1 && dr != -2) return 0;
        if (player == 2 && dr != 2) return 0;
    }
    *captured_idx = mididx;
    return 1;
}

int any_capture_available(GameState *g, int player) {
    U64 pieces = (player == 1) ? g->p1 : g->p2;
    for (int from = 0; from < 64; ++from) if ((pieces >> from) & 1ULL) {
        int fr, fc; idx_to_rc(from, &fr, &fc);
        const int drs[4] = {2, 2, -2, -2};
        const int dcs[4] = {2, -2, 2, -2};
        for (int k = 0; k < 4; ++k) {
            int tr = fr + drs[k], tc = fc + dcs[k];
            if (tr < 0 || tr >= 8 || tc < 0 || tc >= 8) continue;
            int to = tr * 8 + tc;
            int mid;
            if (valid_capture(g, player, from, to, &mid)) return 1;
        }
    }
    return 0;
}

int apply_move(GameState *g, int from, int to) {
    int player = g->turn;
    if (!is_player_piece(g, player, from)) return 0;
    if (occupied(g, to)) return 0;
    int capidx = -1;
    if (valid_capture(g, player, from, to, &capidx)) {
        int wasKing = is_king(g, player, from);
        remove_piece(g, player, from);
        remove_piece(g, (player == 1) ? 2 : 1, capidx);
        place_piece(g, player, to, wasKing);
    } else if (valid_simple_move(g, player, from, to)) {
        int wasKing = is_king(g, player, from);
        remove_piece(g, player, from);
        place_piece(g, player, to, wasKing);
    } else {
        return 0;
    }
    int tr = to / 8;
    if (player == 1 && tr == 0) g->p1k |= (1ULL << to);
    if (player == 2 && tr == 7) g->p2k |= (1ULL << to);
    g->turn = (g->turn == 1) ? 2 : 1;
    return 1;
}

static int count_pieces(GameState *g, int player) {
    U64 p = (player == 1) ? g->p1 : g->p2;
    int c = 0;
    while (p) { c += (int)(p & 1ULL); p >>= 1ULL; }
    return c;
}

int check_win(GameState *g) {
    if (count_pieces(g, 1) == 0) return 2;
    if (count_pieces(g, 2) == 0) return 1;
    return 0;
}

/* Save/load simple text format:
   line1: p1 (hex)
   line2: p1k
   line3: p2
   line4: p2k
   line5: turn (1 or 2)
*/

int save_game(const char *filename, GameState *g) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "%016llx\n%016llx\n%016llx\n%016llx\n%d\n",
            (unsigned long long)g->p1,
            (unsigned long long)g->p1k,
            (unsigned long long)g->p2,
            (unsigned long long)g->p2k,
            g->turn);
    fclose(f);
    return 1;
}

int load_game(const char *filename, GameState *g) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    unsigned long long a,b,c,d; int t;
    if (fscanf(f, "%llx%llx%llx%llx%d", &a,&b,&c,&d,&t) != 5) { fclose(f); return 0; }
    g->p1 = (U64)a; g->p1k = (U64)b; g->p2 = (U64)c; g->p2k = (U64)d; g->turn = t;
    fclose(f);
    return 1;
}

/* Phase 1 tests and Phase 2 interactive loop (internal calls) */
void run_phase1_tests(void) {
    unsigned int v = 0;
    printf("Phase 1 tests\n");
    printf("Initial: "); PrintBinary(v);
    v = SetBit(v, 3); printf("SetBit 3: "); PrintBinary(v);
    v = SetBit(v, 31); printf("SetBit 31: "); PrintBinary(v);
    v = ToggleBit(v, 3); printf("ToggleBit 3: "); PrintBinary(v);
    v = ClearBit(v, 31); printf("ClearBit 31: "); PrintBinary(v);
    printf("GetBit pos 3: %d\n", GetBit(v,3));
    printf("CountBits: %d\n", CountBits(v));
    printf("ShiftLeft 2: "); PrintBinary(ShiftLeft(v,2));
    printf("ShiftRight 1: "); PrintBinary(ShiftRight(v,1));
    printf("Hex: "); PrintHex(v);
    puts("Phase 1 tests complete.\n");
}

void run_game_interactive(void) {
    GameState g;
    init_game(&g);
    char buf1[128], buf2[128];
    while (1) {
        print_board(&g);
        int winner = check_win(&g);
        if (winner) { printf("Player %d wins.\n", winner); break; }
        printf("Enter move (from to). Use a3 b4 or numeric 0-63.\n  Commands: save <file>  load <file>  quit\n> ");
        if (!fgets(buf1, sizeof(buf1), stdin)) break;
        char *nl = strchr(buf1, '\n'); if (nl) *nl = 0;
        if (strncmp(buf1, "quit", 4) == 0) break;
        if (strncmp(buf1, "save ", 5) == 0) {
            char *fn = buf1 + 5;
            if (save_game(fn, &g)) printf("Saved to %s\n", fn);
            else printf("Save failed\n");
            continue;
        }
        if (strncmp(buf1, "load ", 5) == 0) {
            char *fn = buf1 + 5;
            if (load_game(fn, &g)) printf("Loaded %s\n", fn);
            else printf("Load failed\n");
            continue;
        }
        /* parse two tokens */
        char *tok1 = strtok(buf1, " \t");
        char *tok2 = strtok(NULL, " \t");
        if (!tok1) continue;
        if (!tok2) {
            if (!fgets(buf2, sizeof(buf2), stdin)) break;
            char *pnl = strchr(buf2, '\n'); if (pnl) *pnl = 0;
            tok2 = strtok(buf2, " \t");
            if (!tok2) continue;
        }
        int from = coord_to_index(tok1);
        int to = coord_to_index(tok2);
        if (from < 0 || to < 0) { puts("Invalid coordinates."); continue; }
        if (any_capture_available(&g, g.turn)) {
            int mid;
            if (!valid_capture(&g, g.turn, from, to, &mid)) {
                puts("A capture is available. You must capture.");
                continue;
            }
        }
        if (!apply_move(&g, from, to)) {
            puts("Illegal move.");
            continue;
        }
    }
    puts("Exiting game.");
}
