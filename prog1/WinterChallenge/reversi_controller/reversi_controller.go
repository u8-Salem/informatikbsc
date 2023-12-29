package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"math/rand"
	"os"
	"os/exec"
	"strings"
	"time"
)

const N = 8

type Game struct {
	board [N][N]byte // the NxN playing board
}

// Initializes the board such that it looks like this:
// .|A|B|C|D|E|F|G|H|
// 1|_|_|_|_|_|_|_|_|
// 2|_|_|_|_|_|_|_|_|
// 3|_|_|_|_|_|_|_|_|
// 4|_|_|_|O|X|_|_|_|
// 5|_|_|_|X|O|_|_|_|
// 6|_|_|_|_|_|_|_|_|
// 7|_|_|_|_|_|_|_|_|
// 8|_|_|_|_|_|_|_|_|
func NewGame() *Game {
	g := &Game{}
	for i := 0; i < N; i++ {
		for j := 0; j < N; j++ {
			g.board[i][j] = '_'
		}
	}
	a := N / 2
	b := a - 1
	g.board[b][b] = 'O'
	g.board[a][b] = 'X'
	g.board[b][a] = 'X'
	g.board[a][a] = 'O'
	return g
}

// Prints the board. The initial board should look like shown above.
func (g *Game) String() string {
	var b strings.Builder
	fmt.Fprintln(&b, " |A|B|C|D|E|F|G|H|")
	for i := 0; i < N; i++ {
		fmt.Fprintf(&b, "%d", i+1)
		for j := 0; j < N; j++ {
			fmt.Fprintf(&b, "|%c", g.board[j][i])
		}
		fmt.Fprintln(&b, "|")
	}
	return b.String()
}

// Shows and pauses the game coording to the command line flags.
func (g *Game) ShowAndPause() {
	if show {
		fmt.Print(g.String())
		if pause >= 0 {
			fmt.Println()
		}
		if pause > 0 {
			time.Sleep(pause)
		} else if pause < 0 {
			var s string
			fmt.Scanln(&s) // wait for return
			if s == "q" {
				os.Exit(1)
			}
		}
	}
}

// Returns the stone at the given position.
func (g *Game) Get(pos Position) byte {
	if !IsInBounds(pos) {
		panic("position out of bounds")
	}
	return g.board[pos.x][pos.y]
}

// Sets the stone at the given position to the given value.
func (g *Game) Set(pos Position, c byte) {
	if !IsInBounds(pos) {
		panic("position out of bounds")
	}
	if c != 'X' && c != 'O' {
		panic("invalid stone type")
	}
	g.board[pos.x][pos.y] = c
}

func (g *Game) IsEmpty(pos Position) bool {
	return g.Get(pos) == '_'
}

// Checks whether position (x,y) is on the board.
func IsInBounds(pos Position) bool {
	return pos.x >= 0 && pos.x < N && pos.y >= 0 && pos.y < N
}

type Direction struct {
	x, y int
}

var directions = []Direction{
	{-1, -1}, {0, -1}, {1, -1},
	{-1, 0}, {1, 0},
	{-1, 1}, {0, 1}, {1, 1}}

// Checks whether, starting at pos, dir is a legal direction for p.
// A direction is legal if at least one stone can be reversed in that direction.
func (g *Game) IsLegalDir(p *Player, pos Position, dir Direction) bool {
	pos.Add(dir)
	for IsInBounds(pos) && g.Get(pos) == p.YourStone() {
		pos.Add(dir)
		if IsInBounds(pos) && g.Get(pos) == p.MyStone() {
			return true
		}
	}
	return false
}

// Checks whether pos is a legal position for player p to place a stone. A
// position is legal if it is on the board, the corresponding cell is empty, and
// there is at least one legal direction starting at that cell.
func (g *Game) IsLegalMove(p *Player, pos Position) bool {
	if !IsInBounds(pos) || !g.IsEmpty(pos) {
		return false
	}
	for _, dir := range directions {
		if g.IsLegalDir(p, pos, dir) {
			return true
		}
	}
	return false
}

