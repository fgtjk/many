#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <sstream>
using namespace std;

int random_int(int max) {
    return rand() % max;
}

void clear_screen() {
    system("cls");
}

string toUpper(string s) {
    for (char& c : s) c = toupper(c);
    return s;
}

// ======================== 1. Tower Game ========================
struct TowerState {
    int hp, maxHp, attack, defense, gold, floor;
    int keys[3];
    int playerX, playerY;
    int floors[101][10][10];
    string message;
    bool hasSword, hasShield, gameOver;
};

void generateTowerFloors(TowerState* s) {
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
            
            int enemyCount = random_int(5) + 3;
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

void initTower(TowerState* s) {
    generateTowerFloors(s);
    s->hp = 200; s->maxHp = 200; s->attack = 20; s->defense = 20;
    s->gold = 0; s->floor = 1;
    s->keys[0] = 1; s->keys[1] = 0; s->keys[2] = 0;
    s->playerX = 5; s->playerY = 5;
    s->hasSword = false; s->hasShield = false; s->gameOver = false;
    s->message = "";
}

void towerMovePlayer(TowerState* s, int dx, int dy) {
    int nx = s->playerX + dx;
    int ny = s->playerY + dy;
    int tile = s->floors[s->floor][ny][nx];
    
    switch(tile) {
        case 0:
            s->playerX = nx; s->playerY = ny; s->message = ""; break;
        case 1: s->message = "Hit a wall!"; break;
        case 2: {
            int eHp = 30 + s->floor * 2;
            int eAtk = 10 + s->floor / 3;
            int eDef = 5 + s->floor / 5;
            int eGold = 10 + s->floor;
            int hits = (eHp + s->attack - eDef - 1) / max(1, s->attack - eDef);
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                s->message = "Defeated enemy! Got " + to_string(eGold) + " gold, lost " + to_string(dmgToPlayer * (hits - 1)) + " HP";
            } else {
                s->message = "You were defeated! Game over.";
                s->gameOver = true;
            }
            break;
        }
        case 3: {
            string itemType;
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
            
            if (itemType == "yellow_key") { s->keys[0]++; s->message = "Got 1 Yellow Key!"; }
            else if (itemType == "blue_key") { s->keys[1]++; s->message = "Got 1 Blue Key!"; }
            else if (itemType == "red_key") { s->keys[2]++; s->message = "Got 1 Red Key!"; }
            else if (itemType == "potion") { s->hp = min(s->hp + 50, s->maxHp); s->message = "Recovered 50 HP!"; }
            else if (itemType == "super_potion") { s->hp = min(s->hp + 120, s->maxHp); s->message = "Recovered 120 HP!"; }
            else if (itemType == "legendary_sword") { s->attack += 30; s->hasSword = true; s->message = "Got Legendary Sword! ATK +30"; }
            else if (itemType == "magic_shield") { s->defense += 20; s->hasShield = true; s->message = "Got Magic Shield! DEF +20"; }
            
            s->floors[s->floor][ny][nx] = 0;
            s->playerX = nx; s->playerY = ny;
            break;
        }
        case 4: {
            int newFloor = s->floor + 1;
            if (newFloor >= 1 && newFloor <= 100) {
                s->floor = newFloor;
                s->message = "Now at Floor " + to_string(s->floor);
                do { s->playerX = random_int(8) + 1; s->playerY = random_int(8) + 1; } while (s->floors[s->floor][s->playerY][s->playerX] != 0);
            } else {
                s->message = "Cannot leave the tower!";
            }
            break;
        }
        case 5: {
            int newFloor = s->floor - 1;
            if (s->keys[1] > 0) {
                s->keys[1]--;
                if (newFloor >= 1 && newFloor <= 100) {
                    s->floor = newFloor;
                    s->message = "Used Blue Key. Now at Floor " + to_string(s->floor);
                    do { s->playerX = random_int(8) + 1; s->playerY = random_int(8) + 1; } while (s->floors[s->floor][s->playerY][s->playerX] != 0);
                }
            } else {
                s->message = "Need Blue Key!";
            }
            break;
        }
        case 6: {
            if (!s->hasSword) { s->message = "You need the legendary sword!"; return; }
            int bossHp = 500, bossAtk = 50, bossDef = 30, bossGold = 1000;
            int hits = (bossHp + s->attack * 2 - bossDef - 1) / max(1, s->attack * 2 - bossDef);
            int dmgToBoss = (s->attack * 2 > bossDef) ? s->attack * 2 - bossDef : 1;
            int dmgToPlayer = (bossAtk > s->defense) ? bossAtk - s->defense : 1;
            
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += bossGold;
                s->playerX = nx; s->playerY = ny;
                s->message = "Defeated Boss! Congratulations!";
                s->gameOver = true;
            } else {
                s->message = "You were defeated by Boss!";
                s->gameOver = true;
            }
            break;
        }
        case 7: {
            int eHp = 80 + s->floor * 2, eAtk = 20 + s->floor / 2, eDef = 10 + s->floor / 4, eGold = 30 + s->floor * 2;
            int hits = (eHp + s->attack - eDef - 1) / max(1, s->attack - eDef);
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                s->message = "Defeated enemy! Got " + to_string(eGold) + " gold";
            } else { s->message = "You were defeated!"; s->gameOver = true; }
            break;
        }
        case 8: {
            int eHp = 120 + s->floor * 3, eAtk = 30 + s->floor / 2, eDef = 15 + s->floor / 3, eGold = 50 + s->floor * 3;
            int hits = (eHp + s->attack - eDef - 1) / max(1, s->attack - eDef);
            int dmgToEnemy = (s->attack > eDef) ? s->attack - eDef : 1;
            int dmgToPlayer = (eAtk > s->defense) ? eAtk - s->defense : 1;
            
            s->hp -= dmgToPlayer * (hits - 1);
            if (s->hp > 0) {
                s->floors[s->floor][ny][nx] = 0;
                s->gold += eGold;
                s->playerX = nx; s->playerY = ny;
                s->message = "Defeated enemy! Got " + to_string(eGold) + " gold";
            } else { s->message = "You were defeated!"; s->gameOver = true; }
            break;
        }
    }
}

