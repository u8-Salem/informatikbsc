/*
Eval player. Picks the "best" move using a simple heuristic.
make eval_player

This player accepts commands from standard input. It is designed to be used with
the reversi controller. The reversi controller accepts a number of flags and the
names of the two player programs as follows:

Usage: ./reversi_controller {-flags} <playerX> <playerO>
Example: ./reversi_controller -pause=100ms ./eval_player ./random_player
Flags:
  -directory string
        Sets the directory in which the games are located. (default ".")
  -longmove duration
        Gives a threshold duration above which the time of a move is shown (in
        ms or s, the unit must be given after the value). If omitted or
        negative, move durations are not shown. (default -1ms)
  -pause duration
        Waiting time after each move (in ms or s, the unit must be given after
        the value). If omitted or negative, waits for <Return> key press after
        each move. This flag only has an effect if flag "show" is true. (default
        -1ms)
  -repeat int
        The number of times the game is played. (default 1)
  -show
        Sets whether every move (true) or only the end result (false) is shown.
        (default true)
  -stderr
        Sets whether stderr of the players should be printed (true) or not
        (false).
  -timeout duration
        Defines the maximum duration that a player's move may take (in ms or s,
        the unit must be given after the value). 0 means no timeout.

The controller starts the two player programs and talks to them via their
standard input, standard output, and standard error streams. The following
commands are recognized:

Commands (cp = controller to player, pc = player to controller, '|' means or):

Initialization (cp, tells the player, which stone type it has):
init: X | O

Seeding the random number generator (cp):
srand: <number>

Stopping the player program (cp):
exit

Receiving the opponent's move (or an indication that the opponent could not make
a move) and responding with the next move (or an indication that no move can be
made):
d6 | none (cp, opponent move)
e6 | none (pc, own move)
*/

///////////////////////////////////////////////////////////////////////////////

#include "base.h"

bool s_starts_with(String s, String start) {
    require_not_null(s);
    require_not_null(start);
    String p = strstr(s, start);
    return p == s;
}

int s_index(String s, String part) {
    require_not_null(s);
    require_not_null(part);
    String p = strstr(s, part);
    if (p == NULL) return -1;
    return p - s;
}

#define N 8

typedef struct {
    char board[N][N]; // the NxN playing board
} Game;

typedef struct {
    int x, y;
} Direction;

Direction directions[] = {
    {-1, -1}, {0, -1}, {1, -1},
    {-1, 0}, {1, 0},
    {-1, 1}, {0, 1}, {1, 1}};
#define directions_count (sizeof(directions) / sizeof(directions[0]))

typedef struct {
    int x, y;
} Position;

void print_position(Position p) {
    printf("%c%d\n", p.x + 'A', p.y + 1);
}

// Checks whether position (x,y) is on the board.
bool in_bounds(Position pos) {
    return pos.x >= 0 && pos.x < N && pos.y >= 0 && pos.y < N;
}

// Parses the string representation of a position (like B3) to a pos (like (1, 2)).
bool parse_position(/*in*/char* s, /*out*/Position* pos) {
    pos->x = (int)toupper(s[0]) - 'A';
    pos->y = (int)s[1] - '1';
    return in_bounds(*pos);
}

void add_dir(/*inout*/Position* p, Direction d) {
    p->x += d.x;
    p->y += d.y;
}

typedef struct {
    char stone;
} Player;

// Returs the color of the player.
char player_stone(Player* p) {
    return p->stone;
}

// Returs the color of the opponent player.
char opponent_stone(Player* p) {
    return (p->stone == 'X') ? 'O' : 'X';
}

// Initializes the board such that it looks like this if printed:
//  |A|B|C|D|E|F|G|H|
// 1|_|_|_|_|_|_|_|_|
// 2|_|_|_|_|_|_|_|_|
// 3|_|_|_|_|_|_|_|_|
// 4|_|_|_|O|X|_|_|_|
// 5|_|_|_|X|O|_|_|_|
// 6|_|_|_|_|_|_|_|_|
// 7|_|_|_|_|_|_|_|_|
// 8|_|_|_|_|_|_|_|_|
Game init_game(void) {
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
    return g;
}

// Prints the board. The initial board should look like shown above.
void print_board(Game* g) {
    fprintf(stderr, " |A|B|C|D|E|F|G|H|\n");
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "%d", i + 1);
        for (int j = 0; j < N; j++) {
            fprintf(stderr, "|%c", g->board[j][i]);
        }
        fprintf(stderr, "|\n");
    }
    fflush(stderr);
}

