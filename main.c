// main.c
#include <stdio.h>
#include "checkers_bitboard.h"

int main(void) {
    while (1) {
        puts("Project1 BitBoard Checkers");
        puts("1) Phase1 tests (bit ops)");
        puts("2) Play checkers (interactive)");
        puts("3) Quit");
        printf("> ");
        char line[32];
        if (!fgets(line, sizeof(line), stdin)) break;
        int choice = atoi(line);
        if (choice == 1) run_phase1_tests();
        else if (choice == 2) run_game_interactive();
        else if (choice == 3) break;
        else puts("Invalid choice.");
    }
    return 0;
}
