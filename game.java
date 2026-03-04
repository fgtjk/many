import java.util.*;

public class Game {
    private static Scanner scanner = new Scanner(System.in);
    private static Random random = new Random();
    
    public static void main(String[] args) {
        while (true) {
            showMainMenu();
            String choice = scanner.nextLine().trim();
            
            switch (choice) {
                case "1": towerGame(); break;
                case "2": snakeGame(); break;
                case "3": guessNumberGame(); break;
                case "4": rockPaperScissorsGame(); break;
                case "5": ticTacToeGame(); break;
                case "6": memoryGame(); break;
                case "7": slidePuzzleGame(); break;
                case "8": mazeGame(); break;
                case "9": sudokuGame(); break;
                case "10": minesweeperGame(); break;
                case "11": game2048(); break;
                case "12": gomokuGame(); break;
                case "13": textAdventureGame(); break;
                case "14": numberPuzzleGame(); break;
                case "15": numberPositionGame(); break;
                case "16": numberSortGame(); break;
                case "17": wordSpellingGame(); break;
                case "18": colorMatchGame(); break;
                case "19": shapeEliminationGame(); break;
                case "q": case "Q": 
                    System.out.println("Goodbye!");
                    return;
                default: 
                    System.out.println("Invalid choice, press Enter to continue...");
                    scanner.nextLine();
            }
        }
    }
    
    static void showMainMenu() {
        clearScreen();
        System.out.println("========== GAME MENU ==========");
        System.out.println("1. 100-Floor Tower");
        System.out.println("2. Snake");
        System.out.println("3. Guess Number");
        System.out.println("4. Rock Paper Scissors");
        System.out.println("5. Tic-Tac-Toe");
        System.out.println("6. Memory Card");
        System.out.println("7. 4x4 Slide Puzzle");
        System.out.println("8. Maze");
        System.out.println("9. Sudoku");
        System.out.println("10. Minesweeper");
        System.out.println("11. 2048");
        System.out.println("12. Gomoku");
        System.out.println("13. Text Adventure");
        System.out.println("14. 3x3 Slide Puzzle");
        System.out.println("15. Number Position");
        System.out.println("16. Number Sort");
        System.out.println("17. Word Spelling");
        System.out.println("18. Color Match");
        System.out.println("19. Shape Match");
        System.out.println("q. Quit");
        System.out.print("\nSelect game: ");
    }
    
    static void clearScreen() {
        for (int i = 0; i < 40; i++) System.out.println();
    }
    
    // ======================== 1. Tower ========================
    static void towerGame() {
        GameState state = new GameState();
        
        while (true) {
            renderTower(state);
            System.out.print("> ");
            String cmd = scanner.nextLine().toLowerCase().trim();
            
            if (cmd.equals("q")) break;
            
            switch (cmd) {
                case "w": state.movePlayer(0, -1); break;
                case "s": state.movePlayer(0, 1); break;
                case "a": state.movePlayer(-1, 0); break;
                case "d": state.movePlayer(1, 0); break;
                default: state.message = "Invalid! Use w,a,s,d to move";
            }
            
            if (state.gameOver) break;
        }
    }
    
    static void renderTower(GameState state) {
        clearScreen();
        System.out.println("=== 100-Floor Tower ===");
        System.out.printf("Floor: %d | HP: %d/%d | ATK: %d | DEF: %d | Gold: %d\n",
            state.floor, state.hp, state.maxHp, state.attack, state.defense, state.gold);
        System.out.printf("Keys: Y[%d] B[%d] R[%d]\n", state.keys[0], state.keys[1], state.keys[2]);
        if (state.hasSword) System.out.print("[Sword] ");
        if (state.hasShield) System.out.print("[Shield] ");
        System.out.println();
        
        int[][] map = state.floors.get(state.floor);
        if (map == null) return;
        
        System.out.println("\n  0123456789");
        System.out.println(" +----------+");
        for (int y = 0; y < 10; y++) {
            System.out.print(y + "|");
            for (int x = 0; x < 10; x++) {
                if (x == state.playerX && y == state.playerY) System.out.print("@");
                else {
                    char tile = " .#SGEBODM".charAt(map[y][x]);
                    System.out.print(tile);
                }
            }
            System.out.println("|");
        }
        System.out.println(" +----------+\n");
        System.out.println(state.message);
        System.out.println("w/a/s/d to move, q to quit");
    }
    
    static class GameState {
        int hp = 200, maxHp = 200, attack = 20, defense = 20, gold = 0, floor = 1;
        int[] keys = {1, 0, 0};
        int playerX = 5, playerY = 5;
        boolean hasSword = false, hasShield = false, gameOver = false;
        String message = "";
        Map<Integer, int[][]> floors = new HashMap<>();
        
        GameState() { generateFloors(); }
        
        void generateFloors() {
            for (int f = 1; f <= 100; f++) {
                int[][] map = new int[10][10];
                for (int i = 0; i < 10; i++) map[i][0] = map[i][9] = map[0][i] = map[9][i] = 1;
                
                if (f == 1) { map[5][5] = 0; map[7][3] = 4; map[8][8] = 2; }
                else if (f == 100) { map[5][5] = 6; map[5][4] = 5; }
                else {
                    map[random.nextInt(8)+1][random.nextInt(8)+1] = 4;
                    if (f > 1) map[random.nextInt(8)+1][random.nextInt(8)+1] = 5;
                    int enemyCount = Math.min(8, 2 + f / 15);
                    for (int i = 0; i < enemyCount; i++) {
                        int x = random.nextInt(8) + 1, y = random.nextInt(8) + 1;
                        if (map[y][x] == 0) map[y][x] = f < 30 ? 2 : (f < 70 ? 7 : 8);
                    }
                    for (int i = 0; i < 3 + random.nextInt(3); i++) {
                        int x = random.nextInt(8) + 1, y = random.nextInt(8) + 1;
                        if (map[y][x] == 0) map[y][x] = 3;
                    }
                }
                floors.put(f, map);
            }
        }
        
        void movePlayer(int dx, int dy) {
            int nx = playerX + dx, ny = playerY + dy;
            int[][] map = floors.get(floor);
            if (nx < 0 || ny < 0 || nx >= 10 || ny >= 10) { message = "Cannot move that way!"; return; }
            int tile = map[ny][nx];
            
            switch (tile) {
                case 0: playerX = nx; playerY = ny; message = ""; break;
                case 1: message = "Hit a wall!"; break;
                case 2: fightEnemy(nx, ny, 30 + floor * 2, 10 + floor / 3, 5 + floor / 5, 10 + floor); break;
                case 3: pickItem(nx, ny); break;
                case 4: changeFloor(1); break;
                case 5: changeFloor(-1); break;
                case 6: fightBoss(nx, ny); break;
                case 7: fightEnemy(nx, ny, 80 + floor * 2, 20 + floor / 2, 10 + floor / 4, 30 + floor * 2); break;
                case 8: fightEnemy(nx, ny, 120 + floor * 3, 30 + floor / 2, 15 + floor / 3, 50 + floor * 3); break;
            }
        }
        
        void fightEnemy(int x, int y, int eHp, int eAtk, int eDef, int eGold) {
            int dmgToEnemy = Math.max(1, attack - eDef);
            int dmgToPlayer = Math.max(1, eAtk - defense);
            int hits = (eHp + dmgToEnemy - 1) / dmgToEnemy;
            hp -= dmgToPlayer * (hits - 1);
            
            if (hp > 0) {
                floors.get(floor)[y][x] = 0;
                gold += eGold;
                playerX = x; playerY = y;
                message = "Defeated enemy! Got " + eGold + " gold, lost " + (dmgToPlayer * (hits - 1)) + " HP";
            } else { message = "You were defeated! Game over."; gameOver = true; }
        }
        