// Returns the stone at the given position.
char stone_at(Game* g, Position pos) {
    require("position in bounds", in_bounds(pos));
    return g->board[pos.x][pos.y];
}

// Sets the stone at the given position to the given value.
void set_stone(Game* g, Position pos, char c) {
    require("position in bounds", in_bounds(pos));
    require("valid stone color", c == 'X' || c =='O');
    g->board[pos.x][pos.y] = c;
}

bool is_empty(Game* g, Position pos) {
    return stone_at(g, pos) == '_';
}

// Checks whether, starting at pos, dir is a legal direction for p.
// A direction is legal if at least one stone can be reversed in that direction.
bool legal_dir(Game* g, Player* p, Position pos, Direction dir) {
    add_dir(&pos, dir);
    while (in_bounds(pos) && stone_at(g, pos) == opponent_stone(p)) {
        add_dir(&pos, dir);
        if (in_bounds(pos) && stone_at(g, pos) == player_stone(p)) {
            return true;
        }
    }
    return false;
}

// Checks whether pos is a legal position for player p to place a stone. A
// position is legal if it is on the board, the corresponding cell is empty, and
// there is at least one legal direction starting at that cell.
bool legal_move(Game* g, Player* p, Position pos) {
    if (!in_bounds(pos) || !is_empty(g, pos)) {
        return false;
    }
    for (int i = 0; i < directions_count; i++) {
        Direction dir = directions[i];
        if (legal_dir(g, p, pos, dir)) {
            return true;
        }
    }
    return false;
}

// Reverses stones starting at pos in direction dir, but only if the
// direction is legal.
void reverse_dir(Game* g, Player* p, Position pos, Direction dir) {
    if (legal_dir(g, p, pos, dir)) {
        add_dir(&pos, dir);
        while (in_bounds(pos) && stone_at(g, pos) == opponent_stone(p)) {
            set_stone(g, pos, player_stone(p));
            add_dir(&pos, dir);
        }
    }
}

// Reverses the stones in all legal directions starting at (x,y),
// but only if the move is legal.
void reverse(Game* g, Player* p, Position pos) {
    if (legal_move(g, p, pos)) {
        set_stone(g, pos, player_stone(p));
        for (int i = 0; i < directions_count; i++) {
            Direction dir = directions[i];
            reverse_dir(g, p, pos, dir);
        }
    }
}

// Evaluates the cell at position. If the cell is empty, its value is 0. If it
// is the player's stone, then its value is v. If it is the opponents stone, its
// value is -v.
int eval_cell(Game* g, Player* p, Position pos, int v) {
    char c = stone_at(g, pos);
    if (c == player_stone(p)) return v;
    else if (c == opponent_stone(p)) return -v;
    return 0;
}

// Computes the score of the board from the perspective of given player.
//  |A|B|C|D|E|F|G|H|
// 1|_|_|_|_|_|_|_|_|
// 2|_|_|_|_|_|_|_|_|
// 3|_|_|_|_|_|_|_|_|
// 4|_|_|_|O|X|_|_|_|
// 5|_|_|_|X|O|_|_|_|
// 6|_|_|_|_|_|_|_|_|
// 7|_|_|_|_|_|_|_|_|
// 8|_|_|_|_|_|_|_|_|
int eval_board(Game* g, Player* p) {
    int value = 0;

    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            value += eval_cell(g, p, (Position){x, y}, 1);
        }
    }

    // the middle area has a higher weight: add 9 for each middle cell
    for (int y = 2; y <= 5; y++) {
        for (int x = 2; x < 5; x++) {
            //Position pos = {x, y};
            value += eval_cell(g, p, (Position){x, y}, 9);
        }
    }

    return value;
}

typedef struct {
    Position pos;
    int score;
} Move;

// Stack for storing candidate moves. One of those is chosen randomly.
#define MOVE_STACK_SIZE 64
typedef struct {
    int length; // the number of valid values
    Move values[MOVE_STACK_SIZE]; // valid values are at 0..length-1
} MoveStack;

MoveStack make_move_stack(void) {
    return (MoveStack){.length = 0};
}

void push(MoveStack *s, Move m) {
    require("stack not full", s->length < MOVE_STACK_SIZE);
    s->values[s->length++] = m;
}

Move pop(MoveStack* s) {
    require("stack not empty", s->length > 0);
    return s->values[--s->length];
}

