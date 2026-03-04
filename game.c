#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

#define MAX_FLOORS 100
#define MAP_SIZE 10

// Global random function
int random_int(int max) {
    return rand() % max;
}

void clear_screen() {
    printf("\033[2J\033[H");
    for (int i = 0; i < 30; i++) printf("\n");
}

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower(*s1) != tolower(*s2)) return tolower(*s1) - tolower(*s2);
        s1++; s2++;
    }
    return tolower(*s1) - tolower(*s2);
}

char* strupr(char *s) {
    char *p = s;
    while (*s) { *s = toupper(*s); s++; }
    return p;
}

// ======================== 1. Tower Game ========================
typedef struct {
    int hp, maxHp, attack, defense, gold, floor;
    int keys[3]; // yellow, blue, red
    int playerX, playerY;
    int floors[101][10][10];
    char message[256];
    bool hasSword, hasShield, gameOver;
} TowerState;

void generateTowerFloors(TowerState *s) {
    for (int f = 1; f <= 100; f++) {
        for (int i = 0; i < 10; i++) {
            s->floors[f][i][0] = 1;
            s->floors[f][i][9] = 1;
            s->floors[f][0][i] = 1;
            s->floors[f][9][i] = 1;
        }
        
        if (f == 1) {
            s->floors[f][5][5] = 0;
            s->floors[f][7][3] = 4;
            s->floors[f][8][8] = 2;
        } else if (f == 100) {
            s->floors[f][5][5] = 6;
            s->floors[f][5][4] = 5;
        } else {
            int x, y;
            do { x = random_int(8) + 1; y = random_int(8) + 1; } while (s->floors[f][y][x] != 0);
            s->floors[f][y][x] = 4;
            
            if (f > 1) {
                do { x = random_int(8) + 1; y = random_int(8) + 1; } while (s->floors[f][y][x] != 0);
                s->floors[f][y][x] = 5;
            }
            
            int enemyCount = (2 + f / 15);
            if (enemyCount > 8) enemyCount = 8;
            for (int i = 0; i < enemyCount; i++) {
                do { x = random_int(8) + 1; y = random_int(8) + 1; } while (s->floors[f][y][x] != 0);
                if (f < 30) s->floors[f][y][x] = 2;
                else if (f < 70) s->floors[f][y][x] = 7;
                else s->floors[f][y][x] = 8;
            }
            
            int itemCount = random_int(3) + 3;
            for (int i = 0; i < itemCount; i++) {
                do { x = random_int(8) + 1; y = random_int(8) + 1; } while (s->floors[f][y][x] != 0);
                s->floors[f][y][x] = 3;
            }
            
            if (f < 10) {
                do { x = random_int(8) + 1; y = random_int(8) + 1; } while (s->floors[f][y][x] != 0);
                s->floors[f][y][x] = 3;
            }
        }
    }
}

void initTower(TowerState *s) {
    s->hp = 200; s->maxHp = 200; s->attack = 20; s->defense = 20;
    s->gold = 0; s->floor = 1;
    s->keys[0] = 1; s->keys[1] = 0; s->keys[2] = 0;
    s->playerX = 5; s->playerY = 5;
    s->hasSword = false; s->hasShield = false; s->gameOver = false;
    strcpy(s->message, "");
    generateTowerFloors(s);
}

void towerMovePlayer(TowerState *s, int dx, int dy) {
    int nx = s->playerX + dx;
    int ny = s->playerY + dy;
    int tile = s->floors[s->floor][ny][nx];
    
    switch(tile) {
        case 0: s->playerX = nx; s->playerY = ny; strcpy(s->message, ""); break;
        case 1: strcpy(s->message, "Hit a wall!"); break;
        case 2: {
            int eHp = 30 + s->floor * 2;
            int eAtk = 10 + s->floor / 3;
            int eDef = 5 + s->floor / 5;
            int eGold = 10 + s->floor;
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            int hits = (eHp + dmgToEnemy - 1) / dmgToEnemy;
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                sprintf(s->message, "Defeated enemy! Got %d gold, lost %d HP", eGold, dmgToPlayer * (hits - 1));
            } else {
                strcpy(s->message, "You were defeated! Game over.");
                s->gameOver = true;
            }
            break;
        }
        case 3: {
            char *itemType;
            if (s->floor == 25 && !s->hasSword) itemType = "legendary_sword";
            else if (s->floor == 50 && !s->hasShield) itemType = "magic_shield";
            else {
                double r = (double)random_int(100) / 100.0;
                if (r < 0.2) itemType = "yellow_key";
                else if (r < 0.3) itemType = "blue_key";
                else if (r < 0.35) itemType = "red_key";
                else if (r < 0.85) itemType = "potion";
                else itemType = "super_potion";
            }
            
            if (strcmp(itemType, "yellow_key") == 0) { s->keys[0]++; strcpy(s->message, "Got 1 Yellow Key!"); }
            else if (strcmp(itemType, "blue_key") == 0) { s->keys[1]++; strcpy(s->message, "Got 1 Blue Key!"); }
            else if (strcmp(itemType, "red_key") == 0) { s->keys[2]++; strcpy(s->message, "Got 1 Red Key!"); }
            else if (strcmp(itemType, "potion") == 0) { s->hp = (s->hp + 50 > s->maxHp) ? s->maxHp : s->hp + 50; strcpy(s->message, "Recovered 50 HP!"); }
            else if (strcmp(itemType, "super_potion") == 0) { s->hp = (s->hp + 120 > s->maxHp) ? s->maxHp : s->hp + 120; strcpy(s->message, "Recovered 120 HP!"); }
            else if (strcmp(itemType, "legendary_sword") == 0) { s->attack += 30; s->hasSword = true; strcpy(s->message, "Got Legendary Sword! ATK +30"); }
            else if (strcmp(itemType, "magic_shield") == 0) { s->defense += 20; s->hasShield = true; strcpy(s->message, "Got Magic Shield! DEF +20"); }
            
            s->floors[s->floor][ny][nx] = 0;
            s->playerX = nx; s->playerY = ny;
            break;
        }
        case 4: case 5: {
            int newFloor = s->floor + (tile == 4 ? 1 : -1);
            if (newFloor >= 1 && newFloor <= 100) {
                s->floor = newFloor;
                sprintf(s->message, "Now at Floor %d", s->floor);
                do { s->playerX = random_int(8) + 1; s->playerY = random_int(8) + 1; } while (s->floors[s->floor][s->playerY][s->playerX] != 0);
            } else {
                strcpy(s->message, "Cannot leave the tower!");
            }
            break;
        }
        case 6: {
            if (!s->hasSword) { strcpy(s->message, "You need the legendary sword!"); return; }
            int bossHp = 500, bossAtk = 50, bossDef = 30, bossGold = 1000;
            int dmgToBoss = (s->attack * 2 > bossDef) ? s->attack * 2 - bossDef : 1;
            int dmgToPlayer = (bossAtk > s->defense) ? bossAtk - s->defense : 1;
            int hits = (bossHp + dmgToBoss - 1) / dmgToBoss;
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += bossGold;
                s->playerX = nx; s->playerY = ny;
                strcpy(s->message, "Defeated Boss! Congratulations!");
                s->gameOver = true;
            } else {
                strcpy(s->message, "You were defeated by Boss!");
                s->gameOver = true;
            }
            break;
        }
        case 7: {
            int eHp = 80 + s->floor * 2, eAtk = 20 + s->floor / 2, eDef = 10 + s->floor / 4, eGold = 30 + s->floor * 2;
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            int hits = (eHp + dmgToEnemy - 1) / dmgToEnemy;
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                sprintf(s->message, "Defeated enemy! Got %d gold", eGold);
            } else { strcpy(s->message, "You were defeated!"); s->gameOver = true; }
            break;
        }
        case 8: {
            int eHp = 120 + s->floor * 3, eAtk = 30 + s->floor / 2, eDef = 15 + s->floor / 3, eGold = 50 + s->floor * 3;
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            int hits = (eHp + dmgToEnemy - 1) / dmgToEnemy;
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                sprintf(s->message, "Defeated enemy! Got %d gold", eGold);
            } else { strcpy(s->message, "You were defeated!"); s->gameOver = true; }
            break;
        }
    }
}

