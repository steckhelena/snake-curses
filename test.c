#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <curses.h>

#define SNAKE_MAX_LENGTH 12
#define WIN_MSG "You are the WINNER!"
#define WELCOME_MSG "We present you the ultimate SNAKE game!!!"
#define LOST_MSG "Loooooser :-))))"
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define SNAKE_CHAR 'O'
#define FOOD_CHAR 'F'
#define WALL_CHAR '#'

typedef struct SnakePart_ {
    int x;
    int y;
} SnakePart;

typedef struct Food_ {
    int x;
    int y;
} Food;

Food food;
int direction;
SnakePart snake[SNAKE_MAX_LENGTH];
int snakeLength = 1;
int row, col;

// Initialization of ncurses and the game
void initGame(void) {
    srand(time(NULL));
    for (int i=0; i<SNAKE_MAX_LENGTH; i++) {
        snake[i].x = -1;
        snake[i].y = -1;
    }

    initscr();
	if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		return 1;
	}

    getmaxyx(stdscr, row, col);
    noecho();
    cbreak();
    curs_set(0);
	start_color();			/* Start color 			*/
	init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(A_BOLD);
    clear();
    color_set(1, NULL);
}

// Randomly add fodd spot to the level
void addFood() {
    int x,y;
    y = rand()%row;
    x = rand()%col;
    while ((mvinch(y, x) & A_CHARTEXT) != ' ') {
        y = rand()%row;
        x = rand()%col;
    }
    food.y = y;
    food.x = x;
}

// Draw food on a game plan
void drawFood() {
    move(food.y, food.x);
    addch(FOOD_CHAR);
}

void initSnake() {
    snake[1].x = 2;
    snake[1].y = row/2;
    snake[0].x = 3;
    snake[0].y = row/2;
    snakeLength = 2;
    direction = RIGHT;
}

// Draw the the actual position of the snake
void drawSnake() {
    int i=0;
    while (snake[i].x != -1 && snake[i].y != -1) {
        move(snake[i].y, snake[i].x);
        addch(SNAKE_CHAR);
        i++;
    }
}

// Helper method for shifting snake parts
void shiftSnake() {
    int i=0;
    for (int i=snakeLength-1; i>=0; i--) {
        snake[i+1] = snake[i];
    }
}

// Helper method for the snake movement that moves snake head in a desired direction
void addHead() {
     SnakePart origHead = snake[0];
     shiftSnake();
     switch (direction) {
        case UP:
            origHead.y--;
            break;
        case DOWN:
            origHead.y++;
            break;
        case LEFT:
            origHead.x--;
            break;
        case RIGHT:
            origHead.x++;
            break;
    }
    snake[0] = origHead;
}

// Helper method for snake movement that cuts the end of the snake
void removeTail() {
    mvaddch(snake[snakeLength].y, snake[snakeLength].x, ' ');
    snake[snakeLength].x = -1;
    snake[snakeLength].y = -1;
}

// Refresh snake movement
void moveSnake() {
    addHead();
    checkGame();
    mvaddch(snake[0].y, snake[0].x, SNAKE_CHAR);
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        addFood();
    } else {
        removeTail();
    }
}

// Recognize snake control
void setDirection(char c) {
    switch(c) {
        case 'w':
            direction = UP;
            break;
        case 's':
            direction = DOWN;
            break;
        case 'd':
            direction = RIGHT;
            break;
        case 'a':
            direction = LEFT;
            break;
    }
}

// Draw the winner screen
void showWinner() {
    clear();
    mvprintw(row/2, col/2-strlen(WIN_MSG)/2, WIN_MSG);
    refresh();
}

// Draw the welcome screen
void showWelcome() {
    clear();
    mvprintw(row/2, col/2-strlen(WELCOME_MSG)/2, WELCOME_MSG);
    refresh();
}

// Draw the welcome screen
void showYouLoose() {
    clear();
    mvprintw(row/2, col/2-strlen(LOST_MSG)/2, LOST_MSG);
    refresh();
}

// Draw the level walls
void drawLevel() {
    for (int i=0; i<row; i++) {
        mvaddch(i, 0, WALL_CHAR);
        mvaddch(i, col-1, WALL_CHAR);
    }
    for (int i=1; i<col-1; i++) {
        mvaddch(0, i, WALL_CHAR);
        mvaddch(row-1, i, WALL_CHAR);
    }
    for (int i=25; i<55; i++) {
        mvaddch(row/4, i, WALL_CHAR);
        mvaddch((row/4)*3, i, WALL_CHAR);
    }
}

// Check on a game state. Checked after every game step (see main())
void checkGame() {
    if (snakeLength == SNAKE_MAX_LENGTH-1) {
        showWinner();
        timeout(-1);
        getch();
        exit(0);
    }
    if ((mvinch(snake[0].y, snake[0].x) & A_CHARTEXT) == WALL_CHAR) {
        showYouLoose();
        timeout(-1);
        getch();
        exit(0);
    }
}

// Game init function
int main() {
    char c;

    initGame();
    showWelcome();
    getch();
    clear();
    initSnake();
    drawLevel();
    drawSnake();
    addFood();
    refresh();

    // Main game loop
    while (1) {
        timeout(70);
        c = getch();
        setDirection(c);
        clear();
        drawLevel();
        moveSnake();
        checkGame();
        drawSnake();
        drawFood();
        refresh();
    }
}
