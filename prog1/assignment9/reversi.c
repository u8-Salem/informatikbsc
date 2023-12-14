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

#define N 8

void print_position(Position p) {
    // todo: implement
	//printf("%c", 'A' + 1);
	printf("%c%d\n", 'A' + p.x,1 + p.y);

}

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
    Game g;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            g.board [i][j] = '_';
        }
    }
    int a = N / 2;
    int b = a - 1;
    g.board[b][b] = 'O';
    g.board[a][b] = 'X';
    g.board[b][a] = 'X';
    g.board[a][a] = 'O';
    g.my_stone = my_stone;
    return g;
}

// Print the board. The initial board should look like shown above.
void print_board(Game* g) {
    printsln("\n |A|B|C|D|E|F|G|H|");
    for (int i = 0; i < N; i++) {
        printi(i + 1);
        for (int j = 0; j < N; j++) {
            printf("|%c", g->board[j][i]);
        }
        printsln("|");
    }
}

// Check whether position (x,y) is on the board.
bool out_of_bounds(int x, int y) {
    return x < 0 || x > N - 1 || y < 0 || y > N - 1;
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
    x += dx; y += dy;
    while (!out_of_bounds(x, y) && (g->board[x][y] == your_stone(g))) {
        x += dx; y += dy;
        if (!out_of_bounds(x, y) && (g->board[x][y] == my_stone(g))) {
            return true;
        }
    }
    return false;
}

// Check whether (x,y) is a legal position to place a stone. A position is legal
// if it is empty ('_'), is on the board, and has at least one legal direction.
bool legal(Game* g, int x, int y) {
    return !out_of_bounds(x, y) && (g->board[x][y] == '_') &&
       (legal_dir(g, x, y, -1, -1) || legal_dir(g, x, y, 0, -1) ||
        legal_dir(g, x, y, 1, -1) || legal_dir(g, x, y, -1, 0) ||
        legal_dir(g, x, y, 1, 0) || legal_dir(g, x, y, -1, 1) ||
        legal_dir(g, x, y, 0, 1) || legal_dir(g, x, y, 1, 1));
}

// Reverse stones starting at (x,y) in direction (dx,dy), but only if the 
// direction is legal. May modify the state of the game.
// (dx,dy) is element of { (a,b) | a, b in {-1, 0, 1} and (a,b) != (0,0) }
void reverse_dir(Game* g, int x, int y, int dx, int dy) {
    if (!legal_dir(g, x, y, dx, dy)) return;
    do {
        g->board[x][y] = my_stone(g);
        x += dx; y += dy;
    } while (!out_of_bounds(x, y) && (g->board[x][y] == your_stone(g)));
}

// Reverse the stones in all legal directions starting at (x,y).
// May modify the state of the game.
void reverse(Game* g, int x, int y) {
    reverse_dir(g, x, y, -1, -1); reverse_dir(g, x, y, 0, -1);
    reverse_dir(g, x, y, 1, -1); reverse_dir(g, x, y, -1, 0);
    reverse_dir(g, x, y, 1, 0); reverse_dir(g, x, y, -1, 1);
    reverse_dir(g, x, y, 0, 1); reverse_dir(g, x, y, 1, 1);
}

// Input a position of the form D6 or d6, i.e., giving the column first and 
// then the row. A1 corresponds to position (0,0). B1 corresponds to (1,0).
Position human_move(Game* g) {
    String s = s_input(10);
    if (s_length(s) >= 1 && s[0] == 'q') exit(0);
    // todo: modify to temporarily mark valid moves
	if(s_equals(s, "?") == true){
		//printf("Input ist ?");
		for (int y = 0; y < N; y++) {
			for (int x = 0; x < N; x++) {
				if (legal(g, x, y)) {
					//printf("legaler Zug gefunden");
					g->board[x][y] = '*';
				}
			}
		}
		print_board(g);
		return human_move(g);		
	}
	for (int y = 0; y < N; y++) {
			for (int x = 0; x < N; x++) {
				if (g->board[x][y] == '*') {
					g->board[x][y] = '_';
				}
			}
	}
					
	Position pos = { -1, -1 };
	if (s_length(s) >= 2) {
		pos.x = (int)tolower(s[0]) - 'a';
		pos.y = (int)s[1] - '1';
	}
	
	if (legal(g, pos.x, pos.y)) {
		return pos;
	} else {
		printsln("Invalid position!");
		return human_move(g);
	}
	
}

int count_stones(Game *g, char c) {
    int result = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (g->board[x][y] == c) {
                result++;
            }
        }
    }
    return result;
}

#define POSITION_STACK_SIZE 64
typedef struct {
    int length;
    Position values[POSITION_STACK_SIZE];
} PositionStack;

// Initializes a position stack.
PositionStack make_position_stack() {
    PositionStack ps;
    ps.length = 0;
    return ps;
}

// Pushes a new position on top of the stack.
void push(PositionStack *ps, Position p) {
    // todo: implement
	if(ps->length >= POSITION_STACK_SIZE){
		prints("Stack_Überlauf");
		exit(0);
	}
	ps->values[ps->length++] = p;
}

// Pops the topmost position from the stack.
Position pop(PositionStack *ps) {
    // todo: implement
	if(ps->length <= 0){
		prints("Stack_Unterlaufen");
		exit(0);
	}
	return ps->values[ps->length--];
    //return make_position(0, 0);
}

// Returns a random position from the stack.
Position random_position(PositionStack *ps) {
    // todo: implement
	int rnd_ps = i_rnd(ps->length);
    //return make_position(0, 0);
	return ps->values[rnd_ps];
}

// Tests all positions and chooses a random valid move.
Position computer_move(Game *g) {
    // todo: implement
	PositionStack ps = make_position_stack();
	for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (legal(g, x, y)) {
				push(&ps, make_position(x,y));
			}
		}
	}
    //return make_position(0, 0);
    return random_position(&ps);
}

int main(void) {
    Game g = init_game('X');
    print_board(&g);
    while (true) {
        printf("%c's move: ", my_stone(&g));
        Position pos = human_move(&g);
        reverse(&g, pos.x, pos.y);
        print_board(&g);
        int score = count_stones(&g, my_stone(&g)) - count_stones(&g, your_stone(&g));
        printf("Score for %c: %d\n", my_stone(&g), score);
        switch_stones(&g);

        printf("%c's move: ", my_stone(&g));
        pos = computer_move(&g);
        print_position(pos);
        reverse(&g, pos.x, pos.y);
        print_board(&g);
        score = count_stones(&g, my_stone(&g)) - count_stones(&g, your_stone(&g));
        printf("Score for %c: %d\n", my_stone(&g), score);
        switch_stones(&g);
    }
    return 0;
}