void playTowerGame() {
    TowerState state;
    initTower(&state);
    
    while (1) {
        clear_screen();
        cout << "=== 100-Floor Tower ===" << endl;
        cout << "Floor: " << state.floor << " | HP: " << state.hp << "/" << state.maxHp 
             << " | ATK: " << state.attack << " | DEF: " << state.defense << " | Gold: " << state.gold << endl;
        cout << "Keys: Y[" << state.keys[0] << "] B[" << state.keys[1] << "] R[" << state.keys[2] << "]" << endl;
        if (state.hasSword) cout << "[Sword] ";
        if (state.hasShield) cout << "[Shield] ";
        cout << "\n\n  0123456789" << endl;
        
        for (int y = 0; y < 10; y++) {
            cout << y << " ";
            for (int x = 0; x < 10; x++) {
                if (x == state.playerX && y == state.playerY) cout << "@";
                else {
                    char tile = " .#SGEBODM"[state.floors[state.floor][y][x]];
                    cout << tile;
                }
            }
            cout << endl;
        }
        cout << " +----------+" << endl << endl;
        cout << state.message << endl;
        
        if (state.gameOver) { cout << "\nPress Enter..."; cin.get(); break; }
        
        cout << "\nw/a/s/d=Move, q=Quit: ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        if (cmd == "w") towerMovePlayer(&state, 0, -1);
        else if (cmd == "s") towerMovePlayer(&state, 0, 1);
        else if (cmd == "a") towerMovePlayer(&state, -1, 0);
        else if (cmd == "d") towerMovePlayer(&state, 1, 0);
    }
}

// ======================== 2. Snake Game ========================
struct SnakeState {
    int headX, headY, length, foodX, foodY, score, dir;
    bool gameOver;
    vector<pair<int,int>> body;
};