// Reverses stones starting at pos in direction dir, but only if the
// direction is legal.
func (g *Game) ConquerDir(p *Player, pos Position, dir Direction) {
	if g.IsLegalDir(p, pos, dir) {
		pos.Add(dir)
		for IsInBounds(pos) && g.Get(pos) == p.YourStone() {
			g.Set(pos, p.MyStone())
			pos.Add(dir)
		}
	}
}

// Reverses the stones in all legal directions starting at pos,
// but only if the move is legal.
func (g *Game) Conquer(p *Player, pos Position) {
	if g.IsLegalMove(p, pos) {
		g.Set(pos, p.MyStone())
		for _, dir := range directions {
			g.ConquerDir(p, pos, dir)
		}
	}
}

// Counts the number of cells of a certain type.
func (g *Game) CountCells(c byte) int {
	n := 0
	for _, row := range g.board {
		for _, cell := range row {
			if cell == c {
				n++
			}
		}
	}
	return n
}

type Position struct {
	x, y int
}

func (p *Position) String() string {
	return fmt.Sprintf("%c%d", p.x+'A', p.y+1)
}

func (p *Position) Add(d Direction) {
	p.x += d.x
	p.y += d.y
}

func ParsePosition(s string) (Position, error) {
	s = strings.TrimSpace(s)
	s = strings.ToUpper(s)
	if len(s) < 2 {
		return Position{}, fmt.Errorf("too short")
	}
	x := int(s[0] - 'A')
	y := int(s[1] - '1')
	pos := Position{x, y}
	if !IsInBounds(pos) {
		return Position{}, fmt.Errorf("out of bounds")
	}
	return pos, nil
}

func (g *Game) GetValidMoves(player *Player) []Position {
	result := make([]Position, 0, N*N)
	for y := 0; y < N; y++ {
		for x := 0; x < N; x++ {
			pos := Position{x, y}
			if g.IsLegalMove(player, pos) {
				result = append(result, pos)
			}
		}
	}
	return result
}

type ReversiController struct {
	playerX Player
	playerO Player
	game    *Game
}

type Player struct {
	name          string         // the name of the executable
	myStone       byte           // either 'X' or 'O'
	cmd           *exec.Cmd      // the process of the player
	stdinWriter   io.Writer      // writing to the player's stdin
	stdoutReader  io.Reader      // reading bytes from the player's stdout
	stdoutScanner *bufio.Scanner // reading text lines from player's stdout
	stderrReader  io.Reader      // reading from the player's stderr
}

func NewReversiController(playerX, playerO string) *ReversiController {
	pX := Player{name: playerX, myStone: 'X'}
	pO := Player{name: playerO, myStone: 'O'}
	rc := &ReversiController{playerX: pX, playerO: pO}
	return rc
}

// Returns the player's stone type.
func (p *Player) MyStone() byte {
	return p.myStone
}

// Returns the other player's stone type.
func (p *Player) YourStone() byte {
	if p.myStone == 'X' {
		return 'O'
	} else {
		return 'X'
	}
}

// Starts the player's process and connects to its stdin, stdout, and stderr.
func (p *Player) Start() (err error) {
	if p.name == "" {
		return fmt.Errorf("name is missing")
	}
	if p.cmd != nil {
		p.cmd.Process.Kill()
	}
	p.cmd = exec.Command(p.name)
	p.cmd.Dir = directory
	p.stdinWriter, err = p.cmd.StdinPipe()
	if err != nil {
		return err
	}
	p.stdoutReader, err = p.cmd.StdoutPipe()
	if err != nil {
		return err
	}
	// reading text from player's stdout
	p.stdoutScanner = bufio.NewScanner(p.stdoutReader)
	p.stderrReader, err = p.cmd.StderrPipe()
	if err != nil {
		return err
	}
	err = p.cmd.Start()
	if err != nil {
		return err
	}
	// show standard error output of the players?
	if stderr {
		go func() {
			var buf []byte = make([]byte, 100)
			for {
				n, err := p.stderrReader.Read(buf)
				if err != nil {
					if err != io.EOF {
						fmt.Fprintf(os.Stderr, "%s(%c).stderr: %v\n", p.name, p.myStone, err.Error())
					}
					return
				}
				s := string(buf[:n])
				s = strings.TrimSpace(s)
				fmt.Fprintf(os.Stderr, "%s(%c).stderr: %v\n", p.name, p.myStone, s)
			}
		}()
	}
	return nil
}