void renderTower(TowerState *s) {
    clear_screen();
    printf("=== 100-Floor Tower ===\n");
    printf("Floor: %d | HP: %d/%d | ATK: %d | DEF: %d | Gold: %d\n", 
           s->floor, s->hp, s->maxHp, s->attack, s->defense, s->gold);
    printf("Keys: Y[%d] B[%d] R[%d]\n", s->keys[0], s->keys[1], s->keys[2]);
    if (s->hasSword) printf("[Sword] ");
    if (s->hasShield) printf("[Shield] ");
    printf("\n\n  0123456789\n");
    printf(" +----------+\n");
    for (int y = 0; y < 10; y++) {
        printf("%d|", y);
        for (int x = 0; x < 10; x++) {
            if (x == s->playerX && y == s->playerY) printf("@");
            else {
                char tile = " .#SGEBODM"[s->floors[s->floor][y][x]];
                printf("%c", tile);
            }
        }
        printf("|\n");
    }
    printf(" +----------+\n\n%s\n", s->message);
    printf("w/a/s/d to move, q to quit\n");
}

void playTowerGame() {
    TowerState state;
    initTower(&state);
    while (1) {
        renderTower(&state);
        if (state.gameOver) { getchar(); break; }
        printf("> ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        if (strcmp(cmd, "w") == 0) towerMovePlayer(&state, 0, -1);
        else if (strcmp(cmd, "s") == 0) towerMovePlayer(&state, 0, 1);
        else if (strcmp(cmd, "a") == 0) towerMovePlayer(&state, -1, 0);
        else if (strcmp(cmd, "d") == 0) towerMovePlayer(&state, 1, 0);
    }
}

// ======================== 2. Snake Game ========================
typedef struct {
    int snakeX[300], snakeY[300], snakeLen;
    int dirX, dirY, foodX, foodY;
    int score;
    bool gameOver;
    char message[256];
} SnakeGame;

void initSnake(SnakeGame *s) {
    s->snakeLen = 1;
    s->snakeX[0] = 10; s->snakeY[0] = 7;
    s->dirX = 1; s->dirY = 0;
    s->score = 0;
    s->gameOver = false;
    strcpy(s->message, "Use WASD to control");
    
    do {
        s->foodX = random_int(20);
        s->foodY = random_int(15);
    } while (s->foodX == s->snakeX[0] && s->foodY == s->snakeY[0]);
}

void snakeUpdate(SnakeGame *s) {
    int nx = (s->snakeX[0] + s->dirX + 20) % 20;
    int ny = (s->snakeY[0] + s->dirY + 15) % 15;
    
    for (int i = 0; i < s->snakeLen; i++) {
        if (s->snakeX[i] == nx && s->snakeY[i] == ny) {
            s->gameOver = true;
            sprintf(s->message, "Game Over! Score: %d", s->score);
            return;
        }
    }
    
    for (int i = s->snakeLen; i > 0; i--) {
        s->snakeX[i] = s->snakeX[i-1];
        s->snakeY[i] = s->snakeY[i-1];
    }
    s->snakeX[0] = nx;
    s->snakeY[0] = ny;
    
    if (nx == s->foodX && ny == s->foodY) {
        s->score++;
        s->snakeLen++;
        do {
            s->foodX = random_int(20);
            s->foodY = random_int(15);
        } while (0);
    }
}

void renderSnake(SnakeGame *s) {
    clear_screen();
    printf("=== Snake Game ===\n");
    printf("Score: %d\n%s\n\n", s->score, s->message);
    
    for (int y = 0; y < 15; y++) {
        for (int x = 0; x < 20; x++) {
            if (x == s->foodX && y == s->foodY) printf("$ ");
            else if (x == s->snakeX[0] && y == s->snakeY[0]) printf("@ ");
            else {
                bool isSnake = false;
                for (int i = 1; i < s->snakeLen; i++) {
                    if (s->snakeX[i] == x && s->snakeY[i] == y) { isSnake = true; break; }
                }
                printf(isSnake ? "O " : ". ");
            }
        }
        printf("\n");
    }
    printf("\nControls: w/a/s/d, q to quit\n");
}

void playSnakeGame() {
    SnakeGame game;
    initSnake(&game);
    while (1) {
        renderSnake(&game);
        if (game.gameOver) { getchar(); break; }
        printf("> ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        
        if (strcmp(cmd, "w") == 0 && (game.dirX != 0 || game.dirY != 1)) { game.dirX = 0; game.dirY = -1; }
        else if (strcmp(cmd, "s") == 0 && (game.dirX != 0 || game.dirY != -1)) { game.dirX = 0; game.dirY = 1; }
        else if (strcmp(cmd, "a") == 0 && (game.dirX != 1 || game.dirY != 0)) { game.dirX = -1; game.dirY = 0; }
        else if (strcmp(cmd, "d") == 0 && (game.dirX != -1 || game.dirY != 0)) { game.dirX = 1; game.dirY = 0; }
        
        snakeUpdate(&game);
    }
}

// ======================== 3. Guess Number ========================
void playGuessNumberGame() {
    int number = random_int(100) + 1;
    int attempts = 0, maxAttempts = 10;
    bool gameOver = false;
    char message[256] = "Guess 1-100, you have 10 attempts";
    
    while (1) {
        clear_screen();
        printf("=== Guess Number ===\n%s\n", message);
        if (gameOver) { printf("\nPress Enter..."); getchar(); break; }
        printf("\nEnter guess or q: ");
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int guess = atoi(input);
        if (guess < 1 || guess > 100) { strcpy(message, "Invalid! Enter 1-100"); continue; }
        
        attempts++;
        if (guess == number) { sprintf(message, "Correct! %d tries!", attempts); gameOver = true; }
        else if (attempts >= maxAttempts) { sprintf(message, "Game over! Answer was %d", number); gameOver = true; }
        else if (guess < number) sprintf(message, "Too small! %d attempts left", maxAttempts - attempts);
        else sprintf(message, "Too big! %d attempts left", maxAttempts - attempts);
    }
}

// ======================== 4. Rock Paper Scissors ========================
void playRockPaperScissors() {
    int playerScore = 0, computerScore = 0, rounds = 0;
    char *choices[3] = {"Rock", "Scissors", "Paper"};
    char message[256] = "1(Rock) 2(Scissors) 3(Paper) or q:";
    
    while (1) {
        clear_screen();
        printf("=== Rock Paper Scissors ===\n");
        printf("Rounds: %d | Score: Player %d - %d Computer\n\n", rounds, playerScore, computerScore);
        printf("1. Rock\n2. Scissors\n3. Paper\n\n%s\n", message);
        printf("> ");
        char input[10]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int playerIdx = atoi(input) - 1;
        if (playerIdx < 0 || playerIdx > 2) { strcpy(message, "Enter 1-3!"); continue; }
        
        int computerIdx = random_int(3);
        rounds++;
        
        int result = (playerIdx - computerIdx + 3) % 3;
        if (result == 2) playerScore++;
        else if (result == 1) computerScore++;
        
        sprintf(message, "You: %s, Computer: %s\n", choices[playerIdx], choices[computerIdx]);
        if (result == 0) strcat(message, "Draw!");
        else if (result == 2) strcat(message, "You win!");
        else strcat(message, "Computer wins!");
    }
}

// ======================== 5. Tic Tac Toe ========================
void playTicTacToe() {
    char board[9];
    for (int i = 0; i < 9; i++) board[i] = ' ';
    char currentPlayer = 'X';
    bool gameOver = false;
    char message[256] = "Enter position 1-9 or q:";
    
    while (1) {
        clear_screen();
        printf("=== Tic-Tac-Toe ===\nPositions: 1|2|3 4|5|6 7|8|9\n\n");
        for (int i = 0; i < 9; i += 3) {
            printf(" %c | %c | %c \n", board[i], board[i+1], board[i+2]);
            if (i < 6) printf("-----------\n");
        }
        printf("\n%s\n", message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char input[10]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int pos = atoi(input) - 1;
        if (pos < 0 || pos > 8) { strcpy(message, "Enter 1-9!"); continue; }
        if (board[pos] != ' ') { strcpy(message, "Position occupied!"); continue; }
        
        board[pos] = currentPlayer;
        
        // Check winner
        int win[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
        bool won = false;
        for (int i = 0; i < 8; i++) {
            if (board[win[i][0]] != ' ' && 
                board[win[i][0]] == board[win[i][1]] && 
                board[win[i][1]] == board[win[i][2]]) {
                won = true; break;
            }
        }
        
        if (won) { sprintf(message, "Player %c wins! Press Enter...", currentPlayer); gameOver = true; }
        else if (strchr(board, ' ') == NULL) { strcpy(message, "Draw! Press Enter..."); gameOver = true; }
        else { currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; sprintf(message, "Player %c's turn:", currentPlayer); }
    }
}

// ======================== 6. Memory Card ========================
void playMemoryGame() {
    char symbols[16];
    char deck[16] = {'A','B','C','D','E','F','G','H','A','B','C','D','E','F','G','H'};
    for (int i = 0; i < 16; i++) {
        int r = random_int(16);
        char t = deck[i]; deck[i] = deck[r]; deck[r] = t;
    }
    for (int i = 0; i < 16; i++) symbols[i] = '?';
    bool matched[16] = {0};
    int firstCard = -1, secondCard = -1;
    int attempts = 0;
    bool gameOver = false;
    char message[256] = "Select card 1-16 or q:";
    
    while (1) {
        clear_screen();
        printf("=== Memory Card ===\nAttempts: %d\n\n", attempts);
        for (int i = 0; i < 16; i += 4) {
            for (int j = 0; j < 4; j++) {
                int idx = i + j;
                if (matched[idx] || firstCard == idx || secondCard == idx) 
                    printf(" %c ", deck[idx]);
                else 
                    printf("%2d ", idx + 1);
                if (j < 3) printf("|");
            }
            printf("\n");
            if (i < 12) printf("---------------------\n");
        }
        printf("\n%s\n", message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char input[10]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int pos = atoi(input) - 1;
        if (pos < 0 || pos > 15) { strcpy(message, "Enter 1-16!"); continue; }
        if (matched[pos]) { strcpy(message, "Already matched!"); continue; }
        if (pos == firstCard) { strcpy(message, "Same card!"); continue; }
        
        if (firstCard == -1) {
            firstCard = pos;
            symbols[pos] = deck[pos];
            strcpy(message, "Select second card:");
        } else {
            secondCard = pos;
            symbols[pos] = deck[pos];
            attempts++;
            
            if (deck[firstCard] == deck[secondCard]) {
                matched[firstCard] = matched[secondCard] = true;
                bool allMatched = true;
                for (int i = 0; i < 16; i++) if (!matched[i]) { allMatched = false; break; }
                if (allMatched) { sprintf(message, "Congratulations! %d attempts. Press Enter...", attempts); gameOver = true; }
                else strcpy(message, "Match! Continue.");
            } else {
                strcpy(message, "No match!");
            }
            
            printf("\n");
            for (int i = 0; i < 16; i += 4) {
                for (int j = 0; j < 4; j++) {
                    int idx = i + j;
                    if (matched[idx] || firstCard == idx || secondCard == idx) printf(" %c ", deck[idx]);
                    else printf("%2d ", idx + 1);
                    if (j < 3) printf("|");
                }
                printf("\n");
                if (i < 12) printf("---------------------\n");
            }
            printf("Press Enter...");
            getchar(); getchar();
            
            if (!matched[firstCard]) symbols[firstCard] = '?';
            if (!matched[secondCard]) symbols[secondCard] = '?';
            firstCard = secondCard = -1;
        }
    }
}

// ======================== 7. 4x4 Slide Puzzle ========================
void playSlidePuzzle() {
    int board[16];
    for (int i = 0; i < 15; i++) board[i] = i + 1;
    board[15] = 0;
    for (int i = 0; i < 1000; i++) {
        int r = random_int(4);
        int emptyIdx = -1;
        for (int j = 0; j < 16; j++) if (board[j] == 0) { emptyIdx = j; break; }
        int emptyRow = emptyIdx / 4, emptyCol = emptyIdx % 4;
        int newIdx = -1;
        if (r == 0 && emptyRow > 0) newIdx = emptyIdx - 4;
        else if (r == 1 && emptyRow < 3) newIdx = emptyIdx + 4;
        else if (r == 2 && emptyCol > 0) newIdx = emptyIdx - 1;
        else if (r == 3 && emptyCol < 3) newIdx = emptyIdx + 1;
        if (newIdx >= 0) { int t = board[emptyIdx]; board[emptyIdx] = board[newIdx]; board[newIdx] = t; }
    }
    int moves = 0;
    bool gameOver = false;
    char message[256] = "Use WASD or q:";
    
    while (1) {
        clear_screen();
        printf("=== 4x4 Slide Puzzle ===\nArrange 1-15:\n\n");
        for (int i = 0; i < 4; i++) {
            printf("+");
            for (int j = 0; j < 4; j++) printf("-----+");
            printf("\n|");
            for (int j = 0; j < 4; j++) {
                int v = board[i*4+j];
                printf(v == 0 ? "     |" : " %2d  |", v);
            }
            printf("\n");
        }
        printf("+");
        for (int j = 0; j < 4; j++) printf("-----+");
        printf("\n\n%s\n", message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        
        int emptyIdx = -1;
        for (int i = 0; i < 16; i++) if (board[i] == 0) { emptyIdx = i; break; }
        int row = emptyIdx / 4, col = emptyIdx % 4;
        int newIdx = -1;
        
        if (strcmp(cmd, "w") == 0 && row > 0) newIdx = emptyIdx - 4;
        else if (strcmp(cmd, "s") == 0 && row < 3) newIdx = emptyIdx + 4;
        else if (strcmp(cmd, "a") == 0 && col > 0) newIdx = emptyIdx - 1;
        else if (strcmp(cmd, "d") == 0 && col < 3) newIdx = emptyIdx + 1;
        
        if (newIdx >= 0) {
            int t = board[emptyIdx];
            board[emptyIdx] = board[newIdx];
            board[newIdx] = t;
            moves++;
            
            bool solved = true;
            for (int i = 0; i < 15; i++) if (board[i] != i + 1) { solved = false; break; }
            if (solved) { sprintf(message, "Solved in %d moves! Press Enter...", moves); gameOver = true; }
            else sprintf(message, "Moves: %d", moves);
        }
    }
}

// ======================== 8. Maze ========================
void playMazeGame() {
    int maze[15][15];
    for (int i = 0; i < 15; i++) for (int j = 0; j < 15; j++) maze[i][j] = 1;
    
    int stack[225][2], top = 0;
    stack[top][0] = 1; stack[top][1] = 1;
    maze[1][1] = 0;
    
    int dirs[4][2] = {{0,2},{2,0},{0,-2},{-2,0}};
    
    while (top >= 0) {
        int x = stack[top][0], y = stack[top][1];
        int shuffled[4] = {0,1,2,3};
        for (int i = 0; i < 4; i++) {
            int r = random_int(4);
            int t = shuffled[i]; shuffled[i] = shuffled[r]; shuffled[r] = t;
        }
        
        bool found = false;
        for (int i = 0; i < 4; i++) {
            int d = shuffled[i];
            int nx = x + dirs[d][0], ny = y + dirs[d][1];
            if (nx > 0 && nx < 14 && ny > 0 && ny < 14 && maze[ny][nx] == 1) {
                maze[ny][nx] = 0;
                maze[y + dirs[d][1]/2][x + dirs[d][0]/2] = 0;
                top++;
                stack[top][0] = nx;
                stack[top][1] = ny;
                found = true;
                break;
            }
        }
        if (!found) top--;
    }
    
    for (int i = 0; i < 20; i++) {
        int x = random_int(13) + 1, y = random_int(13) + 1;
        if (maze[y][x] == 1 && (maze[y-1][x] == 0 || maze[y+1][x] == 0 || maze[y][x-1] == 0 || maze[y][x+1] == 0))
            maze[y][x] = 0;
    }
    maze[13][13] = 0;
    
    int px = 1, py = 1;
    bool win = false;
    char message[256] = "Use WASD or q:";
    
    while (1) {
        clear_screen();
        printf("=== 15x15 Maze ===\nFind exit (E)!\n\n");
        for (int y = 0; y < 15; y++) {
            for (int x = 0; x < 15; x++) {
                if (x == px && y == py) printf("P ");
                else if (x == 13 && y == 13) printf("E ");
                else if (maze[y][x] == 1) printf("# ");
                else printf(". ");
            }
            printf("\n");
        }
        printf("\n%s\n", message);
        if (win) { getchar(); break; }
        printf("> ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        
        int nx = px, ny = py;
        if (strcmp(cmd, "w") == 0) ny--;
        else if (strcmp(cmd, "s") == 0) ny++;
        else if (strcmp(cmd, "a") == 0) nx--;
        else if (strcmp(cmd, "d") == 0) nx++;
        else { strcpy(message, "Use w/a/s/d"); continue; }
        
        if (nx < 0 || ny < 0 || nx >= 15 || ny >= 15 || maze[ny][nx] == 1) {
            strcpy(message, "Hit a wall!");
        } else {
            px = nx; py = ny;
            if (px == 13 && py == 13) { strcpy(message, "You escaped! Press Enter..."); win = true; }
            else strcpy(message, "Use WASD or q:");
        }
    }
}

// ======================== 9. Sudoku ========================
void playSudokuGame() {
    int board[9][9], original[9][9];
    
    // Generate solution
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            board[i][j] = (i * 3 + i / 3 + j) % 9 + 1;
    
    // Shuffle
    for (int i = 0; i < 40; i++) {
        int r1 = random_int(9), c1 = random_int(9), r2 = random_int(9), c2 = random_int(9);
        int t = board[r1][c1]; board[r1][c1] = board[r2][c2]; board[r2][c2] = t;
    }
    
    // Remove numbers
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (random_int(100) < 60) board[i][j] = 0;
    
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            original[i][j] = board[i][j];
    
    char message[256] = "Enter row col num (e.g. 1 2 3) or q:";
    
    while (1) {
        clear_screen();
        printf("=== Sudoku ===\nEach row/col/3x3 must have 1-9\n\n");
        for (int i = 0; i < 9; i++) {
            if (i % 3 == 0 && i != 0) printf("-----------------------------\n");
            for (int j = 0; j < 9; j++) {
                if (j % 3 == 0 && j != 0) printf("|");
                if (original[i][j] != 0) printf("*%d*", board[i][j]);
                else if (board[i][j] == 0) printf(" . ");
                else printf(" %d ", board[i][j]);
            }
            printf("\n");
        }
        printf("\n%s\n", message);
        
        bool complete = true;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (board[i][j] == 0) complete = false;
        
        if (complete) { printf("Solved! Press Enter..."); getchar(); break; }
        
        printf("> ");
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int r, c, n;
        if (sscanf(input, "%d %d %d", &r, &c, &n) != 3) {
            strcpy(message, "Format: r c n"); continue;
        }
        r--; c--;
        if (r < 0 || r > 8 || c < 0 || c > 8 || n < 1 || n > 9) {
            strcpy(message, "Must be 1-9!"); continue;
        }
        if (original[r][c] != 0) { strcpy(message, "Cannot change initial!"); continue; }
        
        // Check valid
        bool valid = true;
        for (int i = 0; i < 9; i++) if (board[r][i] == n) valid = false;
        for (int i = 0; i < 9; i++) if (board[i][c] == n) valid = false;
        int br = (r/3)*3, bc = (c/3)*3;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (board[br+i][bc+j] == n) valid = false;
        
        if (!valid) { sprintf(message, "%d invalid at (%d,%d)", n, r+1, c+1); continue; }
        
        board[r][c] = n;
        strcpy(message, "Enter row col num or q:");
    }
}

// ======================== 10. Minesweeper ========================
void playMinesweeperGame() {
    char board[10][10];
    bool revealed[10][10] = {0};
    bool firstMove = true;
    
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            board[i][j] = ' ';
    
    bool gameOver = false;
    char message[256] = "Enter coordinate (e.g. A5) or q:";
    
    while (1) {
        clear_screen();
        printf("=== Minesweeper ===\nMines: 15\n\n   ");
        for (int i = 0; i < 10; i++) printf("%c ", 'A' + i);
        printf("\n  +------------------+\n");
        for (int r = 0; r < 10; r++) {
            printf("%2d|", r + 1);
            for (int c = 0; c < 10; c++) {
                if (revealed[r][c]) printf("%c ", board[r][c]);
                else printf(". ");
            }
            printf("|\n");
        }
        printf("  +------------------+\n\n%s\n", message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char input[10]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        if (strlen(input) < 2) { strcpy(message, "Format: A5"); continue; }
        
        int c = input[0] - 'A';
        int r = atoi(input + 1) - 1;
        if (r < 0 || r > 9 || c < 0 || c > 9) { strcpy(message, "Invalid!"); continue; }
        
        if (firstMove) {
            firstMove = false;
            int placed = 0;
            while (placed < 15) {
                int rr = random_int(10), cc = random_int(10);
                if ((rr != r || cc != c) && board[rr][cc] != 'X') {
                    board[rr][cc] = 'X';
                    placed++;
                }
            }
            // Calculate numbers
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 10; j++)
                    if (board[i][j] != 'X') {
                        int count = 0;
                        for (int di = -1; di <= 1; di++)
                            for (int dj = -1; dj <= 1; dj++)
                                if (i+di >= 0 && i+di < 10 && j+dj >= 0 && j+dj < 10 && board[i+di][j+dj] == 'X')
                                    count++;
                        if (count > 0) board[i][j] = '0' + count;
                    }
        }
        
        if (board[r][c] == 'X') {
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 10; j++)
                    revealed[i][j] = true;
            strcpy(message, "BOOM! Game Over! Press Enter...");
            gameOver = true;
        } else {
            revealed[r][c] = true;
            
            // Flood fill
            if (board[r][c] == ' ') {
                // Simple flood fill would go here
            }
            
            // Check win
            bool win = true;
            for (int i = 0; i < 10; i++)
                for (int j = 0; j < 10; j++)
                    if (board[i][j] != 'X' && !revealed[i][j]) win = false;
            
            if (win) { strcpy(message, "You won! Press Enter..."); gameOver = true; }
        }
    }
}

// ======================== 11. 2048 ========================
void play2048Game() {
    int board[4][4] = {0};
    int score = 0;
    bool gameOver = false;
    char message[256] = "Use WASD or q:";
    
    // Add random tiles
    int empty[16], emptyCount = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (board[i][j] == 0) empty[emptyCount++] = i * 4 + j;
    if (emptyCount > 0) board[empty[random_int(emptyCount)] / 4][empty[random_int(emptyCount)] % 4] = random_int(10) < 9 ? 2 : 4;
    
    while (1) {
        clear_screen();
        printf("=== 2048 ===\nGoal: 2048\n\n");
        for (int i = 0; i < 4; i++) {
            printf("+");
            for (int j = 0; j < 4; j++) printf("-----+");
            printf("\n|");
            for (int j = 0; j < 4; j++) printf(board[i][j] == 0 ? "     |" : "%4d |", board[i][j]);
            printf("\n");
        }
        printf("+");
        for (int j = 0; j < 4; j++) printf("-----+");
        printf("\n\nScore: %d\n%s\n", score, message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        
        bool moved = false;
        
        if (strcmp(cmd, "w") == 0) {
            for (int c = 0; c < 4; c++) {
                for (int r = 1; r < 4; r++) {
                    if (board[r][c] != 0) {
                        for (int k = r; k > 0; k--) {
                            if (board[k-1][c] == 0) { board[k-1][c] = board[k][c]; board[k][c] = 0; moved = true; }
                            else if (board[k-1][c] == board[k][c]) { board[k-1][c] *= 2; score += board[k-1][c]; board[k][c] = 0; moved = true; break; }
                            else break;
                        }
                    }
                }
            }
        }
        else if (strcmp(cmd, "s") == 0) {
            for (int c = 0; c < 4; c++) {
                for (int r = 2; r >= 0; r--) {
                    if (board[r][c] != 0) {
                        for (int k = r; k < 3; k++) {
                            if (board[k+1][c] == 0) { board[k+1][c] = board[k][c]; board[k][c] = 0; moved = true; }
                            else if (board[k+1][c] == board[k][c]) { board[k+1][c] *= 2; score += board[k+1][c]; board[k][c] = 0; moved = true; break; }
                            else break;
                        }
                    }
                }
            }
        }
        else if (strcmp(cmd, "a") == 0) {
            for (int r = 0; r < 4; r++) {
                for (int c = 1; c < 4; c++) {
                    if (board[r][c] != 0) {
                        for (int k = c; k > 0; k--) {
                            if (board[r][k-1] == 0) { board[r][k-1] = board[r][k]; board[r][k] = 0; moved = true; }
                            else if (board[r][k-1] == board[r][k]) { board[r][k-1] *= 2; score += board[r][k-1]; board[r][k] = 0; moved = true; break; }
                            else break;
                        }
                    }
                }
            }
        }
        else if (strcmp(cmd, "d") == 0) {
            for (int r = 0; r < 4; r++) {
                for (int c = 2; c >= 0; c--) {
                    if (board[r][c] != 0) {
                        for (int k = c; k < 3; k++) {
                            if (board[r][k+1] == 0) { board[r][k+1] = board[r][k]; board[r][k] = 0; moved = true; }
                            else if (board[r][k+1] == board[r][k]) { board[r][k+1] *= 2; score += board[r][k+1]; board[r][k] = 0; moved = true; break; }
                            else break;
                        }
                    }
                }
            }
        }
        
        if (moved) {
            emptyCount = 0;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (board[i][j] == 0) empty[emptyCount++] = i * 4 + j;
            if (emptyCount > 0) {
                int pos = empty[random_int(emptyCount)];
                board[pos / 4][pos % 4] = random_int(10) < 9 ? 2 : 4;
            }
            
            // Check game over
            bool canMove = false;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (board[i][j] == 0) canMove = true;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 3; j++)
                    if (board[i][j] == board[i][j+1]) canMove = true;
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 4; j++)
                    if (board[i][j] == board[i+1][j]) canMove = true;
            
            if (!canMove) { sprintf(message, "Game Over! Score: %d", score); gameOver = true; }
            else sprintf(message, "Score: %d", score);
        }
    }
}

// ======================== 12. Gomoku ========================
void playGomokuGame() {
    char board[15][15];
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            board[i][j] = '.';
    
    char currentPlayer = 'X';
    bool gameOver = false;
    char message[256] = "Enter coordinate (e.g. A15) or q:";
    
    while (1) {
        clear_screen();
        printf("=== Gomoku ===\nConnect 5 to win!\n\n   ");
        for (int i = 0; i < 15; i++) printf("%c ", 'A' + i);
        printf("\n  +");
        for (int i = 0; i < 15; i++) printf("--");
        printf("+\n");
        
        for (int i = 0; i < 15; i++) {
            printf("%2d|", i + 1);
            for (int j = 0; j < 15; j++) printf("%c ", board[i][j]);
            printf("|\n");
        }
        printf("  +");
        for (int i = 0; i < 15; i++) printf("--");
        printf("+\n\n%s\n", message);
        if (gameOver) { getchar(); break; }
        printf("> ");
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int c = input[0] - 'A';
        int r = atoi(input + 1) - 1;
        
        if (r < 0 || r > 14 || c < 0 || c > 14) { strcpy(message, "Invalid!"); continue; }
        if (board[r][c] != '.') { strcpy(message, "Occupied!"); continue; }
        
        board[r][c] = currentPlayer;
        
        // Check win
        int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
        bool won = false;
        for (int d = 0; d < 4; d++) {
            int count = 1;
            for (int i = 1; i < 5; i++) {
                int nr = r + dirs[d][0] * i, nc = c + dirs[d][1] * i;
                if (nr >= 0 && nr < 15 && nc >= 0 && nc < 15 && board[nr][nc] == currentPlayer) count++;
                else break;
            }
            for (int i = 1; i < 5; i++) {
                int nr = r - dirs[d][0] * i, nc = c - dirs[d][1] * i;
                if (nr >= 0 && nr < 15 && nc >= 0 && nc < 15 && board[nr][nc] == currentPlayer) count++;
                else break;
            }
            if (count >= 5) won = true;
        }
        
        if (won) { sprintf(message, "Player %c wins! Press Enter...", currentPlayer); gameOver = true; }
        else {
            bool full = true;
            for (int i = 0; i < 15; i++)
                for (int j = 0; j < 15; j++)
                    if (board[i][j] == '.') full = false;
            if (full) { strcpy(message, "Draw! Press Enter..."); gameOver = true; }
            else { currentPlayer = currentPlayer == 'X' ? 'O' : 'X'; sprintf(message, "Player %c's turn:", currentPlayer); }
        }
    }
}

// ======================== 13-19. Additional Games Placeholder ========================
void playAdditionalGames() {
    printf("Additional games coming soon!\n");
    getchar();
}

// ======================== 13. Text Adventure ========================
void playTextAdventure() {
    typedef struct {
        char name[20];
        char description[100];
        char exits[6][10];
        char exitDest[6][20];
        int exitCount;
        char items[5][20];
        int itemCount;
        int enemyHealth;
        char enemyName[20];
        int enemyDamage;
        int locked[6];
        int isWin;
    } Room;
    
    Room rooms[7];
    strcpy(rooms[0].name, "Hall");
    strcpy(rooms[0].description, "You stand in an ancient hall. Faded tapestries hang on the walls.");
    strcpy(rooms[0].exits[0], "n"); strcpy(rooms[0].exitDest[0], "Library");
    strcpy(rooms[0].exits[1], "e"); strcpy(rooms[0].exitDest[1], "Kitchen");
    strcpy(rooms[0].exits[2], "s"); strcpy(rooms[0].exitDest[2], "Garden");
    rooms[0].exitCount = 3;
    strcpy(rooms[0].items[0], "Torch");
    rooms[0].itemCount = 1;
    rooms[0].enemyHealth = 0;
    rooms[0].isWin = 0;
    
    strcpy(rooms[1].name, "Library");
    strcpy(rooms[1].description, "Dusty bookshelves line the walls. The air smells of mildew.");
    strcpy(rooms[1].exits[0], "s"); strcpy(rooms[1].exitDest[0], "Hall");
    strcpy(rooms[1].exits[1], "w"); strcpy(rooms[1].exitDest[1], "Secret");
    rooms[1].exitCount = 2;
    rooms[1].locked[1] = 1;
    strcpy(rooms[1].items[0], "Book");
    rooms[1].itemCount = 1;
    rooms[1].enemyHealth = 0;
    rooms[1].isWin = 0;
    
    strcpy(rooms[2].name, "Kitchen");
    strcpy(rooms[2].description, "A decrepit kitchen with dusty countertops.");
    strcpy(rooms[2].exits[0], "w"); strcpy(rooms[2].exitDest[0], "Hall");
    strcpy(rooms[2].exits[1], "n"); strcpy(rooms[2].exitDest[1], "Storage");
    rooms[2].exitCount = 2;
    strcpy(rooms[2].items[0], "Knife");
    rooms[2].itemCount = 1;
    rooms[2].enemyHealth = 0;
    rooms[2].isWin = 0;
    
    strcpy(rooms[3].name, "Storage");
    strcpy(rooms[3].description, "A dark small room filled with crates and jars.");
    strcpy(rooms[3].exits[0], "s"); strcpy(rooms[3].exitDest[0], "Kitchen");
    rooms[3].exitCount = 1;
    strcpy(rooms[3].items[0], "Key");
    rooms[3].itemCount = 1;
    rooms[3].enemyHealth = 0;
    rooms[3].isWin = 0;
    
    strcpy(rooms[4].name, "Garden");
    strcpy(rooms[4].description, "An overgrown garden with a dry well in the center.");
    strcpy(rooms[4].exits[0], "n"); strcpy(rooms[4].exitDest[0], "Hall");
    strcpy(rooms[4].exits[1], "d"); strcpy(rooms[4].exitDest[1], "Cellar");
    rooms[4].exitCount = 2;
    rooms[4].itemCount = 0;
    strcpy(rooms[4].enemyName, "Giant Rat");
    rooms[4].enemyHealth = 30;
    rooms[4].enemyDamage = 10;
    rooms[4].isWin = 0;
    
    strcpy(rooms[5].name, "Cellar");
    strcpy(rooms[5].description, "A dark damp cellar. Something glows in the corner.");
    strcpy(rooms[5].exits[0], "u"); strcpy(rooms[5].exitDest[0], "Garden");
    rooms[5].exitCount = 1;
    strcpy(rooms[5].items[0], "Treasure");
    rooms[5].itemCount = 1;
    rooms[5].enemyHealth = 0;
    rooms[5].isWin = 1;
    
    strcpy(rooms[6].name, "Secret");
    strcpy(rooms[6].description, "A hidden room with strange symbols carved on the walls.");
    strcpy(rooms[6].exits[0], "e"); strcpy(rooms[6].exitDest[0], "Library");
    rooms[6].exitCount = 1;
    strcpy(rooms[6].items[0], "Amulet");
    rooms[6].itemCount = 1;
    rooms[6].enemyHealth = 0;
    rooms[6].isWin = 0;
    
    int currentRoom = 0;
    char inventory[5][20];
    int invCount = 0;
    int health = 100;
    char message[256] = "";
    
    while (1) {
        clear_screen();
        printf("=== Text Adventure ===\n");
        printf("Health: %d\n\n", health);
        printf("Room: %s\n", rooms[currentRoom].name);
        printf("%s\n\n", rooms[currentRoom].description);
        
        if (rooms[currentRoom].itemCount > 0) {
            printf("Items here: ");
            for (int i = 0; i < rooms[currentRoom].itemCount; i++)
                printf("%s ", rooms[currentRoom].items[i]);
            printf("\n");
        }
        
        if (rooms[currentRoom].enemyHealth > 0) {
            printf("WARNING: %s (HP: %d) is here!\n", 
                   rooms[currentRoom].enemyName, rooms[currentRoom].enemyHealth);
        }
        
        printf("\nExits: ");
        for (int i = 0; i < rooms[currentRoom].exitCount; i++)
            printf("%s ", rooms[currentRoom].exits[i]);
        printf("\n");
        
        printf("\nInventory: ");
        for (int i = 0; i < invCount; i++) printf("%s ", inventory[i]);
        printf("\n");
        
        printf("\n%s\n", message);
        
        if (health <= 0) {
            printf("\nYou died! Game Over!\n");
            break;
        }
        if (rooms[currentRoom].isWin) {
            printf("\nYOU FOUND THE TREASURE! YOU WIN!\n");
            break;
        }
        
        printf("\nCommand (n/s/e/w/u/d=move, i=inventory, get/take <item>, use <item>, attack, q): ");
        char cmd[50], action[20], item[20];
        scanf("%s", cmd);
        
        strcpy(message, "");
        
        if (strcmp(cmd, "q") == 0) break;
        else if (strcmp(cmd, "i") == 0) {
            strcpy(message, "Inventory: ");
            for (int i = 0; i < invCount; i++) {
                strcat(message, inventory[i]);
                strcat(message, " ");
            }
        }
        else if (strcmp(cmd, "get") == 0 || strcmp(cmd, "take") == 0) {
            scanf("%s", item);
            int found = -1;
            for (int i = 0; i < rooms[currentRoom].itemCount; i++) {
                if (strcmp(rooms[currentRoom].items[i], item) == 0) found = i;
            }
            if (found >= 0) {
                strcpy(inventory[invCount++], rooms[currentRoom].items[found]);
                for (int i = found; i < rooms[currentRoom].itemCount - 1; i++)
                    strcpy(rooms[currentRoom].items[i], rooms[currentRoom].items[i+1]);
                rooms[currentRoom].itemCount--;
                sprintf(message, "You picked up %s", item);
            } else {
                strcpy(message, "Item not found!");
            }
        }
        else if (strcmp(cmd, "use") == 0) {
            scanf("%s", item);
            int hasItem = 0;
            for (int i = 0; i < invCount; i++) if (strcmp(inventory[i], item) == 0) hasItem = 1;
            if (!hasItem) { strcpy(message, "You don't have that!"); }
            else if (strcmp(item, "Key") == 0) {
                for (int i = 0; i < rooms[currentRoom].exitCount; i++) {
                    if (rooms[currentRoom].locked[i]) {
                        rooms[currentRoom].locked[i] = 0;
                        sprintf(message, "You unlocked the %s door!", rooms[currentRoom].exits[i]);
                        break;
                    }
                }
                if (strlen(message) == 0) strcpy(message, "No locked doors here");
            }
            else if (strcmp(item, "Knife") == 0) {
                if (rooms[currentRoom].enemyHealth > 0) {
                    rooms[currentRoom].enemyHealth -= 25;
                    sprintf(message, "You attacked %s!", rooms[currentRoom].enemyName);
                    if (rooms[currentRoom].enemyHealth <= 0) {
                        strcat(message, " It was defeated!");
                    } else {
                        health -= rooms[currentRoom].enemyDamage;
                        sprintf(message + strlen(message), " It反击! -%d HP", rooms[currentRoom].enemyDamage);
                    }
                } else strcpy(message, "No enemy to attack");
            }
            else strcpy(message, "Can't use that here");
        }
        else if (strcmp(cmd, "attack") == 0 || strcmp(cmd, "fight") == 0) {
            if (rooms[currentRoom].enemyHealth > 0) {
                int hasKnife = 0;
                for (int i = 0; i < invCount; i++) if (strcmp(inventory[i], "Knife") == 0) hasKnife = 1;
                if (hasKnife) {
                    rooms[currentRoom].enemyHealth -= 25;
                    sprintf(message, "You attacked with Knife!");
                } else {
                    rooms[currentRoom].enemyHealth -= 10;
                    sprintf(message, "You attacked with bare hands!");
                }
                if (rooms[currentRoom].enemyHealth <= 0) {
                    strcat(message, " Enemy defeated!");
                } else {
                    health -= rooms[currentRoom].enemyDamage;
                    sprintf(message + strlen(message), " Enemy反击! -%d HP", rooms[currentRoom].enemyDamage);
                }
            } else strcpy(message, "No enemy here");
        }
        else {
            int moved = 0;
            for (int i = 0; i < rooms[currentRoom].exitCount; i++) {
                if (strcmp(cmd, rooms[currentRoom].exits[i]) == 0) {
                    if (rooms[currentRoom].locked[i]) {
                        strcpy(message, "That door is locked!");
                    } else {
                        for (int r = 0; r < 7; r++) {
                            if (strcmp(rooms[currentRoom].exitDest[i], rooms[r].name) == 0) {
                                currentRoom = r;
                                moved = 1;
                                sprintf(message, "You entered %s", rooms[r].name);
                                if (rooms[r].enemyHealth > 0) {
                                    sprintf(message + strlen(message), ". %s attacks!", rooms[r].enemyName);
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            if (!moved && strlen(message) == 0) strcpy(message, "Can't go that way!");
        }
    }
    getchar();
}

// ======================== 14. 3x3 Slide Puzzle ========================
void play3x3Puzzle() {
    char board[3][3] = {{'1','2','3'},{'4','5','6'},{'7','8',' '}};
    int emptyR = 2, emptyC = 2;
    int moves = 0;
    
    for (int i = 0; i < 100; i++) {
        int dr[4] = {-1,1,0,0}, dc[4] = {0,0,-1,1};
        int idx = random_int(4);
        int nr = emptyR + dr[idx], nc = emptyC + dc[idx];
        if (nr >= 0 && nr < 3 && nc >= 0 && nc < 3) {
            board[emptyR][emptyC] = board[nr][nc];
            board[nr][nc] = ' ';
            emptyR = nr; emptyC = nc;
        }
    }
    
    while (1) {
        clear_screen();
        printf("=== 3x3 Slide Puzzle ===\n");
        printf("Moves: %d\n\n", moves);
        
        for (int i = 0; i < 3; i++) {
            printf("+---+---+---+\n|");
            for (int j = 0; j < 3; j++)
                printf(" %c |", board[i][j] == ' ' ? '.' : board[i][j]);
            printf("\n");
        }
        printf("+---+---+---+\n");
        
        int won = 1;
        char expected[9] = {'1','2','3','4','5','6','7','8',' '};
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (board[i][j] != expected[i*3+j]) won = 0;
        
        if (won) { printf("\nYOU WIN! Moves: %d\n", moves); break; }
        
        printf("\nWASD to move, q to quit: ");
        char cmd[10]; scanf("%s", cmd);
        if (strcmp(cmd, "q") == 0) break;
        
        int dr = 0, dc = 0;
        if (strcmp(cmd, "w") == 0) dr = -1;
        else if (strcmp(cmd, "s") == 0) dr = 1;
        else if (strcmp(cmd, "a") == 0) dc = -1;
        else if (strcmp(cmd, "d") == 0) dc = 1;
        else continue;
        
        int nr = emptyR + dr, nc = emptyC + dc;
        if (nr >= 0 && nr < 3 && nc >= 0 && nc < 3) {
            board[emptyR][emptyC] = board[nr][nc];
            board[nr][nc] = ' ';
            emptyR = nr; emptyC = nc;
            moves++;
        }
    }
    getchar();
}

// ======================== 15. Number Position ========================
void playNumberPosition() {
    char secret[5];
    int digits[10] = {0,1,2,3,4,5,6,7,8,9};
    for (int i = 0; i < 10; i++) {
        int r = random_int(10), tmp = digits[i];
        digits[i] = digits[r]; digits[r] = tmp;
    }
    for (int i = 0; i < 4; i++) secret[i] = '0' + digits[i];
    secret[4] = '\0';
    
    int attempts = 0, maxAttempts = 10;
    char message[256] = "Guess 4 unique digits (e.g., 1234):";
    
    while (attempts < maxAttempts) {
        clear_screen();
        printf("=== Number Position ===\n");
        printf("Attempts: %d/%d\n\n", attempts, maxAttempts);
        printf("Rules: A = correct digit & position, B = correct digit wrong position\n\n");
        printf("%s\n", message);
        
        if (attempts >= maxAttempts) {
            printf("\nGAME OVER! Answer: %s\n", secret);
            break;
        }
        
        char guess[10]; scanf("%s", guess);
        if (strcmp(guess, "q") == 0) break;
        
        if (strlen(guess) != 4 || guess[0] == guess[1] || guess[0] == guess[2] || 
            guess[0] == guess[3] || guess[1] == guess[2] || guess[1] == guess[3] || 
            guess[2] == guess[3] || !isdigit(guess[0])) {
            sprintf(message, "Need 4 unique digits! Try again:");
            continue;
        }
        
        int a = 0, b = 0;
        for (int i = 0; i < 4; i++) {
            if (guess[i] == secret[i]) a++;
            else for (int j = 0; j < 4; j++) 
                if (guess[i] == secret[j]) { b++; break; }
        }
        
        if (a == 4) { printf("\nCORRECT! You won in %d attempts!\n", attempts+1); break; }
        sprintf(message, "%s: %dA%dB\nTry again:", guess, a, b);
        attempts++;
    }
    getchar();
}

// ======================== 16. Number Sort ========================
void playNumberSort() {
    int nums[4] = {1,2,3,4};
    int swaps[4][2] = {{0,1},{1,2},{2,3},{0,3}};
    for (int i = 0; i < 10; i++) {
        int idx = random_int(4);
        int a = swaps[idx][0], b = swaps[idx][1];
        int tmp = nums[a]; nums[a] = nums[b]; nums[b] = tmp;
    }
    
    int steps = 0, minSteps = 0;
    int sorted[4]; for (int i = 0; i < 4; i++) sorted[i] = nums[i];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 3; j++)
            if (sorted[j] > sorted[j+1]) { int t = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = t; minSteps++; }
    
    char message[256] = "Enter positions to swap (e.g., 1 2):";
    
    while (1) {
        clear_screen();
        printf("=== Number Sort ===\n");
        printf("Sort to: 1 2 3 4\n");
        printf("Current: %d %d %d %d\n", nums[0], nums[1], nums[2], nums[3]);
        printf("Steps: %d (best: %d)\n\n", steps, minSteps);
        printf("%s\n", message);
        
        int sortedNow = 1;
        for (int i = 0; i < 3; i++) if (nums[i] > nums[i+1]) sortedNow = 0;
        if (sortedNow) { printf("\nYOU WIN! Steps: %d (best: %d)\n", steps, minSteps); break; }
        
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int p1, p2;
        if (sscanf(input, "%d %d", &p1, &p2) != 2 || p1 < 1 || p1 > 4 || p2 < 1 || p2 > 4) {
            strcpy(message, "Invalid! Format: 1 2 (swap pos 1 and 2)");
            continue;
        }
        p1--; p2--;
        if (abs(p1-p2) != 1) {
            strcpy(message, "Must swap adjacent positions!");
            continue;
        }
        int tmp = nums[p1]; nums[p1] = nums[p2]; nums[p2] = tmp;
        steps++;
    }
    getchar();
}

// ======================== 17. Word Spelling ========================
void playWordSpelling() {
    char* words[] = {"apple", "banana", "computer", "driver", "elephant", "game", "have", "in", "just", "know", "light", "mouse", "not", "off", "play", "quick", "read", "sun", "time", "you"};
    int wordCount = 20;
    int score = 0, round = 0, maxRounds = 10;
    char message[256] = "Press Enter to start!";
    
    while (round < maxRounds) {
        clear_screen();
        printf("=== Word Spelling ===\n");
        printf("Score: %d/%d  Round: %d/%d\n\n", score, maxRounds, round+1, maxRounds);
        
        if (round == 0) {
            printf("%s\n", message);
            getchar(); getchar();
        }
        
        char* current = words[random_int(wordCount)];
        printf("Spell this word: %s\n", strupr(current));
        printf("Your answer: ");
        
        char input[50]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        if (strcmp(input, current) == 0) {
            score++;
            sprintf(message, "CORRECT! Score: %d", score);
        } else {
            sprintf(message, "WRONG! Answer: %s", strupr(current));
        }
        round++;
    }
    printf("\nGAME OVER! Final Score: %d/%d\n", score, maxRounds);
    getchar();
}

// ======================== 18. Color Match ========================
void playColorMatch() {
    char* colorNames[] = {"Red", "Green", "Yellow", "Blue", "Purple", "Cyan"};
    char* colorCodes[] = {"\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m"};
    int score = 0, round = 0, maxRounds = 10;
    char message[256] = "Press Enter to start!";
    
    while (round < maxRounds) {
        clear_screen();
        printf("=== Color Match ===\n");
        printf("Score: %d/%d  Round: %d/%d\n\n", score, maxRounds, round+1, maxRounds);
        
        if (round == 0) {
            printf("%s\n", message);
            printf("Match the COLOR of the text, not the word!\n");
            getchar(); getchar();
        }
        
        int colorIdx = random_int(6);
        int wordIdx = random_int(6);
        if (random_int(2) == 0) wordIdx = colorIdx;
        
        printf("Text color: %s%s\033[0m\n", colorCodes[colorIdx], colorNames[wordIdx]);
        printf("Enter color (Red/Green/Yellow/Blue/Purple/Cyan) or q: ");
        
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        if (strcasecmp(input, colorNames[colorIdx]) == 0) {
            score++;
            sprintf(message, "CORRECT!");
        } else {
            sprintf(message, "WRONG! It was %s", colorNames[colorIdx]);
        }
        round++;
    }
    printf("\nGAME OVER! Final Score: %d/%d\n", score, maxRounds);
    getchar();
}

// ======================== 19. Shape Match ========================
void playShapeMatch() {
    char* shapes[] = {"▲", "■", "●", "◆", "★", "♥"};
    char board[8][8];
    int selectedR = -1, selectedC = -1;
    int score = 0, round = 0;
    
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = random_int(6);
    
    while (1) {
        clear_screen();
        printf("=== Shape Match ===\n");
        printf("Score: %d\n\n", score);
        
        printf("  ");
        for (int j = 0; j < 8; j++) printf(" %d ", j+1);
        printf("\n");
        for (int i = 0; i < 8; i++) {
            printf("%d ", i+1);
            for (int j = 0; j < 8; j++) {
                if (i == selectedR && j == selectedC) printf("[%s]", shapes[board[i][j]]);
                else printf(" %s ", shapes[board[i][j]]);
            }
            printf("\n");
        }
        
        int matches = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 7; j++)
                if (board[i][j] == board[i][j+1] && board[i][j] >= 0) matches++;
        
        if (matches == 0) {
            printf("\nALL CLEARED! YOU WIN! Score: %d\n", score);
            break;
        }
        
        printf("\nEnter row col (e.g., 1 1) or q: ");
        char input[20]; scanf("%s", input);
        if (strcmp(input, "q") == 0) break;
        
        int r, c;
        if (sscanf(input, "%d %d", &r, &c) != 2 || r < 1 || r > 8 || c < 1 || c > 8) {
            continue;
        }
        r--; c--;
        
        if (selectedR == -1) {
            selectedR = r; selectedC = c;
        } else {
            if (selectedR == r && selectedC == c) {
                selectedR = -1; selectedC = -1;
            } else if (board[selectedR][selectedC] == board[r][c]) {
                board[selectedR][selectedC] = -1;
                board[r][c] = -1;
                score += 10;
                selectedR = -1; selectedC = -1;
            } else {
                selectedR = r; selectedC = c;
            }
        }
    }
    getchar();
}

// ======================== Main Menu ========================
void showMenu() {
    clear_screen();
    printf("========== GAME MENU ==========\n");
    printf("1. 100-Floor Tower\n");
    printf("2. Snake\n");
    printf("3. Guess Number\n");
    printf("4. Rock Paper Scissors\n");
    printf("5. Tic-Tac-Toe\n");
    printf("6. Memory Card\n");
    printf("7. 4x4 Slide Puzzle\n");
    printf("8. Maze\n");
    printf("9. Sudoku\n");
    printf("10. Minesweeper\n");
    printf("11. 2048\n");
    printf("12. Gomoku\n");
    printf("13. Text Adventure\n");
    printf("14. 3x3 Slide Puzzle\n");
    printf("15. Number Position\n");
    printf("16. Number Sort\n");
    printf("17. Word Spelling\n");
    printf("18. Color Match\n");
    printf("19. Shape Match\n");
    printf("q. Quit\n");
    printf("\nSelect game: ");
}

int main() {
    srand(time(NULL));
    
    while (1) {
        showMenu();
        char choice[10];
        scanf("%s", choice);
        
        if (strcmp(choice, "q") == 0) { printf("Goodbye!\n"); break; }
        
        if (strcmp(choice, "1") == 0) playTowerGame();
        else if (strcmp(choice, "2") == 0) playSnakeGame();
        else if (strcmp(choice, "3") == 0) playGuessNumberGame();
        else if (strcmp(choice, "4") == 0) playRockPaperScissors();
        else if (strcmp(choice, "5") == 0) playTicTacToe();
        else if (strcmp(choice, "6") == 0) playMemoryGame();
        else if (strcmp(choice, "7") == 0) playSlidePuzzle();
        else if (strcmp(choice, "8") == 0) playMazeGame();
        else if (strcmp(choice, "9") == 0) playSudokuGame();
        else if (strcmp(choice, "10") == 0) playMinesweeperGame();
        else if (strcmp(choice, "11") == 0) play2048Game();
        else if (strcmp(choice, "12") == 0) playGomokuGame();
        else if (strcmp(choice, "13") == 0) playTextAdventure();
        else if (strcmp(choice, "14") == 0) play3x3Puzzle();
        else if (strcmp(choice, "15") == 0) playNumberPosition();
        else if (strcmp(choice, "16") == 0) playNumberSort();
        else if (strcmp(choice, "17") == 0) playWordSpelling();
        else if (strcmp(choice, "18") == 0) playColorMatch();
        else if (strcmp(choice, "19") == 0) playShapeMatch();
        else { printf("Invalid! Press Enter..."); getchar(); }
    }
    
    return 0;
}