        void fightBoss(int x, int y) {
            if (!hasSword) { message = "You need the legendary sword to hurt the Boss!"; return; }
            int bossHp = 500, bossAtk = 50, bossDef = 30, bossGold = 1000;
            int dmgToBoss = Math.max(1, attack * 2 - bossDef);
            int dmgToPlayer = Math.max(1, bossAtk - defense);
            int hits = (bossHp + dmgToBoss - 1) / dmgToBoss;
            hp -= dmgToPlayer * (hits - 1);
            
            if (hp > 0) {
                floors.get(floor)[y][x] = 0;
                gold += bossGold;
                playerX = x; playerY = y;
                message = "Defeated Boss! Got " + bossGold + " gold\nCongratulations! You cleared 100 floors!";
                gameOver = true;
            } else { message = "You were defeated by Boss! Game over."; gameOver = true; }
        }
        
        void pickItem(int x, int y) {
            String itemType;
            if (floor == 25 && !hasSword) itemType = "legendary_sword";
            else if (floor == 50 && !hasShield) itemType = "magic_shield";
            else {
                double r = random.nextDouble();
                if (r < 0.2) itemType = "yellow_key";
                else if (r < 0.3) itemType = "blue_key";
                else if (r < 0.35) itemType = "red_key";
                else if (r < 0.85) itemType = "potion";
                else itemType = "super_potion";
            }
            
            switch (itemType) {
                case "yellow_key": keys[0]++; message = "Got 1 Yellow Key!"; break;
                case "blue_key": keys[1]++; message = "Got 1 Blue Key!"; break;
                case "red_key": keys[2]++; message = "Got 1 Red Key!"; break;
                case "potion": hp = Math.min(maxHp, hp + 50); message = "Recovered 50 HP!"; break;
                case "super_potion": hp = Math.min(maxHp, hp + 120); message = "Recovered 120 HP!"; break;
                case "legendary_sword": attack += 30; hasSword = true; message = "Got Legendary Sword! ATK +30"; break;
                case "magic_shield": defense += 20; hasShield = true; message = "Got Magic Shield! DEF +20"; break;
            }
            floors.get(floor)[y][x] = 0;
            playerX = x; playerY = y;
        }
        
        void changeFloor(int dir) {
            int newFloor = floor + dir;
            if (newFloor >= 1 && newFloor <= 100) {
                floor = newFloor;
                message = "Now at Floor " + floor;
                playerX = random.nextInt(8) + 1;
                playerY = random.nextInt(8) + 1;
            } else { message = "Cannot leave the tower!"; }
        }
    }
    
    // ======================== 2. Snake ========================
    static void snakeGame() {
        SnakeGame snake = new SnakeGame();
        while (true) {
            snake.render();
            System.out.print("> ");
            String cmd = scanner.nextLine().toLowerCase().trim();
            if (cmd.equals("q")) break;
            if (!snake.gameOver) {
                switch (cmd) {
                    case "w": snake.changeDirection(0, -1); break;
                    case "s": snake.changeDirection(0, 1); break;
                    case "a": snake.changeDirection(-1, 0); break;
                    case "d": snake.changeDirection(1, 0); break;
                }
                snake.update();
            } else { scanner.nextLine(); break; }
        }
    }
    
    static class SnakeGame {
        int width = 20, height = 15;
        List<int[]> snake = new ArrayList<>();
        int[] dir = {1, 0};
        int[] food;
        int score = 0;
        boolean gameOver = false;
        String message = "Use WASD to control, Q to quit";
        
        SnakeGame() { snake.add(new int[]{width/2, height/2}); food = generateFood(); }
        
        int[] generateFood() {
            while (true) {
                int[] f = {random.nextInt(width), random.nextInt(height)};
                boolean ok = true;
                for (int[] s : snake) if (s[0] == f[0] && s[1] == f[1]) { ok = false; break; }
                if (ok) return f;
            }
        }
        
        void changeDirection(int dx, int dy) { if (dx != -dir[0] || dy != -dir[1]) { dir[0] = dx; dir[1] = dy; } }
        