// Sends a message to the player.
func (p *Player) Tell(s string) error {
	_, err := io.WriteString(p.stdinWriter, s+"\n")
	return err
}

func (p *Player) Ask0(s string) (string, error) {
	err := p.Tell(s)
	if err != nil {
		return "", err
	}
	var timer *time.Timer
	if timeout > 0 {
		timer = time.AfterFunc(timeout, func() {
			fmt.Printf("%s (%c) reached the timeout of %v\n", p.name, p.myStone, timeout)
			os.Exit(1)
		})
	}
	// advance to next line from player's stdout (may block on underlying reader)
	s = ""
	if p.stdoutScanner.Scan() {
		s = p.stdoutScanner.Text()
	} else {
		return "", fmt.Errorf("%s (%c) stdout scanning stopped or reached the timeout",
			p.name, p.myStone)
	}
	if timer != nil {
		timer.Stop()
	}
	//fmt.Fprintln(os.Stderr, "response:", n, s)
	return s, nil
}

// Sends a message to the player and gets the response. Exits if a timeout
// occurs while waiting for the response.
func (p *Player) Ask(s string) (string, error) {
	err := p.Tell(s)
	if err != nil {
		return "", err
	}
	start := time.Now()
	sch := make(chan string)
	ech := make(chan error)
	var timer *time.Timer
	go func() {
		// advance to next line from player's stdout (may block on underlying reader)
		ok := p.stdoutScanner.Scan()
		if timer != nil {
			timer.Stop() // https://pkg.go.dev/time#Timer
		}
		if ok {
			sch <- p.stdoutScanner.Text()
		} else {
			ech <- fmt.Errorf("%s (%c) stdout scanning stopped", p.name, p.myStone)
		}
	}()
	if timeout > 0 {
		timer = time.AfterFunc(timeout, func() {
			ech <- fmt.Errorf("%s (%c) reached the timeout of %v", p.name, p.myStone, timeout)
		})
	}
	select {
	case s = <-sch:
		duration := time.Since(start)
		if longmove >= 0 && duration > longmove {
			fmt.Printf("%s (%c): move took %v\n", p.name, p.myStone, duration)
		}
		return s, nil
	case err = <-ech:
		return "", err
	}
}

// Changes and shows the game state according to the player's message.
// Ends the game in case of an illegal message.
func (g *Game) ProcessMessage(player *Player, message string) {
	// fmt.Printf("message from %s: %q\n", player.name, message)
	prefix := fmt.Sprintf("%s (%c)", player.name, player.myStone)
	if message == "none" { // no move, board does not change
		if show {
			fmt.Printf("%s: no move\n\n", prefix)
		}
		// check whether there is a valid move for player
		validMoves := g.GetValidMoves(player)
		if len(validMoves) > 0 {
			vm := validMoves[0].String()
			fmt.Printf("however, %s has valid moves, e.g., %v\n", prefix, vm)
			os.Exit(1)
		}
	} else if len(message) == 2 { // e.g., "d3"
		pos, err := ParsePosition(message)
		if err != nil {
			fmt.Printf("%s: cannot parse move: %s\n", prefix, message)
			os.Exit(1)
		} else {
			if show {
				fmt.Printf("%s: %s\n\n", prefix, pos.String())
			}
			if !g.IsLegalMove(player, pos) {
				fmt.Printf("%s: move to %s is not legal\n", prefix, pos.String())
				os.Exit(1)
			}
			g.Conquer(player, pos) // update game board
		}
	} else {
		fmt.Printf("%s: unknown message: %q\n", prefix, message)
		os.Exit(1)
	}
	g.ShowAndPause()
}

