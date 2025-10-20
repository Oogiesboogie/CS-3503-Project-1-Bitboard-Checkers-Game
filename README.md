# CS 3503 Project 1 - Bitboard Checkers Game

## Author
Saja Plath

## Checkers Gameplay Instructions

### Starting the game
1. Run the program in your terminal or IDE
2. You'll see the main menu:
   Project1 BitBoard Checkers
   1) Phase1 tests (bit ops)
   2) Play checkers (interactive)
   3) Quit
   >
3. Enter **2** to start playing checkers

### Game Rules
- **Player 1 (Red)** uses r and R (kings)
- **Player 2 (Black)** uses b and B (kings)
- Lowercase pieces (r, b) move diagonally forward
- Uppercase pieces (R, B) are kings and move diagonally in any direction
- A piece becomes a king when it reaches the opposite end of the board
- Captures are mandatory - if a jump is available, you must take it
- The game ends when one player has no pieces left

### How to move
You can move using board coordinates or numeric positions
Examples: 
  a3 b4 -> moves from a3 to b4
  12 16 -> move from square 12 to 16
The program will validate moves; invalid moves will be rejected 

### Commands During Gameplay
save _filename_ -> saves the current game
load _filename_ -> loads a saved game
quit            -> quits the current game

### Winning
A player wins when the opponent has no legal moves left or no pieces remaining
