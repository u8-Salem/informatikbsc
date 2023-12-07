/*
Compile: make reversi
Run: ./reversi
make reversi && ./reversi

Reversi Rules
- X moves first
- Pieces are reversed if surrounded (on two sides) by opponent pieces
- A valid move is one in which at least one piece is reversed
- If one player cannot make a move, play passes back to the other player
- When neither player can move, the game ends, the player with the most pieces wins

Details: https://en.wikipedia.org/wiki/Reversi
*/

#include "base.h"

typedef struct {
    int x;
    int y;
} Position;

Position make_position(int x, int y) {
    Position p = { x, y };
    return p;
}

void print_position(Position p) {
    printf("(%d, %d)\n", p.x, p.y);
}

#define N 8

typedef struct {
    char board[N][N]; // the NxN playing board
    char my_stone; // 'X' or 'O'
} Game;

// Initialize the board such that it looks like this if printed:
//  |A|B|C|D|E|F|G|H|
// 1|_|_|_|_|_|_|_|_|
// 2|_|_|_|_|_|_|_|_|
// 3|_|_|_|_|_|_|_|_|
// 4|_|_|_|O|X|_|_|_|
// 5|_|_|_|X|O|_|_|_|
// 6|_|_|_|_|_|_|_|_|
// 7|_|_|_|_|_|_|_|_|
// 8|_|_|_|_|_|_|_|_|
Game init_game(char my_stone) {
    // todo: implement
    Game g; // todo: modify or delete if necessary
    g.my_stone = '?'; // todo: modify or delete if necessary
    return g;
}

// Print the board. The initial board should look like shown above.
void print_board(Game* g) {
    // todo: implement
}

// Check whether position (x,y) is on the board.
bool out_of_bounds(int x, int y) {
    // todo: implement
    return false;
}

// If it is X's turn, then "my stone" is 'X', otherwise it is 'O'.
char my_stone(Game* g) {
    return g->my_stone;
}

// If it is X's turn, then "your stone" is 'O', otherwise it is 'X'.
char your_stone(Game* g) {
    return (g->my_stone == 'X') ? 'O' : 'X';
}

// Switch the my stones and your stones ('X' <--> 'O')
void switch_stones(Game *g) {
    g->my_stone = (g->my_stone == 'X') ? 'O' : 'X';
}

// Check whether, if starting at (x,y), direction (dx,dy) is a legal direction
// to reverse stones. A direction is legal if (assuming my stone is 'X')
// the pattern in that direction is: O+X.* (one or more 'O's, followed by an 'X', 
// followed by zero or more arbitrary characters).
// (dx,dy) is element of { (a,b) | a, b in {-1, 0, 1} and (a,b) != (0,0) }
bool legal_dir(Game* g, int x, int y, int dx, int dy) {
    // todo: implement
    return false;
}

// Check whether (x,y) is a legal position to place a stone. A position is legal
// if it is empty ('_'), is on the board, and has at least one legal direction.
bool legal(Game* g, int x, int y) {
    // todo: implement
    return false;
}

// Reverse stones starting at (x,y) in direction (dx,dy), but only if the 
// direction is legal. May modify the state of the game.
// (dx,dy) is element of { (a,b) | a, b in {-1, 0, 1} and (a,b) != (0,0) }
void reverse_dir(Game* g, int x, int y, int dx, int dy) {
    // todo: implement
}

// Reverse the stones in all legal directions starting at (x,y).
// May modify the state of the game.
void reverse(Game* g, int x, int y) {
    // todo: implement
}

// Input a position of the form D6 or d6, i.e., giving the column first and 
// then the row. A1 corresponds to position (0,0). B1 corresponds to (1,0).
Position input_position(Game* g) {
    printf("%c's turn: ", my_stone(g));
    String s = s_input(10);
    if (s_length(s) >= 1 && s[0] == 'q') exit(0);
    Position pos = { -1, -1 };
    if (s_length(s) >= 2) {
        pos.x = (int)tolower(s[0]) - 'a';
        pos.y = (int)s[1] - '1';
    }
    if (legal(g, pos.x, pos.y)) {
        return pos;
    } else {
        printsln("Invalid position!");
        return input_position(g);
    }
}

int count_stones(Game *g, char c) {
    // implement
    return 0;
}

int main(void) {
    Game g = init_game('X');
    print_board(&g);
    while (true) {
        Position pos = input_position(&g);
        reverse(&g, pos.x, pos.y);
        print_board(&g);
        int score = count_stones(&g, my_stone(&g)) - count_stones(&g, your_stone(&g));
        printf("Score for %c: %d\n", my_stone(&g), score);
        switch_stones(&g);
    }
    return 0;
}