// Plays one game. Returns the number of stones that X and O got, respectively.
func (rc *ReversiController) Play() (int, int, error) {
	rc.game = NewGame()
	err := rc.playerX.Start()
	if err != nil {
		return 0, 0, err
	}
	err = rc.playerO.Start()
	if err != nil {
		return 0, 0, err
	}
	pX := rc.playerX
	pO := rc.playerO
	game := rc.game
	game.ShowAndPause()

	err = pX.Tell("init: X")
	if err != nil {
		return 0, 0, err
	}
	err = pO.Tell("init: O")
	if err != nil {
		return 0, 0, err
	}

	err = pX.Tell(fmt.Sprintf("srand: %d", rand.Int31n(0xffff)))
	if err != nil {
		return 0, 0, err
	}
	err = pO.Tell(fmt.Sprintf("srand: %d", rand.Int31n(0xffff)))
	if err != nil {
		return 0, 0, err
	}

	p := pX
	message, err := p.Ask("none") // ask player X for its first move
	if err != nil {
		return 0, 0, err
	}
	game.ProcessMessage(&p, message)

	prev_message := message
	for {
		// determine whose turn it is
		if p == pX {
			p = pO
		} else {
			p = pX
		}
		// inform player about opponent's move and ask for its move
		message, err = p.Ask(message)
		if err != nil {
			return 0, 0, err
		}
		game.ProcessMessage(&p, message)
		if prev_message == "none" && message == "none" {
			if show {
				fmt.Println("None of the players could make a move. Game over.")
			}
			break
		}
		prev_message = message
	}

	xCount := game.CountCells('X')
	oCount := game.CountCells('O')
	fmt.Printf("%d Xs, %d Os\n\n", xCount, oCount)

	err = pX.Tell("exit")
	if err != nil {
		return 0, 0, err
	}
	err = pO.Tell("exit")
	if err != nil {
		return 0, 0, err
	}

	return xCount, oCount, nil
}

// command line flags
var (
	timeout   time.Duration
	pause     time.Duration
	longmove  time.Duration
	repeat    int
	show      bool
	stderr    bool
	directory string
)

func usage() {
	w := flag.CommandLine.Output()
	fmt.Fprintln(w, "Usage: ./reversi_controller {-flags} <playerX> <playerO>")
	fmt.Fprintln(w, "Example: ./reversi_controller -pause=100ms ./eval_player ./random_player")
	fmt.Fprintln(w, "Flags:")
	flag.PrintDefaults()
	os.Exit(0)
}

func main() {
	rand.Seed(time.Now().UnixNano())

	flag.Usage = usage
	flag.DurationVar(&timeout, "timeout", 0,
		"Defines the maximum duration that a player's move may take (in ms or s, the unit must be given after the value). 0 means no timeout.")
	flag.DurationVar(&longmove, "longmove", -time.Millisecond,
		"Gives a threshold duration above which the time of a move is shown (in ms or s, the unit must be given after the value). If omitted or negative, move durations are not shown.")
	flag.DurationVar(&pause, "pause", -time.Millisecond,
		"Waiting time after each move (in ms or s, the unit must be given after the value). If omitted or negative, waits for <Return> key press after each move. This flag only has an effect if flag \"show\" is true.")
	flag.IntVar(&repeat, "repeat", 1,
		"The number of times the game is played.")
	flag.BoolVar(&show, "show", true,
		"Sets whether every move (true) or only the end result (false) is shown.")
	flag.BoolVar(&stderr, "stderr", false,
		"Sets whether stderr of the players should be printed (true) or not (false).")
	flag.StringVar(&directory, "directory", ".",
		"Sets the directory in which the games are located.")
	flag.Parse()

	players := flag.Args()
	if len(players) != 2 {
		usage()
	}

	rc := NewReversiController(players[0], players[1])
	var xWins, oWins, draws int
	for i := 0; i < repeat; i++ {
		if show {
			fmt.Printf("Game %d\n\n", i+1)
		}
		xCount, oCount, err := rc.Play()
		if err != nil {
			fmt.Println(err.Error())
			os.Exit(1)
		}
		if xCount > oCount {
			xWins++
		} else if oCount > xCount {
			oWins++
		} else {
			draws++
		}
	}
	fmt.Printf("X wins %d game(s), O wins %d game(s), %d draw(s)\n",
		xWins, oWins, draws)
}