Move random_move(MoveStack *s) {
    require("stack not empty", s->length > 0);
    // todo: check if this works on Windows
    int i = i_rnd(s->length);
    return s->values[i];
}

// Counts the number of cells of the given value.
int count_cells(Game *g, char c) {
    int n = 0;
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            if (stone_at(g, (Position){x, y}) == c) {
                n++;
            }
        }
    }
    return n;
}

// Finds all valid moves and computes their score.
MoveStack valid_moves(Game* g, Player* p) {
    Game g_local = *g; // copy game state (so that changes do not affect the original game state)
    MoveStack s = make_move_stack();
    for (int y = 0; y < N; y++) {
        for (int x = 0; x < N; x++) {
            Position pos = {x, y};
            if (legal_move(g, p, pos)) {
                reverse(&g_local, p, pos); // provisionally make this move
                int score = eval_board(&g_local, p);
                push(&s, (Move){pos, score}); // store the result
                g_local = *g; // undo the move to (x,y)
            }
        }
    }
    return s;
}

// Tests all positions and chooses a random one. Returns false if no valid
// position could be found.
bool best_valid_move(Game* g, Player* p, /*out*/Move* move) {
    // clock_t t = clock(); // get current timestamp
    MoveStack s = valid_moves(g, p);
    if (s.length <= 0) {
        return false;
    }
    if (s.length == 1) {
        *move = s.values[0];
        return true;
    }

    // find a move with the highest score
    int i_max = 0;
    for (int i = 1; i < s.length; i++) {
        if (s.values[i].score > s.values[i_max].score) i_max = i;
    }
    int max_score = s.values[i_max].score;

    // if there are multiple moves with the same max_score, choose randomly among them
    MoveStack best_moves = make_move_stack();
    for (int i = 0; i < s.length; i++) {
        if (s.values[i].score >= max_score) push(&best_moves, s.values[i]);
    }
    assert("at least one", best_moves.length >= 1);
  
    *move = random_move(&best_moves);

    // t = clock() - t; // compute elapsed time
    // double duration = t * 1000.0 / CLOCKS_PER_SEC;
    // fprintf(stderr, "duration: %.3f ms\n", duration);

    return true;
}

///////////////////////////////////////////////////////////////////////////////

void play(void) {
    Game g = init_game();
    Player p, o; // this player and opponent
    while (true) {
        String s = s_input(100);
        // fprintf(stderr, "s = '%s'\n", s);
        int n = s_length(s);
        if (s_equals(s, "exit")) {
            exit(0);
        } else if (s_starts_with(s, "init: ") && n == 7) { // initialization tells use what stone we have
            g = init_game();
            int i = s_index(s, " ");
            char c = s[i + 1];
            if (c != 'X' && c != 'O') {
                fprintf(stderr, "illegal stone: %c\n", c);
                exit(0);
            }
            p.stone = c;
            // fprintf(stderr, "player stone is: %c\n", player_stone(&p));
            o.stone = opponent_stone(&p);
        } else if (s_starts_with(s, "srand: ") && n >= 8) { // seed random number generator
            int i = s_index(s, " ");
            int seed = atoi(s + i + 1);
            // fprintf(stderr, "random seed: %d\n", seed);
            i = i_rnd(2); // trigger seed
            srand(seed + i); // call seed again
        } else if (s_equals(s, "none")) { // opponent made no move
            // fprintf(stderr, "opponent made no move\n");
            // compute our move
            Move m;
            if (best_valid_move(&g, &p, &m)) {
                reverse(&g, &p, m.pos);
                print_position(m.pos);
            } else {
                printf("none\n"); // no valid move found
            }
        } else if (n == 2) { // regular move of opponent
            Position pos;
            if (!parse_position(s, &pos)) {
                fprintf(stderr, "invalid opponent move: %s\n", s);
                exit(0);
            }
            reverse(&g, &o, pos); // make opponent move
            // print_board(&g);
            // compute our move
            Move m;
            if (best_valid_move(&g, &p, &m)) {
                reverse(&g, &p, m.pos); // make our move
                // print_board(&g);
                print_position(m.pos);
            } else {
                printf("none\n"); // no valid move found
            }
        } else {
            fprintf(stderr, "unknown command: %s\n", s);
            exit(0);
        }
        // print_board(&g);
        fflush(stdout); // need to push the data out of the door
        fflush(stderr);
    }
}

int main(void) {
    play();
    return 0;
}