void playSnakeGame() {
    SnakeState s;
    s.headX = 10; s.headY = 7;
    s.length = 3;
    s.score = 0;
    s.dir = 1;
    s.gameOver = false;
    s.body.clear();
    for (int i = 0; i < 3; i++) s.body.push_back({10, 7-i});
    s.foodX = random_int(20);
    s.foodY = random_int(15);
    
    while (!s.gameOver) {
        clear_screen();
        cout << "=== Snake === Score: " << s.score << endl << endl;
        
        for (int y = 0; y < 15; y++) {
            for (int x = 0; x < 20; x++) {
                bool isBody = false;
                for (auto& b : s.body) if (b.first == x && b.second == y) { isBody = true; break; }
                if (x == s.headX && y == s.headY) cout << "O";
                else if (x == s.foodX && y == s.foodY) cout << "*";
                else if (isBody) cout << "o";
                else cout << " ";
            }
            cout << endl;
        }
        
        cout << "\nWASD to move, q to quit: ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        if (cmd == "w" && s.dir != 1) s.dir = 0;
        else if (cmd == "s" && s.dir != 0) s.dir = 1;
        else if (cmd == "a" && s.dir != 3) s.dir = 2;
        else if (cmd == "d" && s.dir != 2) s.dir = 3;
        
        int dx[4] = {0, 0, -1, 1};
        int dy[4] = {-1, 1, 0, 0};
        s.headX += dx[s.dir];
        s.headY += dy[s.dir];
        
        if (s.headX < 0 || s.headX >= 20 || s.headY < 0 || s.headY >= 15) {
            s.gameOver = true;
            cout << "\nHit wall! Game Over!" << endl;
            break;
        }
        
        for (auto& b : s.body) {
            if (s.headX == b.first && s.headY == b.second) {
                s.gameOver = true;
                cout << "\nHit self! Game Over!" << endl;
                break;
            }
        }
        if (s.gameOver) break;
        
        s.body.insert(s.body.begin(), {s.headX, s.headY});
        
        if (s.headX == s.foodX && s.headY == s.foodY) {
            s.score += 10;
            s.length++;
            s.foodX = random_int(20);
            s.foodY = random_int(15);
        } else {
            s.body.pop_back();
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 3. Guess Number ========================
void playGuessNumberGame() {
    int number = random_int(100) + 1;
    int guess, attempts = 0;
    
    while (1) {
        clear_screen();
        cout << "=== Guess Number ===" << endl;
        cout << "I'm thinking of a number 1-100" << endl;
        cout << "Attempts: " << attempts << endl;
        cout << "\nYour guess: ";
        cin >> guess;
        
        if (guess < 1 || guess > 100) {
            cout << "Out of range!" << endl;
            continue;
        }
        
        attempts++;
        
        if (guess < number) {
            cout << "Too low!" << endl;
        } else if (guess > number) {
            cout << "Too high!" << endl;
        } else {
            cout << "Correct! The number was " << number << endl;
            cout << "You got it in " << attempts << " attempts!" << endl;
            break;
        }
        
        if (attempts >= 10) {
            cout << "Game over! The number was " << number << endl;
            break;
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 4. Rock Paper Scissors ========================
void playRockPaperScissors() {
    const string choices[] = {"Rock", "Paper", "Scissors"};
    
    while (1) {
        clear_screen();
        cout << "=== Rock Paper Scissors ===" << endl;
        cout << "1. Rock\n2. Paper\n3. Scissors\nq. Quit" << endl;
        cout << "\nYour choice: ";
        string cmd; cin >> cmd;
        
        if (cmd == "q") break;
        if (cmd != "1" && cmd != "2" && cmd != "3") continue;
        
        int player = stoi(cmd) - 1;
        int computer = random_int(3);
        
        cout << "You: " << choices[player] << " vs Computer: " << choices[computer] << endl;
        
        if (player == computer) {
            cout << "Tie!" << endl;
        } else if ((player == 0 && computer == 2) ||
                   (player == 1 && computer == 0) ||
                   (player == 2 && computer == 1)) {
            cout << "You win!" << endl;
        } else {
            cout << "You lose!" << endl;
        }
        
        cout << "Press Enter..."; cin.get(); cin.get();
    }
}

// ======================== 5. Tic-Tac-Toe ========================
void playTicTacToe() {
    char board[3][3] = {{' ',' ',' '},{' ',' ',' '},{' ',' ',' '}};
    int moves = 0;
    
    while (1) {
        clear_screen();
        cout << "=== Tic-Tac-Toe ===" << endl;
        cout << "  1 2 3" << endl;
        for (int i = 0; i < 3; i++) {
            cout << i+1 << " ";
            for (int j = 0; j < 3; j++) cout << board[i][j] << " ";
            cout << endl;
        }
        
        int win[8][3] = {{0,0,1},{0,0,2},{0,0,3},{1,0,1},{1,1,2},{2,0,1},{0,1,2},{0,2,3}};
        bool xWin = false, oWin = false;
        for (int i = 0; i < 3; i++) if (board[i][0]==board[i][1] && board[i][1]==board[i][2] && board[i][0]!=' ') { if(board[i][0]=='X') xWin=true; else oWin=true; }
        for (int j = 0; j < 3; j++) if (board[0][j]==board[1][j] && board[1][j]==board[2][j] && board[0][j]!=' ') { if(board[0][j]=='X') xWin=true; else oWin=true; }
        if (board[0][0]==board[1][1] && board[1][1]==board[2][2] && board[0][0]!=' ') { if(board[0][0]=='X') xWin=true; else oWin=true; }
        if (board[0][2]==board[1][1] && board[1][1]==board[2][0] && board[0][2]!=' ') { if(board[0][2]=='X') xWin=true; else oWin=true; }
        
        if (xWin) { cout << "X wins!" << endl; break; }
        if (oWin) { cout << "O wins!" << endl; break; }
        if (moves >= 9) { cout << "Tie!" << endl; break; }
        
        char player = (moves % 2 == 0) ? 'X' : 'O';
        cout << "\nPlayer " << player << "'s move (row col): ";
        int r, c; cin >> r >> c;
        
        if (r < 1 || r > 3 || c < 1 || c > 3 || board[r-1][c-1] != ' ') {
            cout << "Invalid move!" << endl;
            continue;
        }
        
        board[r-1][c-1] = player;
        moves++;
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 6. Memory Card ========================
void playMemoryGame() {
    string symbols[] = {"A","B","C","D","E","F","G","H"};
    string board[16];
    string revealed[16];
    for (int i = 0; i < 8; i++) {
        board[i] = symbols[i];
        board[i+8] = symbols[i];
        revealed[i] = "?";
        revealed[i+8] = "?";
    }
    for (int i = 0; i < 16; i++) {
        int r = random_int(16);
        swap(board[i], board[r]);
    }
    
    int matches = 0, moves = 0;
    
    while (matches < 8) {
        clear_screen();
        cout << "=== Memory Card ===" << endl;
        cout << "Matches: " << matches << "/8  Moves: " << moves << endl << endl;
        
        cout << "  ";
        for (int i = 0; i < 4; i++) cout << i+1 << " ";
        cout << endl;
        for (int row = 0; row < 4; row++) {
            cout << row+1 << " ";
            for (int col = 0; col < 4; col++) {
                cout << revealed[row*4+col] << " ";
            }
            cout << endl;
        }
        
        cout << "\nFirst card (row col): ";
        int r1, c1; cin >> r1 >> c1;
        if (r1 < 1 || r1 > 4 || c1 < 1 || c1 > 4) continue;
        int idx1 = (r1-1)*4 + (c1-1);
        
        revealed[idx1] = board[idx1];
        
        clear_screen();
        cout << "=== Memory Card ===" << endl;
        cout << "Matches: " << matches << "/8  Moves: " << moves << endl << endl;
        cout << "  ";
        for (int i = 0; i < 4; i++) cout << i+1 << " ";
        cout << endl;
        for (int row = 0; row < 4; row++) {
            cout << row+1 << " ";
            for (int col = 0; col < 4; col++) {
                cout << revealed[row*4+col] << " ";
            }
            cout << endl;
        }
        
        cout << "\nSecond card (row col): ";
        int r2, c2; cin >> r2 >> c2;
        if (r2 < 1 || r2 > 4 || c2 < 1 || c2 > 4) { revealed[idx1] = "?"; continue; }
        int idx2 = (r2-1)*4 + (c2-1);
        if (idx1 == idx2) { revealed[idx1] = "?"; continue; }
        
        revealed[idx2] = board[idx2];
        
        clear_screen();
        cout << "=== Memory Card ===" << endl;
        cout << "Matches: " << matches << "/8  Moves: " << moves << endl << endl;
        cout << "  ";
        for (int i = 0; i < 4; i++) cout << i+1 << " ";
        cout << endl;
        for (int row = 0; row < 4; row++) {
            cout << row+1 << " ";
            for (int col = 0; col < 4; col++) {
                cout << revealed[row*4+col] << " ";
            }
            cout << endl;
        }
        
        if (board[idx1] == board[idx2]) {
            matches++;
            cout << "\nMatch!" << endl;
        } else {
            cout << "\nNo match!" << endl;
            revealed[idx1] = "?";
            revealed[idx2] = "?";
        }
        moves++;
        cout << "Press Enter..."; cin.get(); cin.get();
    }
    cout << "You won in " << moves << " moves!" << endl;
}

// ======================== 7. 4x4 Slide Puzzle ========================
void playSlidePuzzle() {
    string board[16];
    for (int i = 0; i < 15; i++) board[i] = to_string(i+1);
    board[15] = " ";
    int emptyIdx = 15;
    
    for (int i = 0; i < 200; i++) {
        int adj[4] = {-1, 1, -4, 4};
        vector<int> valid;
        int row = emptyIdx / 4, col = emptyIdx % 4;
        if (row > 0) valid.push_back(emptyIdx - 4);
        if (row < 3) valid.push_back(emptyIdx + 4);
        if (col > 0) valid.push_back(emptyIdx - 1);
        if (col < 3) valid.push_back(emptyIdx + 1);
        
        int r = valid[random_int(valid.size())];
        swap(board[emptyIdx], board[r]);
        emptyIdx = r;
    }
    
    while (1) {
        clear_screen();
        cout << "=== 4x4 Slide Puzzle ===" << endl << endl;
        
        for (int i = 0; i < 4; i++) {
            cout << "+";
            for (int j = 0; j < 4; j++) cout << "----+";
            cout << "\n|";
            for (int j = 0; j < 4; j++) {
                string val = board[i*4+j];
                if (val == " ") cout << "    |";
                else cout << val << (val.size()==1?"   ":"  ") << "|";
            }
            cout << endl;
        }
        cout << "+";
        for (int j = 0; j < 4; j++) cout << "----+";
        cout << endl;
        
        bool won = true;
        for (int i = 0; i < 15; i++) {
            if (board[i] != to_string(i+1)) { won = false; break; }
        }
        if (won) { cout << "\nYou win!" << endl; break; }
        
        cout << "\nWASD to move, q to quit: ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        
        int row = emptyIdx / 4, col = emptyIdx % 4;
        int newIdx = emptyIdx;
        
        if (cmd == "w" && row > 0) newIdx = emptyIdx - 4;
        else if (cmd == "s" && row < 3) newIdx = emptyIdx + 4;
        else if (cmd == "a" && col > 0) newIdx = emptyIdx - 1;
        else if (cmd == "d" && col < 3) newIdx = emptyIdx + 1;
        
        if (newIdx != emptyIdx) {
            swap(board[emptyIdx], board[newIdx]);
            emptyIdx = newIdx;
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 8. Maze ========================
void playMazeGame() {
    const int W = 21, H = 15;
    vector<string> maze(H, string(W, '#'));
    int px = 1, py = 1;
    
    for (int y = 1; y < H-1; y++) {
        for (int x = 1; x < W-1; x++) {
            if (random_int(100) < 70) maze[y][x] = ' ';
        }
    }
    maze[1][1] = 'S';
    maze[H-2][W-2] = 'E';
    
    while (1) {
        clear_screen();
        cout << "=== Maze ===" << endl;
        
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                if (x == px && y == py) cout << "P";
                else cout << maze[y][x];
            }
            cout << endl;
        }
        
        if (px == W-2 && py == H-2) {
            cout << "\nYou found the exit! You win!" << endl;
            break;
        }
        
        cout << "\nWASD to move: ";
        string cmd; cin >> cmd;
        
        int nx = px, ny = py;
        if (cmd == "w") ny--;
        else if (cmd == "s") ny++;
        else if (cmd == "a") nx--;
        else if (cmd == "d") nx++;
        
        if (maze[ny][nx] != '#') {
            px = nx; py = ny;
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 9. Sudoku ========================
bool validSudoku(int board[9][9], int r, int c, int n) {
    for (int i = 0; i < 9; i++) if (board[r][i] == n) return false;
    for (int i = 0; i < 9; i++) if (board[i][c] == n) return false;
    int br = (r/3)*3, bc = (c/3)*3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[br+i][bc+j] == n) return false;
    return true;
}

void playSudokuGame() {
    int board[9][9] = {0}, solution[9][9] = {0};
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int n = 1; n <= 9; n++) {
                if (validSudoku(solution, i, j, n)) {
                    solution[i][j] = n;
                    if (random_int(100) < 30) break;
                }
            }
        }
    }
    
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            board[i][j] = (solution[i][j] && random_int(100) < 40) ? solution[i][j] : 0;
    
    while (1) {
        clear_screen();
        cout << "=== Sudoku ===" << endl << endl;
        
        cout << "  123 456 789" << endl;
        for (int i = 0; i < 9; i++) {
            cout << i+1 << " ";
            for (int j = 0; j < 9; j++) {
                if (board[i][j] == 0) cout << ". ";
                else cout << board[i][j] << " ";
                if (j == 2 || j == 5) cout << "| ";
            }
            cout << endl;
            if (i == 2 || i == 5) cout << "  ---+---+---" << endl;
        }
        
        bool complete = true;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (board[i][j] == 0) complete = false;
        
        if (complete) {
            bool correct = true;
            for (int i = 0; i < 9; i++)
                for (int j = 0; j < 9; j++)
                    if (board[i][j] != solution[i][j]) correct = false;
            
            if (correct) { cout << "\nYou win!" << endl; }
            else { cout << "\nWrong solution!" << endl; }
            break;
        }
        
        cout << "\nPosition and number (row col num, 0 0 0 to quit): ";
        int r, c, n; cin >> r >> c >> n;
        if (r == 0 && c == 0 && n == 0) break;
        if (r < 1 || r > 9 || c < 1 || c > 9 || n < 0 || n > 9) continue;
        
        if (n == 0) board[r-1][c-1] = 0;
        else if (validSudoku(board, r-1, c-1, n)) board[r-1][c-1] = n;
        else cout << "Invalid move!" << endl;
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 10. Minesweeper ========================
void playMinesweeperGame() {
    const int R = 10, C = 10, M = 10;
    int board[R][C] = {0};
    int revealed[R][C] = {0};
    
    vector<pair<int,int>> cells;
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            cells.push_back({i, j});
    
    random_shuffle(cells.begin(), cells.end());
    for (int i = 0; i < M; i++) {
        board[cells[i].first][cells[i].second] = -1;
    }
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            if (board[i][j] == -1) continue;
            int count = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di, nj = j + dj;
                    if (ni >= 0 && ni < R && nj >= 0 && nj < C && board[ni][nj] == -1) count++;
                }
            board[i][j] = count;
        }
    }
    
    int revealedCount = 0;
    
    while (1) {
        clear_screen();
        cout << "=== Minesweeper === Flags: " << M << endl << endl;
        
        cout << "  ";
        for (int j = 0; j < C; j++) cout << j+1 << " ";
        cout << endl;
        for (int i = 0; i < R; i++) {
            cout << i+1 << " ";
            for (int j = 0; j < C; j++) {
                if (revealed[i][j]) {
                    if (board[i][j] == -1) cout << "* ";
                    else if (board[i][j] == 0) cout << ". ";
                    else cout << board[i][j] << " ";
                } else {
                    cout << "# ";
                }
            }
            cout << endl;
        }
        
        int minesLeft = 0;
        for (int i = 0; i < R; i++)
            for (int j = 0; j < C; j++)
                if (revealed[i][j] && board[i][j] == -1) {
                    cout << "\nGame Over! You hit a mine!" << endl;
                    break;
                }
        
        if (revealedCount == R*C - M) {
            cout << "\nYou win!" << endl;
            break;
        }
        
        cout << "\nPosition and action (row col [r=reveal, f=flag]): ";
        int r, c; char act; cin >> r >> c >> act;
        if (r < 1 || r > R || c < 1 || c > C) continue;
        
        r--; c--;
        if (act == 'r' && !revealed[r][c]) {
            revealed[r][c] = 1;
            revealedCount++;
            if (board[r][c] == 0) {
                for (int di = -1; di <= 1; di++)
                    for (int dj = -1; dj <= 1; dj++) {
                        int ni = r + di, nj = c + dj;
                        if (ni >= 0 && ni < R && nj >= 0 && nj < C && !revealed[ni][nj]) {
                            revealed[ni][nj] = 1;
                            revealedCount++;
                        }
                    }
            }
            if (board[r][c] == -1) {
                cout << "\nGame Over! You hit a mine!" << endl;
                break;
            }
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 11. 2048 ========================
void play2048Game() {
    int board[4][4] = {0};
    int empty[16], emptyCount;
    int score = 0;
    bool gameOver = false;
    string message = "";
    
    auto addTile = [&]() {
        emptyCount = 0;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (board[i][j] == 0) empty[emptyCount++] = i * 4 + j;
        
        if (emptyCount > 0) {
            int pos = empty[random_int(emptyCount)];
            board[pos / 4][pos % 4] = (random_int(10) < 9) ? 2 : 4;
        }
    };
    
    addTile();
    addTile();
    
    while (1) {
        clear_screen();
        cout << "=== 2048 === Goal: 2048" << endl << endl;
        
        for (int i = 0; i < 4; i++) {
            cout << "+";
            for (int j = 0; j < 4; j++) cout << "-----+";
            cout << "\n|";
            for (int j = 0; j < 4; j++) {
                if (board[i][j] == 0) cout << "     |";
                else cout.width(4); cout << board[i][j] << " |";
            }
            cout << endl;
        }
        cout << "+";
        for (int j = 0; j < 4; j++) cout << "-----+";
        cout << "\n\nScore: " << score << endl << message << endl;
        if (gameOver) { cin.get(); break; }
        
        cout << "> ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        
        bool moved = false;
        
        if (cmd == "w") {
            for (int c = 0; c < 4; c++) {
                for (int r = 1; r < 4; r++) {
                    if (board[r][c] != 0) {
                        int nr = r;
                        while (nr > 0 && board[nr-1][c] == 0) nr--;
                        if (nr > 0 && board[nr-1][c] == board[r][c]) {
                            board[nr-1][c] *= 2;
                            score += board[nr-1][c];
                            board[r][c] = 0;
                            moved = true;
                        } else if (nr != r) {
                            board[nr][c] = board[r][c];
                            board[r][c] = 0;
                            moved = true;
                        }
                    }
                }
            }
        } else if (cmd == "s") {
            for (int c = 0; c < 4; c++) {
                for (int r = 2; r >= 0; r--) {
                    if (board[r][c] != 0) {
                        int nr = r;
                        while (nr < 3 && board[nr+1][c] == 0) nr++;
                        if (nr < 3 && board[nr+1][c] == board[r][c]) {
                            board[nr+1][c] *= 2;
                            score += board[nr+1][c];
                            board[r][c] = 0;
                            moved = true;
                        } else if (nr != r) {
                            board[nr][c] = board[r][c];
                            board[r][c] = 0;
                            moved = true;
                        }
                    }
                }
            }
        } else if (cmd == "a") {
            for (int r = 0; r < 4; r++) {
                for (int c = 1; c < 4; c++) {
                    if (board[r][c] != 0) {
                        int nc = c;
                        while (nc > 0 && board[r][nc-1] == 0) nc--;
                        if (nc > 0 && board[r][nc-1] == board[r][c]) {
                            board[r][nc-1] *= 2;
                            score += board[r][nc-1];
                            board[r][c] = 0;
                            moved = true;
                        } else if (nc != c) {
                            board[r][nc] = board[r][c];
                            board[r][c] = 0;
                            moved = true;
                        }
                    }
                }
            }
        } else if (cmd == "d") {
            for (int r = 0; r < 4; r++) {
                for (int c = 2; c >= 0; c--) {
                    if (board[r][c] != 0) {
                        int nc = c;
                        while (nc < 3 && board[r][nc+1] == 0) nc++;
                        if (nc < 3 && board[r][nc+1] == board[r][c]) {
                            board[r][nc+1] *= 2;
                            score += board[r][nc+1];
                            board[r][c] = 0;
                            moved = true;
                        } else if (nc != c) {
                            board[r][nc] = board[r][c];
                            board[r][c] = 0;
                            moved = true;
                        }
                    }
                }
            }
        }
        
        if (moved) addTile();
        
        bool canMove = false;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 3; j++)
                if (board[i][j] == board[i][j+1]) canMove = true;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++)
                if (board[i][j] == board[i+1][j]) canMove = true;
        
        if (!canMove) {
            message = "Game Over! Score: " + to_string(score);
            gameOver = true;
        } else {
            message = "Score: " + to_string(score);
        }
    }
}

// ======================== 12. Gomoku ========================
void playGomokuGame() {
    const int N = 15;
    char board[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = '.';
    
    int cx = 7, cy = 7;
    board[cy][cx] = 'X';
    
    while (1) {
        clear_screen();
        cout << "=== Gomoku ===" << endl << "  ";
        for (int i = 0; i < N; i++) cout << i << " ";
        cout << endl;
        
        for (int y = 0; y < N; y++) {
            cout << y << " ";
            for (int x = 0; x < N; x++) cout << board[y][x] << " ";
            cout << endl;
        }
        
        cout << "\nWASD to move, q to quit, space to place: ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        
        if (cmd == "w" && cy > 0) cy--;
        else if (cmd == "s" && cy < N-1) cy++;
        else if (cmd == "a" && cx > 0) cx--;
        else if (cmd == "d" && cx < N-1) cx++;
        else if (cmd == " " && board[cy][cx] == '.') {
            board[cy][cx] = 'O';
            
            auto checkWin = [&](char p) {
                for (int y = 0; y < N; y++) {
                    for (int x = 0; x < N; x++) {
                        if (board[y][x] != p) continue;
                        int count = 1;
                        int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
                        for (auto& d : dirs) {
                            int ny = y + d[0], nx = x + d[1];
                            while (ny >= 0 && ny < N && nx >= 0 && nx < N && board[ny][nx] == p) {
                                count++;
                                ny += d[0]; nx += d[1];
                            }
                        }
                        if (count >= 5) return true;
                    }
                }
                return false;
            };
            
            if (checkWin('O')) { cout << "You win!" << endl; break; }
            
            board[cy][cx] = 'X';
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 13. Text Adventure ========================
struct Room {
    string name;
    string description;
    string exits[6];
    string exitDest[6];
    int exitCount;
    string items[5];
    int itemCount;
    int enemyHealth;
    string enemyName;
    int enemyDamage;
    int locked[6];
    bool isWin;
};

void playTextAdventure() {
    Room rooms[7];
    
    rooms[0].name = "Hall";
    rooms[0].description = "You stand in an ancient hall. Faded tapestries hang on the walls.";
    rooms[0].exits[0] = "n"; rooms[0].exitDest[0] = "Library";
    rooms[0].exits[1] = "e"; rooms[0].exitDest[1] = "Kitchen";
    rooms[0].exits[2] = "s"; rooms[0].exitDest[2] = "Garden";
    rooms[0].exitCount = 3;
    rooms[0].items[0] = "Torch";
    rooms[0].itemCount = 1;
    rooms[0].enemyHealth = 0;
    rooms[0].isWin = false;
    
    rooms[1].name = "Library";
    rooms[1].description = "Dusty bookshelves line the walls. The air smells of mildew.";
    rooms[1].exits[0] = "s"; rooms[1].exitDest[0] = "Hall";
    rooms[1].exits[1] = "w"; rooms[1].exitDest[1] = "Secret";
    rooms[1].exitCount = 2;
    rooms[1].locked[1] = 1;
    rooms[1].items[0] = "Book";
    rooms[1].itemCount = 1;
    rooms[1].enemyHealth = 0;
    rooms[1].isWin = false;
    
    rooms[2].name = "Kitchen";
    rooms[2].description = "A decrepit kitchen with dusty countertops.";
    rooms[2].exits[0] = "w"; rooms[2].exitDest[0] = "Hall";
    rooms[2].exits[1] = "n"; rooms[2].exitDest[1] = "Storage";
    rooms[2].exitCount = 2;
    rooms[2].items[0] = "Knife";
    rooms[2].itemCount = 1;
    rooms[2].enemyHealth = 0;
    rooms[2].isWin = false;
    
    rooms[3].name = "Storage";
    rooms[3].description = "A dark small room filled with crates and jars.";
    rooms[3].exits[0] = "s"; rooms[3].exitDest[0] = "Kitchen";
    rooms[3].exitCount = 1;
    rooms[3].items[0] = "Key";
    rooms[3].itemCount = 1;
    rooms[3].enemyHealth = 0;
    rooms[3].isWin = false;
    
    rooms[4].name = "Garden";
    rooms[4].description = "An overgrown garden with a dry well in the center.";
    rooms[4].exits[0] = "n"; rooms[4].exitDest[0] = "Hall";
    rooms[4].exits[1] = "d"; rooms[4].exitDest[1] = "Cellar";
    rooms[4].exitCount = 2;
    rooms[4].itemCount = 0;
    rooms[4].enemyName = "Giant Rat";
    rooms[4].enemyHealth = 30;
    rooms[4].enemyDamage = 10;
    rooms[4].isWin = false;
    
    rooms[5].name = "Cellar";
    rooms[5].description = "A dark damp cellar. Something glows in the corner.";
    rooms[5].exits[0] = "u"; rooms[5].exitDest[0] = "Garden";
    rooms[5].exitCount = 1;
    rooms[5].items[0] = "Treasure";
    rooms[5].itemCount = 1;
    rooms[5].enemyHealth = 0;
    rooms[5].isWin = true;
    
    rooms[6].name = "Secret";
    rooms[6].description = "A hidden room with strange symbols carved on the walls.";
    rooms[6].exits[0] = "e"; rooms[6].exitDest[0] = "Library";
    rooms[6].exitCount = 1;
    rooms[6].items[0] = "Amulet";
    rooms[6].itemCount = 1;
    rooms[6].enemyHealth = 0;
    rooms[6].isWin = false;
    
    int currentRoom = 0;
    vector<string> inventory;
    int health = 100;
    string message = "";
    
    while (1) {
        clear_screen();
        cout << "=== Text Adventure ===" << endl;
        cout << "Health: " << health << endl << endl;
        cout << "Room: " << rooms[currentRoom].name << endl;
        cout << rooms[currentRoom].description << endl << endl;
        
        if (rooms[currentRoom].itemCount > 0) {
            cout << "Items here: ";
            for (int i = 0; i < rooms[currentRoom].itemCount; i++)
                cout << rooms[currentRoom].items[i] << " ";
            cout << endl;
        }
        
        if (rooms[currentRoom].enemyHealth > 0) {
            cout << "WARNING: " << rooms[currentRoom].enemyName << " (HP: " << rooms[currentRoom].enemyHealth << ") is here!" << endl;
        }
        
        cout << "\nExits: ";
        for (int i = 0; i < rooms[currentRoom].exitCount; i++)
            cout << rooms[currentRoom].exits[i] << " ";
        cout << endl;
        
        cout << "\nInventory: ";
        for (string& item : inventory) cout << item << " ";
        cout << endl;
        
        cout << endl << message << endl;
        
        if (health <= 0) {
            cout << "\nYou died! Game Over!" << endl;
            break;
        }
        if (rooms[currentRoom].isWin) {
            cout << "\nYOU FOUND THE TREASURE! YOU WIN!" << endl;
            break;
        }
        
        cout << "\nCommand (n/s/e/w/u/d=move, i=inventory, get/take <item>, use <item>, attack, q): ";
        string cmd, action, item;
        cin >> cmd;
        
        message = "";
        
        if (cmd == "q") break;
        else if (cmd == "i") {
            message = "Inventory: ";
            for (string& it : inventory) message += it + " ";
        }
        else if (cmd == "get" || cmd == "take") {
            cin >> item;
            int found = -1;
            for (int i = 0; i < rooms[currentRoom].itemCount; i++) {
                if (rooms[currentRoom].items[i] == item) found = i;
            }
            if (found >= 0) {
                inventory.push_back(rooms[currentRoom].items[found]);
                for (int i = found; i < rooms[currentRoom].itemCount - 1; i++)
                    rooms[currentRoom].items[i] = rooms[currentRoom].items[i+1];
                rooms[currentRoom].itemCount--;
                message = "You picked up " + item;
            } else {
                message = "Item not found!";
            }
        }
        else if (cmd == "use") {
            cin >> item;
            bool hasItem = false;
            for (string& it : inventory) if (it == item) hasItem = true;
            if (!hasItem) { message = "You don't have that!"; }
            else if (item == "Key") {
                for (int i = 0; i < rooms[currentRoom].exitCount; i++) {
                    if (rooms[currentRoom].locked[i]) {
                        rooms[currentRoom].locked[i] = 0;
                        message = "You unlocked the " + rooms[currentRoom].exits[i] + " door!";
                        break;
                    }
                }
                if (message == "") message = "No locked doors here";
            }
            else if (item == "Knife") {
                if (rooms[currentRoom].enemyHealth > 0) {
                    rooms[currentRoom].enemyHealth -= 25;
                    message = "You attacked " + rooms[currentRoom].enemyName + "!";
                    if (rooms[currentRoom].enemyHealth <= 0) {
                        message += " It was defeated!";
                    } else {
                        health -= rooms[currentRoom].enemyDamage;
                        message += " It fights back! -" + to_string(rooms[currentRoom].enemyDamage) + " HP";
                    }
                } else message = "No enemy to attack";
            }
            else message = "Can't use that here";
        }
        else if (cmd == "attack" || cmd == "fight") {
            if (rooms[currentRoom].enemyHealth > 0) {
                bool hasKnife = false;
                for (string& it : inventory) if (it == "Knife") hasKnife = true;
                if (hasKnife) {
                    rooms[currentRoom].enemyHealth -= 25;
                    message = "You attacked with Knife!";
                } else {
                    rooms[currentRoom].enemyHealth -= 10;
                    message = "You attacked with bare hands!";
                }
                if (rooms[currentRoom].enemyHealth <= 0) {
                    message += " Enemy defeated!";
                } else {
                    health -= rooms[currentRoom].enemyDamage;
                    message += " Enemy fights back! -" + to_string(rooms[currentRoom].enemyDamage) + " HP";
                }
            } else message = "No enemy here";
        }
        else {
            bool moved = false;
            for (int i = 0; i < rooms[currentRoom].exitCount; i++) {
                if (cmd == rooms[currentRoom].exits[i]) {
                    if (rooms[currentRoom].locked[i]) {
                        message = "That door is locked!";
                    } else {
                        for (int r = 0; r < 7; r++) {
                            if (rooms[currentRoom].exitDest[i] == rooms[r].name) {
                                currentRoom = r;
                                moved = true;
                                message = "You entered " + rooms[r].name;
                                if (rooms[r].enemyHealth > 0) {
                                    message += ". " + rooms[r].enemyName + " attacks!";
                                }
                                break;
                            }
                        }
                    }
                    break;
                }
            }
            if (!moved && message == "") message = "Can't go that way!";
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 14. 3x3 Slide Puzzle ========================
void play3x3Puzzle() {
    vector<string> board = {"1","2","3","4","5","6","7","8"," "};
    int emptyIdx = 8;
    int moves = 0;
    
    for (int i = 0; i < 100; i++) {
        int row = emptyIdx / 3, col = emptyIdx % 3;
        vector<int> valid;
        if (row > 0) valid.push_back(emptyIdx - 3);
        if (row < 2) valid.push_back(emptyIdx + 3);
        if (col > 0) valid.push_back(emptyIdx - 1);
        if (col < 2) valid.push_back(emptyIdx + 1);
        
        int r = valid[random_int(valid.size())];
        swap(board[emptyIdx], board[r]);
        emptyIdx = r;
    }
    
    while (1) {
        clear_screen();
        cout << "=== 3x3 Slide Puzzle ===" << endl;
        cout << "Moves: " << moves << endl << endl;
        
        for (int i = 0; i < 3; i++) {
            cout << "+---+---+---+\n|";
            for (int j = 0; j < 3; j++) {
                string val = board[i*3+j];
                if (val == " ") cout << " . |";
                else cout << " " << val << " |";
            }
            cout << endl;
        }
        cout << "+---+---+---+" << endl;
        
        bool won = true;
        vector<string> expected = {"1","2","3","4","5","6","7","8"," "};
        for (int i = 0; i < 9; i++) if (board[i] != expected[i]) { won = false; break; }
        
        if (won) { cout << "\nYOU WIN! Moves: " << moves << endl; break; }
        
        cout << "\nWASD to move, q to quit: ";
        string cmd; cin >> cmd;
        if (cmd == "q") break;
        
        int row = emptyIdx / 3, col = emptyIdx % 3;
        int newIdx = emptyIdx;
        
        if (cmd == "w" && row > 0) newIdx = emptyIdx - 3;
        else if (cmd == "s" && row < 2) newIdx = emptyIdx + 3;
        else if (cmd == "a" && col > 0) newIdx = emptyIdx - 1;
        else if (cmd == "d" && col < 2) newIdx = emptyIdx + 1;
        
        if (newIdx != emptyIdx) {
            swap(board[emptyIdx], board[newIdx]);
            emptyIdx = newIdx;
            moves++;
        }
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 15. Number Position ========================
void playNumberPosition() {
    vector<int> digits = {0,1,2,3,4,5,6,7,8,9};
    for (int i = 0; i < 10; i++) {
        int r = random_int(10);
        swap(digits[i], digits[r]);
    }
    string secret = "";
    for (int i = 0; i < 4; i++) secret += ('0' + digits[i]);
    
    int attempts = 0, maxAttempts = 10;
    string message = "Guess 4 unique digits (e.g., 1234):";
    
    while (attempts < maxAttempts) {
        clear_screen();
        cout << "=== Number Position ===" << endl;
        cout << "Attempts: " << attempts << "/" << maxAttempts << endl << endl;
        cout << "Rules: A = correct digit & position, B = correct digit wrong position" << endl << endl;
        cout << message << endl;
        
        string guess; cin >> guess;
        if (guess == "q") break;
        
        if (guess.length() != 4 || 
            guess[0]==guess[1] || guess[0]==guess[2] || guess[0]==guess[3] ||
            guess[1]==guess[2] || guess[1]==guess[3] || guess[2]==guess[3] ||
            !isdigit(guess[0])) {
            message = "Need 4 unique digits! Try again:";
            continue;
        }
        
        int a = 0, b = 0;
        for (int i = 0; i < 4; i++) {
            if (guess[i] == secret[i]) a++;
            else for (int j = 0; j < 4; j++) 
                if (guess[i] == secret[j]) { b++; break; }
        }
        
        if (a == 4) { cout << "\nCORRECT! You won in " << attempts+1 << " attempts!" << endl; break; }
        message = guess + ": " + to_string(a) + "A" + to_string(b) + "B\nTry again:";
        attempts++;
    }
    if (attempts >= maxAttempts) cout << "\nGAME OVER! Answer: " << secret << endl;
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 16. Number Sort ========================
void playNumberSort() {
    vector<int> nums = {1,2,3,4};
    int swaps[4][2] = {{0,1},{1,2},{2,3},{0,3}};
    for (int i = 0; i < 10; i++) {
        int idx = random_int(4);
        int a = swaps[idx][0], b = swaps[idx][1];
        swap(nums[a], nums[b]);
    }
    
    int steps = 0;
    vector<int> sorted = nums;
    sort(sorted.begin(), sorted.end());
    int minSteps = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3-i-1; j++) {
            if (sorted[j] > sorted[j+1]) {
                int tmp = sorted[j];
                sorted[j] = sorted[j+1];
                sorted[j+1] = tmp;
                minSteps++;
            }
        }
    }
    
    string message = "Enter positions to swap (e.g., 1 2):";
    
    while (1) {
        clear_screen();
        cout << "=== Number Sort ===" << endl;
        cout << "Sort to: 1 2 3 4" << endl;
        cout << "Current: " << nums[0] << " " << nums[1] << " " << nums[2] << " " << nums[3] << endl;
        cout << "Steps: " << steps << " (best: " << minSteps << ")" << endl << endl;
        cout << message << endl;
        
        bool sortedNow = true;
        for (int i = 0; i < 3; i++) if (nums[i] > nums[i+1]) sortedNow = false;
        if (sortedNow) { cout << "\nYOU WIN! Steps: " << steps << " (best: " << minSteps << ")" << endl; break; }
        
        string input; cin >> input;
        if (input == "q") break;
        
        int p1, p2;
        stringstream ss(input);
        if (!(ss >> p1 >> p2) || p1 < 1 || p1 > 4 || p2 < 1 || p2 > 4) {
            message = "Invalid! Format: 1 2 (swap pos 1 and 2)";
            continue;
        }
        p1--; p2--;
        if (abs(p1-p2) != 1) {
            message = "Must swap adjacent positions!";
            continue;
        }
        swap(nums[p1], nums[p2]);
        steps++;
    }
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 17. Word Spelling ========================
void playWordSpelling() {
    vector<string> words = {"apple", "banana", "computer", "driver", "elephant", "game", "have", "in", "just", "know", "light", "mouse", "not", "off", "play", "quick", "read", "sun", "time", "you"};
    int score = 0, round = 0, maxRounds = 10;
    string message = "Press Enter to start!";
    
    while (round < maxRounds) {
        clear_screen();
        cout << "=== Word Spelling ===" << endl;
        cout << "Score: " << score << "/" << maxRounds << "  Round: " << round+1 << "/" << maxRounds << endl << endl;
        
        if (round == 0) {
            cout << message << endl;
            cin.get();
        }
        
        string current = words[random_int(words.size())];
        cout << "Spell this word: " << toUpper(current) << endl;
        cout << "Your answer: ";
        
        string input; cin >> input;
        if (input == "q") break;
        
        if (input == current) {
            score++;
            message = "CORRECT! Score: " + to_string(score);
        } else {
            message = "WRONG! Answer: " + toUpper(current);
        }
        round++;
    }
    cout << "\nGAME OVER! Final Score: " << score << "/" << maxRounds << endl;
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 18. Color Match ========================
void playColorMatch() {
    vector<string> colorNames = {"Red", "Green", "Yellow", "Blue", "Purple", "Cyan"};
    vector<string> colorCodes = {"\033[91m", "\033[92m", "\033[93m", "\033[94m", "\033[95m", "\033[96m"};
    int score = 0, round = 0, maxRounds = 10;
    string message = "Press Enter to start!";
    
    while (round < maxRounds) {
        clear_screen();
        cout << "=== Color Match ===" << endl;
        cout << "Score: " << score << "/" << maxRounds << "  Round: " << round+1 << "/" << maxRounds << endl << endl;
        
        if (round == 0) {
            cout << message << endl;
            cout << "Match the COLOR of the text, not the word!" << endl;
            cin.get();
        }
        
        int colorIdx = random_int(6);
        int wordIdx = random_int(6);
        if (random_int(2) == 0) wordIdx = colorIdx;
        
        cout << "Text color: " << colorCodes[colorIdx] << colorNames[wordIdx] << "\033[0m" << endl;
        cout << "Enter color (Red/Green/Yellow/Blue/Purple/Cyan) or q: ";
        
        string input; cin >> input;
        if (input == "q") break;
        
        bool correct = false;
        for (char& c : input) c = toupper(c);
        for (char& c : colorNames[colorIdx]) c = toupper(c);
        if (input == colorNames[colorIdx]) correct = true;
        
        if (correct) {
            score++;
            message = "CORRECT!";
        } else {
            message = "WRONG! It was " + colorNames[colorIdx];
        }
        round++;
    }
    cout << "\nGAME OVER! Final Score: " << score << "/" << maxRounds << endl;
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== 19. Shape Match ========================
void playShapeMatch() {
    vector<string> shapes = {"A", "B", "C", "D", "E", "F"};
    vector<vector<int>> board(8, vector<int>(8));
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = random_int(6);
    
    int selectedR = -1, selectedC = -1;
    int score = 0;
    
    while (1) {
        clear_screen();
        cout << "=== Shape Match ===" << endl;
        cout << "Score: " << score << endl << endl;
        
        cout << "  ";
        for (int j = 0; j < 8; j++) cout << " " << j+1 << " ";
        cout << endl;
        for (int i = 0; i < 8; i++) {
            cout << i+1 << " ";
            for (int j = 0; j < 8; j++) {
                if (i == selectedR && j == selectedC) cout << "[" << shapes[board[i][j]] << "]";
                else cout << " " << shapes[board[i][j]] << " ";
            }
            cout << endl;
        }
        
        int matches = 0;
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 7; j++)
                if (board[i][j] == board[i][j+1] && board[i][j] >= 0) matches++;
        
        if (matches == 0) {
            cout << "\nALL CLEARED! YOU WIN! Score: " << score << endl;
            break;
        }
        
        cout << "\nEnter row col (e.g., 1 1) or q: ";
        string input; cin >> input;
        if (input == "q") break;
        
        int r, c;
        stringstream ss(input);
        if (!(ss >> r >> c) || r < 1 || r > 8 || c < 1 || c > 8) {
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
    cout << "Press Enter..."; cin.get(); cin.get();
}

// ======================== Main Menu ========================
void showMenu() {
    clear_screen();
    cout << "========== GAME MENU ==========" << endl;
    cout << "1. 100-Floor Tower" << endl;
    cout << "2. Snake" << endl;
    cout << "3. Guess Number" << endl;
    cout << "4. Rock Paper Scissors" << endl;
    cout << "5. Tic-Tac-Toe" << endl;
    cout << "6. Memory Card" << endl;
    cout << "7. 4x4 Slide Puzzle" << endl;
    cout << "8. Maze" << endl;
    cout << "9. Sudoku" << endl;
    cout << "10. Minesweeper" << endl;
    cout << "11. 2048" << endl;
    cout << "12. Gomoku" << endl;
    cout << "13. Text Adventure" << endl;
    cout << "14. 3x3 Slide Puzzle" << endl;
    cout << "15. Number Position" << endl;
    cout << "16. Number Sort" << endl;
    cout << "17. Word Spelling" << endl;
    cout << "18. Color Match" << endl;
    cout << "19. Shape Match" << endl;
    cout << "q. Quit" << endl;
    cout << "\nSelect game: ";
}

int main() {
    srand(time(NULL));
    
    while (1) {
        showMenu();
        string choice;
        cin >> choice;
        
        if (choice == "q") { cout << "Goodbye!" << endl; break; }
        
        if (choice == "1") playTowerGame();
        else if (choice == "2") playSnakeGame();
        else if (choice == "3") playGuessNumberGame();
        else if (choice == "4") playRockPaperScissors();
        else if (choice == "5") playTicTacToe();
        else if (choice == "6") playMemoryGame();
        else if (choice == "7") playSlidePuzzle();
        else if (choice == "8") playMazeGame();
        else if (choice == "9") playSudokuGame();
        else if (choice == "10") playMinesweeperGame();
        else if (choice == "11") play2048Game();
        else if (choice == "12") playGomokuGame();
        else if (choice == "13") playTextAdventure();
        else if (choice == "14") play3x3Puzzle();
        else if (choice == "15") playNumberPosition();
        else if (choice == "16") playNumberSort();
        else if (choice == "17") playWordSpelling();
        else if (choice == "18") playColorMatch();
        else if (choice == "19") playShapeMatch();
        else { cout << "Invalid! Press Enter..."; cin.get(); cin.get(); }
    }
    
    return 0;
}