        void update() {
            int[] head = snake.get(0);
            int nx = (head[0] + dir[0] + width) % width;
            int ny = (head[1] + dir[1] + height) % height;
            
            for (int[] s : snake) {
                if (s[0] == nx && s[1] == ny) { gameOver = true; message = "Game Over! Score: " + score; return; }
            }
            
            snake.add(0, new int[]{nx, ny});
            if (nx == food[0] && ny == food[1]) { score++; food = generateFood(); }
            else { snake.remove(snake.size() - 1); }
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Snake Game ===");
            System.out.println("Score: " + score);
            System.out.println(message + "\n");
            
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (x == food[0] && y == food[1]) System.out.print("$ ");
                    else if (x == snake.get(0)[0] && y == snake.get(0)[1]) System.out.print("@ ");
                    else {
                        boolean isSnake = false;
                        for (int[] s : snake) if (s[0] == x && s[1] == y) { isSnake = true; break; }
                        System.out.print(isSnake ? "O " : ". ");
                    }
                }
                System.out.println();
            }
            System.out.println("\nControls: w(up) a(left) s(down) d(right) q(quit)");
        }
    }
    
    // ======================== 3. Guess Number ========================
    static void guessNumberGame() {
        GuessNumberGame game = new GuessNumberGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeGuess(input);
        }
    }
    
    static class GuessNumberGame {
        int number = random.nextInt(100) + 1;
        int attempts = 0, maxAttempts = 10;
        boolean gameOver = false;
        String message = "Guess a number 1-100, you have " + maxAttempts + " attempts";
        
        void makeGuess(String guess) {
            try {
                int num = Integer.parseInt(guess);
                attempts++;
                if (num < number) message = "Too small! " + (maxAttempts - attempts) + " attempts left";
                else if (num > number) message = "Too big! " + (maxAttempts - attempts) + " attempts left";
                else { message = "Correct! You got it in " + attempts + " tries!"; gameOver = true; }
                if (attempts >= maxAttempts && !gameOver) { message = "Game over! Answer was " + number; gameOver = true; }
            } catch (Exception e) { message = "Please enter a valid number!"; }
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Guess Number ===");
            System.out.println(message);
            if (!gameOver) System.out.println("\nEnter your guess (1-100), or q to quit:");
            else System.out.println("\nPress Enter to return...");
        }
    }
    
    // ======================== 4. Rock Paper Scissors ========================
    static void rockPaperScissorsGame() {
        RockPaperScissors game = new RockPaperScissors();
        while (true) {
            game.render();
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.playRound(input);
        }
    }
    
    static class RockPaperScissors {
        String[] choices = {"Rock", "Scissors", "Paper"};
        int playerScore = 0, computerScore = 0, rounds = 0;
        String message = "Enter 1(Rock), 2(Scissors), 3(Paper) or q(quit):";
        
        void playRound(String input) {
            if (input.equalsIgnoreCase("q")) return;
            try {
                int playerIdx = Integer.parseInt(input) - 1;
                if (playerIdx < 0 || playerIdx > 2) { message = "Please enter 1-3!"; return; }
                
                int computerIdx = random.nextInt(3);
                rounds++;
                
                int result = (playerIdx - computerIdx + 3) % 3;
                if (result == 2) playerScore++;
                else if (result == 1) computerScore++;
                
                message = "You: " + choices[playerIdx] + ", Computer: " + choices[computerIdx] + "\n";
                if (result == 0) message += "Result: Draw\n";
                else if (result == 2) message += "Result: You Win\n";
                else message += "Result: Computer Wins\n";
                message += "Score: Player " + playerScore + " - " + computerScore + " Computer\n";
                message += "Enter 1(Rock), 2(Scissors), 3(Paper) or q(quit):";
            } catch (Exception e) { message = "Please enter valid number(1-3) or q!"; }
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Rock Paper Scissors ===");
            System.out.println("Round: " + rounds);
            System.out.println("Score: Player " + playerScore + " - " + computerScore + " Computer");
            System.out.println("\n1. Rock\n2. Scissors\n3. Paper");
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 5. Tic Tac Toe ========================
    static void ticTacToeGame() {
        TicTacToeGame game = new TicTacToeGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeMove(input);
        }
    }
    
    static class TicTacToeGame {
        char[] board = new char[9];
        char currentPlayer = 'X';
        boolean gameOver = false;
        String message = "Enter position (1-9) or q to quit:";
        
        TicTacToeGame() { Arrays.fill(board, ' '); }
        
        void makeMove(String pos) {
            if (pos.equalsIgnoreCase("q")) return;
            try {
                int p = Integer.parseInt(pos) - 1;
                if (p < 0 || p > 8) { message = "Please enter 1-9!"; return; }
                if (board[p] != ' ') { message = "Position occupied!"; return; }
                
                board[p] = currentPlayer;
                if (checkWinner()) { gameOver = true; message = "Player " + currentPlayer + " wins! Press Enter..."; }
                else if (!new String(board).contains(" ")) { gameOver = true; message = "Draw! Press Enter..."; }
                else { currentPlayer = currentPlayer == 'X' ? 'O' : 'X'; message = "Player " + currentPlayer + "'s turn, enter position (1-9):"; }
            } catch (Exception e) { message = "Please enter valid number!"; }
        }
        
        boolean checkWinner() {
            int[][] win = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
            for (int[] w : win) if (board[w[0]] != ' ' && board[w[0]] == board[w[1]] && board[w[1]] == board[w[2]]) return true;
            return false;
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Tic-Tac-Toe ===");
            System.out.println("Positions: 1|2|3  4|5|6  7|8|9\n");
            for (int i = 0; i < 9; i += 3) {
                System.out.println(" " + board[i] + " | " + board[i+1] + " | " + board[i+2]);
                if (i < 6) System.out.println("-----------");
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 6. Memory Card ========================
    static void memoryGame() {
        MemoryGame game = new MemoryGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.flipCard(input);
        }
    }
    
    static class MemoryGame {
        char[] symbols = {'A','B','C','D','E','F','G','H','A','B','C','D','E','F','G','H'};
        char[] board = new char[16];
        boolean[] matched = new boolean[16];
        Integer firstCard = null, secondCard = null;
        int attempts = 0;
        boolean gameOver = false;
        String message = "Select card to flip (1-16) or q to quit:";
        
        MemoryGame() {
            List<Character> list = new ArrayList<>();
            for (char c : symbols) list.add(c);
            Collections.shuffle(list);
            for (int i = 0; i < 16; i++) { board[i] = '?'; symbols[i] = list.get(i); }
        }
        
        void flipCard(String pos) {
            if (pos.equalsIgnoreCase("q")) return;
            try {
                int p = Integer.parseInt(pos) - 1;
                if (p < 0 || p > 15) { message = "Please enter 1-16!"; return; }
                if (matched[p]) { message = "Card already matched!"; return; }
                if (firstCard != null && p == firstCard) { message = "Cannot select same card!"; return; }
                
                board[p] = symbols[p];
                
                if (firstCard == null) { firstCard = p; message = "Select second card:"; }
                else { secondCard = p; attempts++; checkMatch(); }
            } catch (Exception e) { message = "Please enter valid number!"; }
        }
        
        void checkMatch() {
            if (symbols[firstCard] == symbols[secondCard]) {
                matched[firstCard] = true; matched[secondCard] = true;
                message = "Match!";
                if (allMatched()) { gameOver = true; message = "Congratulations! " + attempts + " attempts. Press Enter..."; }
            } else { message = "No match!"; }
            
            render(); System.out.println("Press Enter to continue..."); scanner.nextLine();
            
            if (!gameOver) { board[firstCard] = '?'; board[secondCard] = '?'; firstCard = null; secondCard = null; }
        }
        
        boolean allMatched() { for (boolean m : matched) if (!m) return false; return true; }
        
        void render() {
            clearScreen();
            System.out.println("=== Memory Card ===");
            System.out.println("\nAttempts: " + attempts);
            for (int i = 0; i < 16; i += 4) {
                for (int j = 0; j < 4; j++) {
                    System.out.print((matched[i+j] || firstCard == i+j || secondCard == i+j) ? symbols[i+j] + " " : String.format("%2d ", i+j+1));
                    if (j < 3) System.out.print("|");
                }
                System.out.println();
                if (i < 12) System.out.println("---------------------");
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 7. 4x4 Slide Puzzle ========================
    static void slidePuzzleGame() {
        SlidePuzzleGame game = new SlidePuzzleGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.moveTile(input);
        }
    }
    
    static class SlidePuzzleGame {
        int[] board = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};
        int emptyPos = 15;
        int moves = 0;
        boolean gameOver = false;
        String message = "Use WASD to move or q to quit:";
        
        SlidePuzzleGame() {
            List<Integer> list = new ArrayList<>();
            for (int i = 1; i < 16; i++) list.add(i); list.add(0);
            Collections.shuffle(list);
            for (int i = 0; i < 16; i++) { board[i] = list.get(i); if (board[i] == 0) emptyPos = i; }
        }
        
        void moveTile(String dir) {
            dir = dir.toLowerCase();
            if (dir.equals("q")) return;
            
            int row = emptyPos / 4, col = emptyPos % 4, newPos = -1;
            
            if (dir.equals("w") && row > 0) newPos = emptyPos - 4;
            else if (dir.equals("s") && row < 3) newPos = emptyPos + 4;
            else if (dir.equals("a") && col > 0) newPos = emptyPos - 1;
            else if (dir.equals("d") && col < 3) newPos = emptyPos + 1;
            else { message = "Invalid move! Use WASD or q:"; return; }
            
            board[emptyPos] = board[newPos];
            board[newPos] = 0;
            emptyPos = newPos;
            moves++;
            
            if (isSolved()) { gameOver = true; message = "Congratulations! " + moves + " moves. Press Enter..."; }
            else { message = "Moves: " + moves + " Use WASD or q:"; }
        }
        
        boolean isSolved() { for (int i = 0; i < 15; i++) if (board[i] != i + 1) return false; return board[15] == 0; }
        
        void render() {
            clearScreen();
            System.out.println("=== 4x4 Slide Puzzle ===\nArrange numbers in order (1-15):\n");
            for (int i = 0; i < 16; i += 4) {
                for (int j = 0; j < 4; j++) {
                    int v = board[i + j];
                    System.out.print(v == 0 ? "    " : String.format("%4d", v));
                }
                System.out.println();
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 8. Maze ========================
    static void mazeGame() {
        MazeGame game = new MazeGame();
        while (true) {
            game.render();
            if (game.win) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.movePlayer(input);
        }
    }
    
    static class MazeGame {
        int width = 15, height = 15;
        int[] playerPos = {1, 1};
        int[] exitPos = {13, 13};
        int[][] maze;
        boolean win = false;
        String message = "Use WASD to move, q to quit:";
        
        MazeGame() {
            maze = new int[height][width];
            for (int[] row : maze) Arrays.fill(row, 1);
            generateMaze();
            maze[exitPos[1]][exitPos[0]] = 0;
        }
        
        void generateMaze() {
            List<int[]> stack = new ArrayList<>();
            stack.add(playerPos.clone());
            maze[playerPos[1]][playerPos[0]] = 0;
            int[][] dirs = {{0,2},{2,0},{0,-2},{-2,0}};
            
            while (!stack.isEmpty()) {
                int[] current = stack.get(stack.size() - 1);
                List<int[]> shuffled = new ArrayList<>(Arrays.asList(dirs));
                Collections.shuffle(shuffled);
                
                boolean found = false;
                for (int[] d : shuffled) {
                    int nx = current[0] + d[0], ny = current[1] + d[1];
                    if (nx > 0 && nx < width-1 && ny > 0 && ny < height-1 && maze[ny][nx] == 1) {
                        maze[ny][nx] = 0;
                        maze[current[1] + d[1]/2][current[0] + d[0]/2] = 0;
                        stack.add(new int[]{nx, ny});
                        found = true;
                        break;
                    }
                }
                if (!found) stack.remove(stack.size() - 1);
            }
        }
        
        void movePlayer(String dir) {
            dir = dir.toLowerCase();
            if (dir.equals("q")) return;
            
            int nx = playerPos[0], ny = playerPos[1];
            if (dir.equals("w")) ny--; else if (dir.equals("s")) ny++;
            else if (dir.equals("a")) nx--; else if (dir.equals("d")) nx++;
            else { message = "Invalid input! Use WASD or q:"; return; }
            
            if (nx < 0 || ny < 0 || nx >= width || ny >= height || maze[ny][nx] == 1) { message = "Hit a wall!"; return; }
            
            playerPos[0] = nx; playerPos[1] = ny;
            if (nx == exitPos[0] && ny == exitPos[1]) { message = "Congratulations! You escaped! Press Enter..."; win = true; }
            else message = "Use WASD to move, q to quit:";
        }
        
        void render() {
            clearScreen();
            System.out.println("=== 15x15 Maze ===\nFind the exit (E)! Walls (#) cannot pass\n");
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (x == playerPos[0] && y == playerPos[1]) System.out.print("P ");
                    else if (x == exitPos[0] && y == exitPos[1]) System.out.print("E ");
                    else if (maze[y][x] == 1) System.out.print("# ");
                    else System.out.print(". ");
                }
                System.out.println();
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 9. Sudoku ========================
    static void sudokuGame() {
        SudokuGame game = new SudokuGame();
        while (true) {
            game.render();
            if (game.isComplete()) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeMove(input);
        }
    }
    
    static class SudokuGame {
        int[][] board = new int[9][9];
        int[][] original = new int[9][9];
        String message = "Enter row col number (e.g. 1 2 3) or q to quit:";
        
        SudokuGame() {
            for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) board[i][j] = ((i * 3 + i / 3 + j) % 9) + 1;
            for (int i = 0; i < 40; i++) {
                int r1 = random.nextInt(9), c1 = random.nextInt(9), r2 = random.nextInt(9), c2 = random.nextInt(9);
                int temp = board[r1][c1]; board[r1][c1] = board[r2][c2]; board[r2][c2] = temp;
            }
            for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) if (random.nextDouble() < 0.6) board[i][j] = 0;
            for (int i = 0; i < 9; i++) original[i] = board[i].clone();
        }
        
        void makeMove(String input) {
            if (input.equalsIgnoreCase("q")) return;
            try {
                String[] parts = input.split("\\s+");
                if (parts.length != 3) throw new Exception();
                int r = Integer.parseInt(parts[0]) - 1;
                int c = Integer.parseInt(parts[1]) - 1;
                int n = Integer.parseInt(parts[2]);
                if (r < 0 || r > 8 || c < 0 || c > 8 || n < 1 || n > 9) { message = "Input must be 1-9!"; return; }
                if (original[r][c] != 0) { message = "Cannot modify initial numbers!"; return; }
                if (!isValid(r, c, n)) { message = "Number " + n + " is invalid at this position!"; return; }
                board[r][c] = n;
                message = isComplete() ? "Congratulations! Sudoku solved! Press Enter..." : "Enter row col number (e.g. 1 2 3) or q to quit:";
            } catch (Exception e) { message = "Format error! Example: 1 2 3"; }
        }
        
        boolean isValid(int row, int col, int num) {
            for (int i = 0; i < 9; i++) if (board[row][i] == num) return false;
            for (int i = 0; i < 9; i++) if (board[i][col] == num) return false;
            int br = (row / 3) * 3, bc = (col / 3) * 3;
            for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) if (board[br+i][bc+j] == num) return false;
            return true;
        }
        
        boolean isComplete() { for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) if (board[i][j] == 0) return false; return true; }
        
        void render() {
            clearScreen();
            System.out.println("=== Sudoku ===\nRules: Each row/col/3x3 box contains 1-9\n");
            for (int i = 0; i < 9; i++) {
                if (i % 3 == 0 && i != 0) System.out.println("-----------------------------");
                for (int j = 0; j < 9; j++) {
                    if (j % 3 == 0 && j != 0) System.out.print("|");
                    if (original[i][j] != 0) System.out.print("*" + board[i][j] + "*");
                    else if (board[i][j] == 0) System.out.print(" . ");
                    else System.out.print(" " + board[i][j] + " ");
                }
                System.out.println();
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 10. Minesweeper ========================
    static void minesweeperGame() {
        MinesweeperGame game = new MinesweeperGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeMove(input);
        }
    }
    
    static class MinesweeperGame {
        int size = 10, mineCount = 15;
        String[][] board = new String[size][size];
        boolean[][] visible = new boolean[size][size];
        boolean gameOver = false, firstMove = true;
        String message = "Enter coordinate (e.g. A5) or q to quit:";
        
        MinesweeperGame() { for (String[] row : board) Arrays.fill(row, " "); }
        
        void placeMines(int safeR, int safeC) {
            int placed = 0;
            while (placed < mineCount) {
                int r = random.nextInt(size), c = random.nextInt(size);
                if ((r != safeR || c != safeC) && !board[r][c].equals("X")) { board[r][c] = "X"; placed++; }
            }
            for (int r = 0; r < size; r++) for (int c = 0; c < size; c++) if (!board[r][c].equals("X")) {
                int count = 0;
                for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++)
                    if (r+i >= 0 && r+i < size && c+j >= 0 && c+j < size && board[r+i][c+j].equals("X")) count++;
                if (count > 0) board[r][c] = String.valueOf(count);
            }
        }
        
        void makeMove(String input) {
            if (input.equalsIgnoreCase("q")) return;
            try {
                int c = input.charAt(0) - 'A';
                int r = Integer.parseInt(input.substring(1)) - 1;
                if (r < 0 || r >= size || c < 0 || c >= size) throw new Exception();
                
                if (firstMove) { placeMines(r, c); firstMove = false; }
                
                if (board[r][c].equals("X")) { revealAll(); gameOver = true; message = "Hit a mine! Game Over. Press Enter..."; return; }
                
                revealCells(r, c);
                if (checkWin()) { revealAll(); gameOver = true; message = "Congratulations! You won! Press Enter..."; }
                else message = "Enter coordinate (e.g. A5) or q to quit:";
            } catch (Exception e) { message = "Format error! Example: A5"; }
        }
        
        void revealCells(int r, int c) {
            if (r < 0 || r >= size || c < 0 || c >= size || visible[r][c]) return;
            visible[r][c] = true;
            if (board[r][c].equals(" ")) for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) revealCells(r+i, c+j);
        }
        
        boolean checkWin() { for (int r = 0; r < size; r++) for (int c = 0; c < size; c++) if (!board[r][c].equals("X") && !visible[r][c]) return false; return true; }
        void revealAll() { for (boolean[] row : visible) Arrays.fill(row, true); }
        
        void render() {
            clearScreen();
            System.out.println("=== Minesweeper ===\nMines: " + mineCount + "\n");
            System.out.print("   ");
            for (int i = 0; i < size; i++) System.out.print((char)('A'+i) + " ");
            System.out.println("\n  +" + "--".repeat(size) + "+");
            for (int r = 0; r < size; r++) {
                System.out.printf("%2d|", r+1);
                for (int c = 0; c < size; c++) System.out.print(visible[r][c] ? board[r][c] + " " : ". ");
                System.out.println("|");
            }
            System.out.println("  +" + "--".repeat(size) + "+\n" + message);
        }
    }
    
    // ======================== 11. 2048 ========================
    static void game2048() {
        Game2048 game = new Game2048();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.move(input);
        }
    }
    
    static class Game2048 {
        int[][] board = new int[4][4];
        int score = 0;
        boolean gameOver = false;
        String message = "Use WASD to move, q to quit:";
        
        Game2048() { addRandomTile(); addRandomTile(); }
        
        void addRandomTile() {
            List<int[]> empty = new ArrayList<>();
            for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (board[i][j] == 0) empty.add(new int[]{i, j});
            if (!empty.isEmpty()) { int[] p = empty.get(random.nextInt(empty.size())); board[p[0]][p[1]] = random.nextDouble() < 0.9 ? 2 : 4; }
        }
        
        void move(String dir) {
            dir = dir.toLowerCase();
            if (dir.equals("q")) return;
            boolean moved = false;
            
            if (dir.equals("w")) {
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
            else if (dir.equals("s")) {
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
            else if (dir.equals("a")) {
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
            else if (dir.equals("d")) {
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
            else { message = "Invalid! Use WASD or q:"; return; }
            
            if (moved) { addRandomTile(); checkGameOver(); }
            message = "Score: " + score + " Use WASD or q:";
        }
        
        void checkGameOver() {
            for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (board[i][j] == 0) return;
            for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) {
                if (i < 3 && board[i][j] == board[i+1][j]) return;
                if (j < 3 && board[i][j] == board[i][j+1]) return;
            }
            gameOver = true;
            message = "Game Over! Final Score: " + score + " Press Enter...";
        }
        
        void render() {
            clearScreen();
            System.out.println("=== 2048 ===\nMerge numbers, goal is 2048!\n");
            for (int[] row : board) {
                System.out.print("+");
                for (int i = 0; i < 4; i++) System.out.print("-----+");
                System.out.println("\n|");
                for (int v : row) System.out.print(v == 0 ? "     |" : String.format("%5d|", v));
                System.out.println();
            }
            System.out.print("+");
            for (int i = 0; i < 4; i++) System.out.print("-----+");
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 12. Gomoku ========================
    static void gomokuGame() {
        GomokuGame game = new GomokuGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeMove(input);
        }
    }
    
    static class GomokuGame {
        int size = 15;
        char[][] board = new char[size][size];
        char currentPlayer = 'X';
        boolean gameOver = false;
        String message = "Player X's turn, enter coordinate (e.g. A15) or q:";
        
        GomokuGame() { for (char[] row : board) Arrays.fill(row, '.'); }
        
        void makeMove(String input) {
            if (input.equalsIgnoreCase("q")) return;
            try {
                int c = input.charAt(0) - 'A';
                int r = Integer.parseInt(input.substring(1)) - 1;
                if (r < 0 || r >= size || c < 0 || c >= size) throw new Exception();
                if (board[r][c] != '.') { message = "Position occupied!"; return; }
                
                board[r][c] = currentPlayer;
                
                if (checkWin(r, c)) { gameOver = true; message = "Player " + currentPlayer + " wins! Press Enter..."; return; }
                
                boolean full = true;
                for (char[] row : board) for (char c2 : row) if (c2 == '.') { full = false; break; }
                if (full) { gameOver = true; message = "Draw! Press Enter..."; return; }
                
                currentPlayer = currentPlayer == 'X' ? 'O' : 'X';
                message = "Player " + currentPlayer + "'s turn, enter coordinate (e.g. A15) or q:";
            } catch (Exception e) { message = "Format error! Example: A15"; }
        }
        
        boolean checkWin(int row, int col) {
            int[][] dirs = {{0,1},{1,0},{1,1},{1,-1}};
            for (int[] d : dirs) {
                int count = 1;
                for (int i = 1; i < 5; i++) {
                    int x = row + d[0]*i, y = col + d[1]*i;
                    if (x >= 0 && x < size && y >= 0 && y < size && board[x][y] == currentPlayer) count++;
                    else break;
                }
                for (int i = 1; i < 5; i++) {
                    int x = row - d[0]*i, y = col - d[1]*i;
                    if (x >= 0 && x < size && y >= 0 && y < size && board[x][y] == currentPlayer) count++;
                    else break;
                }
                if (count >= 5) return true;
            }
            return false;
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Gomoku ===\nFirst to connect 5 wins!\n");
            System.out.print("   ");
            for (int i = 0; i < Math.min(26, size); i++) System.out.print((char)('A'+i) + " ");
            System.out.println("\n  +" + "--".repeat(size) + "+");
            for (int i = 0; i < size; i++) {
                System.out.printf("%2d|", i+1);
                for (int j = 0; j < size; j++) System.out.print(board[i][j] + " ");
                System.out.println("|");
            }
            System.out.println("  +" + "--".repeat(size) + "+\n" + message);
        }
    }
    
    // ======================== 13. Text Adventure ========================
    static void textAdventureGame() {
        TextAdventureGame game = new TextAdventureGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.processCommand(input);
        }
    }
    
    static class TextAdventureGame {
        String currentRoom = "Hall";
        List<String> inventory = new ArrayList<>();
        int health = 100;
        boolean gameOver = false;
        String message = "";
        Map<String, Map<String, Object>> rooms = new HashMap<>();
        
        TextAdventureGame() {
            Map<String, Object> Hall = new HashMap<>();
            Hall.put("description", "You stand in the center of an ancient hall.");
            Hall.put("exits", Map.of("north", "Library", "east", "Kitchen", "south", "Garden"));
            Hall.put("items", new String[]{"Torch"});
            rooms.put("Hall", Hall);
            
            Map<String, Object> Library = new HashMap<>();
            Library.put("description", "Dusty bookshelves line the walls.");
            Library.put("exits", Map.of("south", "Hall", "west", "SecretRoom"));
            Library.put("items", new String[]{"AncientBook"});
            Library.put("locked", Map.of("west", true));
            rooms.put("Library", Library);
            
            Map<String, Object> Kitchen = new HashMap<>();
            Kitchen.put("description", "A decrepit kitchen with dusty counters.");
            Kitchen.put("exits", Map.of("west", "Hall", "north", "Storage"));
            Kitchen.put("items", new String[]{"Knife"});
            rooms.put("Kitchen", Kitchen);
            
            Map<String, Object> Storage = new HashMap<>();
            Storage.put("description", "A dark room filled with crates.");
            Storage.put("exits", Map.of("south", "Kitchen"));
            Storage.put("items", new String[]{"Key"});
            rooms.put("Storage", Storage);
            
            Map<String, Object> Garden = new HashMap<>();
            Garden.put("description", "An overgrown garden with a dry well.");
            Garden.put("exits", Map.of("north", "Hall", "down", "Cellar"));
            Garden.put("enemy", Map.of("name", "GiantRat", "health", 30, "damage", 10));
            rooms.put("Garden", Garden);
            
            Map<String, Object> Cellar = new HashMap<>();
            Cellar.put("description", "A dark damp cellar with something glowing.");
            Cellar.put("exits", Map.of("up", "Garden"));
            Cellar.put("items", new String[]{"Treasure"});
            Cellar.put("game_win", true);
            rooms.put("Cellar", Cellar);
            
            Map<String, Object> SecretRoom = new HashMap<>();
            SecretRoom.put("description", "A hidden room with strange symbols.");
            SecretRoom.put("exits", Map.of("east", "Library"));
            SecretRoom.put("items", new String[]{"Amulet"});
            rooms.put("SecretRoom", SecretRoom);
        }
        
        void processCommand(String cmd) {
            cmd = cmd.toLowerCase().trim();
            message = "";
            if (cmd.equals("q")) return;
            
            switch (cmd) {
                case "n": case "north": move("north"); break;
                case "s": case "south": move("south"); break;
                case "e": case "east": move("east"); break;
                case "w": case "west": move("west"); break;
                case "u": case "up": move("up"); break;
                case "d": case "down": move("down"); break;
                case "i": showInventory(); break;
                case "h": showHelp(); break;
                case "attack": case "fight": fightEnemy(); break;
                default:
                    if (cmd.startsWith("take")) pickItem(cmd.substring(4).trim());
                    else if (cmd.startsWith("use")) useItem(cmd.substring(3).trim());
                    else message = "Invalid command! Press 'h' for help";
            }
        }
        
        void move(String dir) {
            Map<String, Object> room = rooms.get(currentRoom);
            @SuppressWarnings("unchecked")
            Map<String, String> exits = (Map<String, String>) room.get("exits");
            
            if (exits.containsKey(dir)) {
                String nextRoom = exits.get(dir);
                @SuppressWarnings("unchecked")
                Map<String, Boolean> locked = (Map<String, Boolean>) room.get("locked");
                if (locked != null && locked.getOrDefault(dir, false)) { message = "The " + dir + " door is locked!"; return; }
                
                currentRoom = nextRoom;
                message = "You entered " + nextRoom;
                
                @SuppressWarnings("unchecked")
                Map<String, Object> enemy = (Map<String, Object>) rooms.get(nextRoom).get("enemy");
                if (enemy != null) message += "\nThere is a " + enemy.get("name") + "!";
                
                if (rooms.get(nextRoom).get("game_win") != null) { message += "\nYou found the treasure! You win!"; gameOver = true; }
            } else { message = "You can't go that way!"; }
        }
        
        void pickItem(String itemName) {
            Map<String, Object> room = rooms.get(currentRoom);
            @SuppressWarnings("unchecked")
            List<String> items = new ArrayList<>(Arrays.asList((String[]) room.get("items")));
            itemName = itemName.toLowerCase();
            
            for (String item : items) {
                if (item.toLowerCase().equals(itemName)) {
                    inventory.add(item);
                    items.remove(item);
                    room.put("items", items.toArray(new String[0]));
                    message = "You picked up " + item;
                    return;
                }
            }
            message = "There is no " + itemName + " here";
        }
        
        void useItem(String itemName) {
            itemName = itemName.toLowerCase();
            boolean hasItem = false;
            for (String item : inventory) if (item.toLowerCase().equals(itemName)) hasItem = true;
            if (!hasItem) { message = "You don't have " + itemName; return; }
            
            Map<String, Object> room = rooms.get(currentRoom);
            
            if (itemName.equals("key")) {
                @SuppressWarnings("unchecked")
                Map<String, Boolean> locked = (Map<String, Boolean>) room.get("locked");
                if (locked != null) for (Map.Entry<String, Boolean> e : locked.entrySet()) {
                    if (e.getValue()) { locked.put(e.getKey(), false); inventory.removeIf(i -> i.toLowerCase().equals("key")); message = "You unlocked the " + e.getKey() + " door"; return; }
                }
                message = "No door to unlock here";
            } else if (itemName.equals("knife")) {
                @SuppressWarnings("unchecked")
                Map<String, Object> enemy = (Map<String, Object>) room.get("enemy");
                if (enemy != null) {
                    int eHealth = (int) enemy.get("health") - 25;
                    enemy.put("health", eHealth);
                    message = "You attacked with knife: " + enemy.get("name");
                    if (eHealth <= 0) { room.remove("enemy"); message += "\nYou defeated " + enemy.get("name") + "!"; }
                    else { health -= (int) enemy.get("damage"); message += "\n" + enemy.get("name") + " counter-attacked! Lost " + enemy.get("damage") + " HP"; if (health <= 0) { message += "\nYou were defeated!"; gameOver = true; } }
                } else { message = "No enemy here"; }
            } else { message = "You don't know how to use " + itemName; }
        }
        
        void fightEnemy() {
            Map<String, Object> room = rooms.get(currentRoom);
            @SuppressWarnings("unchecked")
            Map<String, Object> enemy = (Map<String, Object>) room.get("enemy");
            
            if (enemy == null) { message = "No enemy here"; return; }
            
            int eHealth = (int) enemy.get("health") - 10;
            enemy.put("health", eHealth);
            message = "You attacked " + enemy.get("name");
            if (eHealth <= 0) { room.remove("enemy"); message += "\nYou defeated " + enemy.get("name") + "!"; }
            else { health -= (int) enemy.get("damage"); message += "\n" + enemy.get("name") + " counter-attacked! Lost " + enemy.get("damage") + " HP"; if (health <= 0) { message += "\nYou were defeated!"; gameOver = true; } }
        }
        
        void showInventory() { message = inventory.isEmpty() ? "Inventory empty" : "Items: " + String.join(", ", inventory); }
        void showHelp() { message = "Commands: n/s/e/w/u/d | take[item] | use[item] | attack | i(items) | h(help)"; }
        
        void render() {
            clearScreen();
            System.out.println("=== Text Adventure ===");
            Map<String, Object> room = rooms.get(currentRoom);
            System.out.println("\nHP: " + health);
            System.out.println("\n" + room.get("description"));
            
            @SuppressWarnings("unchecked")
            String[] items = (String[]) room.get("items");
            if (items != null && items.length > 0) System.out.println("Items: " + String.join(", ", items));
            
            @SuppressWarnings("unchecked")
            Map<String, String> exits = (Map<String, String>) room.get("exits");
            System.out.println("Exits: " + String.join(", ", exits.keySet()));
            
            @SuppressWarnings("unchecked")
            Map<String, Object> enemy = (Map<String, Object>) room.get("enemy");
            if (enemy != null) System.out.println("Enemy: " + enemy.get("name") + " (HP: " + enemy.get("health") + ")");
            
            System.out.println("\n" + message + "\nCommand (h for help):");
        }
    }
    
    // ======================== 14. 3x3 Puzzle ========================
    static void numberPuzzleGame() {
        NumberPuzzleGame game = new NumberPuzzleGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.moveEmpty(input);
        }
    }
    
    static class NumberPuzzleGame {
        char[][] board = {{'1','2','3'}, {'4','5','6'}, {'7','8',' '}};
        int[] emptyPos = {2, 2};
        int moves = 0;
        boolean gameOver = false;
        String message = "Use WASD to move or q to quit:";
        
        NumberPuzzleGame() { shuffleBoard(); }
        
        void shuffleBoard() { for (int i = 0; i < 100; i++) { String[] dirs = {"w","a","s","d"}; Collections.shuffle(Arrays.asList(dirs)); for (String d : dirs) { if (canMove(d)) { moveEmpty(d); break; } } } }
        
        boolean canMove(String dir) {
            if (dir.equals("w")) return emptyPos[0] > 0;
            if (dir.equals("s")) return emptyPos[0] < 2;
            if (dir.equals("a")) return emptyPos[1] > 0;
            if (dir.equals("d")) return emptyPos[1] < 2;
            return false;
        }
        
        void moveEmpty(String dir) {
            dir = dir.toLowerCase();
            if (dir.equals("q")) return;
            if (!canMove(dir)) { message = "Cannot move that way!"; return; }
            
            int[] newPos = {emptyPos[0], emptyPos[1]};
            if (dir.equals("w")) newPos[0]--;
            else if (dir.equals("s")) newPos[0]++;
            else if (dir.equals("a")) newPos[1]--;
            else if (dir.equals("d")) newPos[1]++;
            
            board[emptyPos[0]][emptyPos[1]] = board[newPos[0]][newPos[1]];
            board[newPos[0]][newPos[1]] = ' ';
            emptyPos = newPos;
            moves++;
            
            if (checkWin()) { gameOver = true; message = "Congratulations! " + moves + " moves. Press Enter..."; }
            else message = "Moves: " + moves + " Use WASD or q:";
        }
        
        boolean checkWin() {
            return board[0][0] == '1' && board[0][1] == '2' && board[0][2] == '3' &&
                   board[1][0] == '4' && board[1][1] == '5' && board[1][2] == '6' &&
                   board[2][0] == '7' && board[2][1] == '8' && board[2][2] == ' ';
        }
        
        void render() {
            clearScreen();
            System.out.println("=== 3x3 Slide Puzzle ===\nArrange numbers in order (1-8):\n");
            for (int i = 0; i < 3; i++) {
                System.out.print(" +---+---+---+\n|");
                for (int j = 0; j < 3; j++) {
                    char c = board[i][j];
                    System.out.print(" " + (c == ' ' ? ' ' : c) + " |");
                }
                System.out.println();
            }
            System.out.println(" +---+---+\n\nControls: W:up A:left S:down D:right\n" + message);
        }
    }
    
    // ======================== 15. Number Position ========================
    static void numberPositionGame() {
        NumberPositionGame game = new NumberPositionGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeGuess(input);
        }
    }
    
    static class NumberPositionGame {
        String secret;
        int attempts = 0, maxAttempts = 10;
        boolean gameOver = false;
        String message = "Guess 4 unique digits (e.g. 1234) or q to quit:";
        
        NumberPositionGame() {
            List<Integer> digits = new ArrayList<>(Arrays.asList(0,1,2,3,4,5,6,7,8,9));
            Collections.shuffle(digits);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < 4; i++) sb.append(digits.get(i));
            secret = sb.toString();
        }
        
        void makeGuess(String guess) {
            if (guess.equalsIgnoreCase("q")) return;
            if (guess.length() != 4 || !guess.matches("\\d+") || guess.chars().distinct().count() != 4) { message = "Enter 4 unique digits!"; return; }
            
            attempts++;
            int a = 0, b = 0;
            for (int i = 0; i < 4; i++) {
                if (guess.charAt(i) == secret.charAt(i)) a++;
                else if (secret.indexOf(guess.charAt(i)) >= 0) b++;
            }
            
            if (a == 4) { message = "Correct! " + attempts + " tries. Answer: " + secret + "\nPress Enter..."; gameOver = true; }
            else if (attempts >= maxAttempts) { message = "Game Over! Answer: " + secret + "\nPress Enter..."; gameOver = true; }
            else message = guess + ": " + a + "A" + b + "B\nRemaining: " + (maxAttempts - attempts) + "\nGuess again (or q):";
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Number Position ===\nRules: Guess 4 unique digits, A=correct pos, B=correct digit wrong pos\n");
            System.out.println("Attempts: " + attempts + "/" + maxAttempts + "\n" + message);
        }
    }
    
    // ======================== 16. Number Sort ========================
    static void numberSortGame() {
        NumberSortGame game = new NumberSortGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            game.makeMove(input);
        }
    }
    
    static class NumberSortGame {
        int[] numbers = {1, 2, 3, 4};
        int steps = 0;
        boolean gameOver = false;
        String message = "Enter positions to swap (e.g. 1 2) or q to quit:";
        
        NumberSortGame() {
            int[][] swaps = {{0,1},{1,2},{2,3},{0,3}};
            for (int i = 0; i < 10; i++) {
                int[] s = swaps[i % 4];
                int tmp = numbers[s[0]]; numbers[s[0]] = numbers[s[1]]; numbers[s[1]] = tmp;
            }
        }
        
        void makeMove(String input) {
            if (input.equalsIgnoreCase("q")) return;
            try {
                String[] parts = input.split("\\s+");
                int p1 = Integer.parseInt(parts[0]) - 1;
                int p2 = Integer.parseInt(parts[1]) - 1;
                if (p1 < 0 || p1 > 3 || p2 < 0 || p2 > 3 || Math.abs(p1 - p2) != 1) { message = "Only swap adjacent numbers!"; return; }
                
                int tmp = numbers[p1]; numbers[p1] = numbers[p2]; numbers[p2] = tmp;
                steps++;
                
                if (checkWin()) { gameOver = true; message = "Congratulations! " + steps + " steps.\nPress Enter..."; }
                else message = "Current: " + Arrays.toString(numbers) + "\nSteps: " + steps + "\nContinue (or q):";
            } catch (Exception e) { message = "Format error! Example: 1 2"; }
        }
        
        boolean checkWin() { return numbers[0] == 1 && numbers[1] == 2 && numbers[2] == 3 && numbers[3] == 4; }
        
        void render() {
            clearScreen();
            System.out.println("=== Number Sort ===\nSwap adjacent numbers to sort in ascending order\n");
            System.out.println("Current: " + Arrays.toString(numbers));
            System.out.println("Steps: " + steps + "\n" + message);
        }
    }
    
    // ======================== 17. Word Spelling ========================
    static void wordSpellingGame() {
        WordSpellingGame game = new WordSpellingGame();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            if (!game.started) { game.start(); }
            else { game.checkSpelling(input); }
        }
    }
    
    static class WordSpellingGame {
        String[] words = {"apple", "banana", "computer", "driver", "elephant", "give", "have", "in", "just", "know", "light", "minecraft", "no", "off", "plus", "quickly", "read", "sun", "time", "water"};
        String currentWord = "";
        int score = 0;
        long startTime = 0;
        int timeLimit = 30;
        boolean started = false, gameOver = false;
        String message = "Press Enter to start!";
        
        void start() {
            currentWord = words[random.nextInt(words.length)];
            startTime = System.currentTimeMillis();
            started = true;
            message = "Spell this word: " + currentWord.toUpperCase();
        }
        
        void checkSpelling(String input) {
            if (System.currentTimeMillis() - startTime > timeLimit * 1000) { gameOver = true; message = "Time's up! Score: " + score + "\nPress Enter..."; return; }
            
            if (input.equalsIgnoreCase(currentWord)) { score++; message = "Correct! Score: " + score + "\nNext..."; try { Thread.sleep(500); } catch (Exception e) {} currentWord = words[random.nextInt(words.length)]; message = "Spell: " + currentWord.toUpperCase(); }
            else { message = "Wrong! Should be: " + currentWord.toUpperCase() + "\nScore: " + score; try { Thread.sleep(1000); } catch (Exception e) {} currentWord = words[random.nextInt(words.length)]; message = "Spell: " + currentWord.toUpperCase(); }
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Word Spelling ===\nScore: " + score);
            if (started) {
                long elapsed = (System.currentTimeMillis() - startTime) / 1000;
                System.out.println("Time left: " + Math.max(0, timeLimit - elapsed) + "s");
            }
            System.out.println("\n" + message);
        }
    }
    
    // ======================== 18. Color Match ========================
    static void colorMatchGame() {
        ColorMatchGame game = new ColorMatchGame();
        game.startRound();
        while (true) {
            game.render();
            if (game.gameOver) { scanner.nextLine(); break; }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            if (game.checkAnswer(input)) break;
        }
    }
    
    static class ColorMatchGame {
        String[] colorNames = {"red", "green", "yellow", "blue", "purple", "cyan"};
        String[] colorCodes = {"\u001b[31m", "\u001b[32m", "\u001b[33m", "\u001b[34m", "\u001b[35m", "\u001b[36m"};
        String currentColor = "", currentWord = "";
        int score = 0, rounds = 0, maxRounds = 10;
        boolean gameOver = false;
        String message = "Color Match! Enter color name (red/green/yellow/blue/purple/cyan)";
        
        void startRound() {
            currentColor = colorNames[random.nextInt(6)];
            currentWord = colorNames[random.nextInt(6)];
            if (random.nextDouble() > 0.5) currentWord = currentColor;
            rounds++;
        }
        
        boolean checkAnswer(String answer) {
            if (answer.toLowerCase().equals(currentColor)) score++;
            else message = "Wrong! Color was: " + colorCodes[Arrays.asList(colorNames).indexOf(currentColor)] + currentColor + "\u001b[0m";
            
            if (rounds < maxRounds) {
                try { Thread.sleep(800); } catch (Exception e) {}
                startRound();
            } else { gameOver = true; message = "Game Over! Score: " + score + "/" + maxRounds; }
            return gameOver;
        }
        
        void render() {
            clearScreen();
            System.out.println("=== Color Match ===\nRound: " + rounds + "/" + maxRounds + "  Score: " + score);
            if (!gameOver && rounds > 0) {
                int idx = Arrays.asList(colorNames).indexOf(currentColor);
                System.out.println("\nWord color is: " + colorCodes[idx] + currentWord.toUpperCase() + "\u001b[0m");
            }
            System.out.println("\n" + message);
            if (!gameOver) System.out.println("\nOptions: red, green, yellow, blue, purple, cyan");
        }
    }
    
    // ======================== 19. Shape Match ========================
    static void shapeEliminationGame() {
        ShapeEliminationGame game = new ShapeEliminationGame();
        long startTime = System.currentTimeMillis();
        while (true) {
            game.render();
            if (game.gameOver || System.currentTimeMillis() - startTime > 60000) { 
                if (!game.gameOver) { game.gameOver = true; game.message = "Time's up! Final score: " + game.score + "\nPress Enter..."; }
                scanner.nextLine(); break; 
            }
            System.out.print("> ");
            String input = scanner.nextLine().trim();
            if (input.equalsIgnoreCase("q")) break;
            try {
                String[] parts = input.split("\\s+");
                if (parts.length == 2) {
                    int y = Integer.parseInt(parts[0]);
                    int x = Integer.parseInt(parts[1]);
                    game.selectTile(y, x);
                }
            } catch (Exception e) {}
        }
    }
    
    static class ShapeEliminationGame {
        String[] shapes = {"A", "B", "C", "D", "E", "F"};
        String[][] board = new String[8][8];
        int[] selected = {-1, -1};
        int score = 0;
        boolean gameOver = false;
        String message = "Select two adjacent same shapes to swap and match";
        
        ShapeEliminationGame() {
            while (true) {
                for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) board[y][x] = shapes[random.nextInt(6)];
                if (!findMatches()) break;
            }
        }
        
        boolean findMatches() {
            for (int y = 0; y < 8; y++) for (int x = 0; x < 6; x++) if (board[y][x].equals(board[y][x+1]) && board[y][x].equals(board[y][x+2])) return true;
            for (int x = 0; x < 8; x++) for (int y = 0; y < 6; y++) if (board[y][x].equals(board[y+1][x]) && board[y][x].equals(board[y+2][x])) return true;
            return false;
        }
        
        void selectTile(int y, int x) {
            if (y < 0 || y > 7 || x < 0 || x > 7) { message = "Out of range!"; return; }
            
            if (selected[0] == -1) { selected[0] = y; selected[1] = x; message = "Selected (" + y + "," + x + "), select another"; }
            else {
                if (Math.abs(y - selected[0]) == 1 && x == selected[1] || Math.abs(x - selected[1]) == 1 && y == selected[0]) {
                    if (swapTiles(selected[0], selected[1], y, x)) message = "Match! Continue";
                    else message = "No match, try again";
                } else message = "Must be adjacent!";
                selected[0] = -1; selected[1] = -1;
            }
        }
        
        boolean swapTiles(int y1, int x1, int y2, int x2) {
            String tmp = board[y1][x1]; board[y1][x1] = board[y2][x2]; board[y2][x2] = tmp;
            
            Set<String> matches = new HashSet<>();
            for (int y = 0; y < 8; y++) for (int x = 0; x < 6; x++) if (board[y][x].equals(board[y][x+1]) && board[y][x].equals(board[y][x+2])) { matches.add(board[y][x]); }
            for (int x = 0; x < 8; x++) for (int y = 0; y < 6; y++) if (board[y][x].equals(board[y+1][x]) && board[y][x].equals(board[y+2][x])) { matches.add(board[y][x]); }
            
            if (!matches.isEmpty()) {
                score += matches.size() * 30;
                for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) if (matches.contains(board[y][x])) board[y][x] = null;
                dropTiles();
                fillBoard();
                return true;
            } else { tmp = board[y1][x1]; board[y1][x1] = board[y2][x2]; board[y2][x2] = tmp; return false; }
        }
        
        void dropTiles() {
            for (int x = 0; x < 8; x++) {
                List<String> col = new ArrayList<>();
                for (int y = 0; y < 8; y++) if (board[y][x] != null) col.add(board[y][x]);
                for (int y = 0; y < 8; y++) board[y][x] = y < col.size() ? col.get(y) : null;
            }
        }
        
        void fillBoard() { for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) if (board[y][x] == null) board[y][x] = shapes[random.nextInt(6)]; }
        
        void render() {
            clearScreen();
            System.out.println("=== Shape Match ===\nScore: " + score + "\n" + message + "\n");
            System.out.print("   ");
            for (int i = 0; i < 8; i++) System.out.print(i + " ");
            System.out.println("\n  +" + "--".repeat(8) + "+");
            for (int y = 0; y < 8; y++) {
                System.out.print(y + " |");
                for (int x = 0; x < 8; x++) {
                    String s = board[y][x];
                    if (selected[0] == y && selected[1] == x) System.out.print("*" + s + "*");
                    else System.out.print(" " + s + " ");
                }
                System.out.println("|");
            }
            System.out.println("  +" + "--".repeat(8) + "+\nEnter coordinate (e.g. 3 4) or q:");
        }
    }
}
