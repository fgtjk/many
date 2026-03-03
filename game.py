import random
import os
import time

# ======================== 魔塔游戏部分 ========================
class GameState:
    def __init__(self):
        self.hp = 200
        self.max_hp = 200
        self.attack = 20
        self.defense = 20
        self.gold = 0
        self.floor = 1
        self.keys = {'yellow': 1, 'blue': 0, 'red': 0}
        self.inventory = []
        self.player_x = 5
        self.player_y = 5
        self.floors = {}
        self.generate_floors()
        self.message = ""
        self.has_sword = False
        self.has_shield = False
    
    def generate_floors(self):
        for floor in range(1, 101):
            game_map = [[0 for _ in range(10)] for _ in range(10)]
            
            # 墙壁
            for i in range(10):
                game_map[i][0] = 1
                game_map[i][9] = 1
                game_map[0][i] = 1
                game_map[9][i] = 1
            
            if floor == 1:
                game_map[5][5] = 0
                game_map[7][3] = 4
                game_map[3][7] = 3
                game_map[2][2] = 3
                game_map[2][7] = 3
                game_map[7][2] = 3
                game_map[8][8] = 2
            elif floor == 100:
                game_map[5][5] = 6
                game_map[5][4] = 5
            else:
                while True:
                    x, y = random.randint(1, 8), random.randint(1, 8)
                    if game_map[y][x] == 0:
                        game_map[y][x] = 4
                        break
                
                if floor > 1:
                    while True:
                        x, y = random.randint(1, 8), random.randint(1, 8)
                        if game_map[y][x] == 0:
                            game_map[y][x] = 5
                            break
                
                enemy_count = min(8, 2 + floor // 15)
                for _ in range(enemy_count):
                    while True:
                        x, y = random.randint(1, 8), random.randint(1, 8)
                        if game_map[y][x] == 0:
                            if floor < 30:
                                game_map[y][x] = 2
                            elif floor < 70:
                                game_map[y][x] = 7
                            else:
                                game_map[y][x] = 8
                            break
                
                item_count = random.randint(3, 5)
                for _ in range(item_count):
                    while True:
                        x, y = random.randint(1, 8), random.randint(1, 8)
                        if game_map[y][x] == 0:
                            game_map[y][x] = 3
                            break
                
                if floor < 10:
                    while True:
                        x, y = random.randint(1, 8), random.randint(1, 8)
                        if game_map[y][x] == 0:
                            game_map[y][x] = 3
                            break
            
            self.floors[floor] = game_map
    
    def move_player(self, dx, dy):
        new_x, new_y = self.player_x + dx, self.player_y + dy
        current_map = self.floors.get(self.floor, [])
        
        if not current_map or new_x < 0 or new_y < 0 or new_x >= 10 or new_y >= 10:
            self.message = "不能往那个方向移动!"
            return
        
        tile = current_map[new_y][new_x]
        
        if tile == 0:
            self.player_x, self.player_y = new_x, new_y
            self.message = ""
        elif tile == 1:
            self.message = "撞到墙了!"
        elif tile == 2:
            enemy_hp = 30 + self.floor * 2
            enemy_atk = 10 + self.floor // 3
            enemy_def = 5 + self.floor // 5
            enemy_gold = 10 + self.floor
            self.fight_enemy(new_x, new_y, enemy_hp, enemy_atk, enemy_def, enemy_gold)
        elif tile == 3:
            self.pick_item(new_x, new_y)
        elif tile == 4:
            self.change_floor(1)
        elif tile == 5:
            self.change_floor(-1)
        elif tile == 6:
            self.fight_boss(new_x, new_y)
        elif tile == 7:
            self.fight_enemy(new_x, new_y, 80 + self.floor*2, 20 + self.floor//2, 10 + self.floor//4, 30 + self.floor*2)
        elif tile == 8:
            self.fight_enemy(new_x, new_y, 120 + self.floor*3, 30 + self.floor//2, 15 + self.floor//3, 50 + self.floor*3)
    
    def fight_enemy(self, x, y, enemy_hp, enemy_atk, enemy_def, enemy_gold):
        damage_to_enemy = max(1, self.attack - enemy_def)
        damage_to_player = max(1, enemy_atk - self.defense)
        
        hits_to_kill = (enemy_hp + damage_to_enemy - 1) // damage_to_enemy
        self.hp -= damage_to_player * (hits_to_kill - 1)
        
        if self.hp > 0:
            self.floors[self.floor][y][x] = 0
            self.gold += enemy_gold
            self.player_x, self.player_y = x, y
            self.message = f"击败敌人! 获得{enemy_gold}金币, 损失{damage_to_player * (hits_to_kill - 1)}HP"
        else:
            self.message = "你被击败了! 游戏结束."
            self.game_over()
    
    def fight_boss(self, x, y):
        if not self.has_sword:
            self.message = "你需要传说中的宝剑才能伤害Boss!"
            return
        
        boss_hp = 500
        boss_atk = 50
        boss_def = 30
        boss_gold = 1000
        
        damage_to_boss = max(1, self.attack * 2 - boss_def)
        damage_to_player = max(1, boss_atk - self.defense)
        
        hits_to_kill = (boss_hp + damage_to_boss - 1) // damage_to_boss
        self.hp -= damage_to_player * (hits_to_kill - 1)
        
        if self.hp > 0:
            self.floors[self.floor][y][x] = 0
            self.gold += boss_gold
            self.player_x, self.player_y = x, y
            self.message = f"击败Boss! 获得{boss_gold}金币\n恭喜通关100层魔塔!"
            self.game_win()
        else:
            self.message = "你被Boss击败了! 游戏结束."
            self.game_over()
    
    def pick_item(self, x, y):
        if self.floor == 25 and not self.has_sword:
            item_type = 'legendary_sword'
        elif self.floor == 50 and not self.has_shield:
            item_type = 'magic_shield'
        else:
            item_choices = [
                ('yellow_key', 0.2),
                ('blue_key', 0.1),
                ('red_key', 0.05),
                ('potion', 0.5),
                ('super_potion', 0.15)
            ]
            item_type = random.choices(
                [item[0] for item in item_choices],
                weights=[item[1] for item in item_choices]
            )[0]
        
        if item_type.endswith('_key'):
            key_color = item_type.split('_')[0]
            self.keys[key_color] += 1
            self.message = f"获得1把{key_color}色钥匙!"
        elif item_type == 'potion':
            heal = 50
            self.hp = min(self.max_hp, self.hp + heal)
            self.message = f"恢复{heal}点生命值!"
        elif item_type == 'super_potion':
            heal = 120
            self.hp = min(self.max_hp, self.hp + heal)
            self.message = f"恢复{heal}点生命值!"
        elif item_type == 'legendary_sword':
            self.attack += 30
            self.has_sword = True
            self.message = "获得传说宝剑! 攻击+30 (对Boss有效)"
        elif item_type == 'magic_shield':
            self.defense += 20
            self.has_shield = True
            self.message = "获得魔法盾牌! 防御+20"
        
        self.floors[self.floor][y][x] = 0
        self.player_x, self.player_y = x, y
    
    def change_floor(self, direction):
        new_floor = self.floor + direction
        
        if 1 <= new_floor <= 100:
            self.floor = new_floor
            self.message = f"来到第{self.floor}层"
            
            for y in range(10):
                for x in range(10):
                    target_tile = 4 if direction == -1 else 5
                    if self.floors[self.floor][y][x] == target_tile:
                        for dy in [-1, 0, 1]:
                            for dx in [-1, 0, 1]:
                                nx, ny = x + dx, y + dy
                                if 0 <= nx < 10 and 0 <= ny < 10 and self.floors[self.floor][ny][nx] == 0:
                                    self.player_x, self.player_y = nx, ny
                                    return
            while True:
                self.player_x, self.player_y = random.randint(1, 8), random.randint(1, 8)
                if self.floors[self.floor][self.player_y][self.player_x] == 0:
                    break
        else:
            self.message = "不能离开魔塔!"
    
    def game_over(self):
        print("\n游戏结束!")
        print(f"你到达了第{self.floor}层")
        print(f"最终属性: HP {self.hp}/{self.max_hp}, ATK {self.attack}, DEF {self.defense}")
        print(f"收集的金币: {self.gold}")
        input("按回车键返回主菜单...")
    
    def game_win(self):
        print("\n恭喜你通关了100层魔塔!")
        print(f"最终属性: HP {self.hp}/{self.max_hp}, ATK {self.attack}, DEF {self.defense}")
        print(f"收集的金币: {self.gold}")
        print("你成为了魔塔的征服者!")
        input("按回车键返回主菜单...")

# ======================== 贪吃蛇游戏部分 ========================
class SnakeGame:
    def __init__(self):
        self.width = 20
        self.height = 15
        self.snake = [(self.width // 2, self.height // 2)]
        self.direction = (1, 0)
        self.food = self.generate_food()
        self.score = 0
        self.game_over = False
        self.message = "使用WASD控制贪吃蛇，Q退出"
    
    def generate_food(self):
        while True:
            food = (random.randint(0, self.width - 1), random.randint(0, self.height - 1))
            if food not in self.snake:
                return food
    
    def change_direction(self, dx, dy):
        if (dx, dy) != (-self.direction[0], -self.direction[1]):
            self.direction = (dx, dy)
    
    def update(self):
        if self.game_over:
            return
        
        head_x, head_y = self.snake[0]
        new_x = (head_x + self.direction[0]) % self.width
        new_y = (head_y + self.direction[1]) % self.height
        new_head = (new_x, new_y)
        
        if new_head in self.snake:
            self.game_over = True
            self.message = f"游戏结束! 得分: {self.score} 按Q返回主菜单"
            return
        
        self.snake.insert(0, new_head)
        
        if new_head == self.food:
            self.score += 1
            self.food = self.generate_food()
        else:
            self.snake.pop()
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 贪吃蛇游戏 ===")
        print(f"得分: {self.score}")
        print(self.message)
        print()
        
        for y in range(self.height):
            row = []
            for x in range(self.width):
                if (x, y) == self.food:
                    row.append('$')
                elif (x, y) == self.snake[0]:
                    row.append('@')
                elif (x, y) in self.snake:
                    row.append('O')
                else:
                    row.append('.')
            print(' '.join(row))
        
        print("\n控制: w(上) a(左) s(下) d(右) q(退出)")

# ======================== 猜数字游戏部分 ========================
class GuessNumberGame:
    def __init__(self):
        self.number = random.randint(1, 100)
        self.attempts = 0
        self.max_attempts = 10
        self.game_over = False
        self.message = f"猜一个1-100之间的数字，你有{self.max_attempts}次机会"
    
    def make_guess(self, guess):
        try:
            guess_num = int(guess)
            self.attempts += 1
            
            if guess_num < self.number:
                self.message = f"太小了! 还剩{self.max_attempts - self.attempts}次机会"
            elif guess_num > self.number:
                self.message = f"太大了! 还剩{self.max_attempts - self.attempts}次机会"
            else:
                self.message = f"恭喜! 你猜对了! 用了{self.attempts}次尝试"
                self.game_over = True
                return True
            
            if self.attempts >= self.max_attempts:
                self.message = f"游戏结束! 正确答案是{self.number}"
                self.game_over = True
            
            return False
        except ValueError:
            self.message = "请输入一个有效的数字!"
            return False
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 猜数字游戏 ===")
        print(self.message)
        if not self.game_over:
            print("\n输入你的猜测(1-100)，或输入q退出:")
        else:
            print("\n按回车键返回主菜单...")
            
# ======================== ####### ========================        
class TicTacToeGame:
    def __init__(self):
        self.board = [' ' for _ in range(9)]  # 3x3棋盘
        self.current_player = 'X'
        self.game_over = False
        self.winner = None
        self.message = "请输入位置(1-9)或q退出:"
    
    def print_board(self):
        print("\n")
        for i in range(0, 9, 3):
            print(f" {self.board[i]} | {self.board[i+1]} | {self.board[i+2]} ")
            if i < 6:
                print("-----------")
        print("\n")
    
    def make_move(self, position):
        if position.lower() == 'q':
            return 'quit'
        
        try:
            pos = int(position) - 1
            if pos < 0 or pos > 8:
                self.message = "请输入1-9的数字!"
                return
        except ValueError:
            self.message = "请输入有效数字!"
            return
        
        if self.board[pos] != ' ':
            self.message = "该位置已被占用!"
            return
        
        self.board[pos] = self.current_player
        if self.check_winner():
            self.game_over = True
            self.winner = self.current_player
            self.message = f"玩家 {self.current_player} 获胜! 按回车键返回..."
        elif ' ' not in self.board:
            self.game_over = True
            self.message = "平局! 按回车键返回..."
        else:
            self.current_player = 'O' if self.current_player == 'X' else 'X'
            self.message = f"玩家 {self.current_player} 的回合，输入位置(1-9):"
    
    def check_winner(self):
        # 检查行
        for i in range(0, 9, 3):
            if self.board[i] == self.board[i+1] == self.board[i+2] != ' ':
                return True
        # 检查列
        for i in range(3):
            if self.board[i] == self.board[i+3] == self.board[i+6] != ' ':
                return True
        # 检查对角线
        if self.board[0] == self.board[4] == self.board[8] != ' ':
            return True
        if self.board[2] == self.board[4] == self.board[6] != ' ':
            return True
        return False
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 井字棋游戏 ===")
        print("位置对应数字:")
        print(" 1 | 2 | 3 ")
        print("-----------")
        print(" 4 | 5 | 6 ")
        print("-----------")
        print(" 7 | 8 | 9 ")
        self.print_board()
        print(self.message)
        
# ======================== 记忆 =========================
class MemoryGame:
    def __init__(self):
        self.symbols = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'] * 2
        random.shuffle(self.symbols)
        self.board = ['?' for _ in range(16)]  # 4x4棋盘
        self.matched = [False for _ in range(16)]
        self.first_card = None
        self.second_card = None
        self.attempts = 0
        self.game_over = False
        self.message = "选择要翻开的牌(1-16)或q退出:"
    
    def print_board(self):
        print("\n")
        for i in range(0, 16, 4):
            row = []
            for j in range(4):
                idx = i + j
                if self.matched[idx] or self.first_card == idx or self.second_card == idx:
                    row.append(self.symbols[idx])
                else:
                    row.append(str(idx+1).rjust(2))
            print(" | ".join(row))
            if i < 12:
                print("-" * 23)
        print("\n")
    
    def flip_card(self, position):
        if position.lower() == 'q':
            return 'quit'
        
        try:
            pos = int(position) - 1
            if pos < 0 or pos > 15:
                self.message = "请输入1-16的数字!"
                return
        except ValueError:
            self.message = "请输入有效数字!"
            return
        
        if self.matched[pos]:
            self.message = "这张牌已经匹配了!"
            return
        if pos == self.first_card:
            self.message = "不能选择同一张牌!"
            return
        
        if self.first_card is None:
            self.first_card = pos
            self.message = "选择第二张牌:"
        else:
            self.second_card = pos
            self.attempts += 1
            self.check_match()
    
    def check_match(self):
        if self.symbols[self.first_card] == self.symbols[self.second_card]:
            self.matched[self.first_card] = True
            self.matched[self.second_card] = True
            self.message = "匹配成功!"
            
            if all(self.matched):
                self.game_over = True
                self.message = f"恭喜! 你完成了游戏，共用了{self.attempts}次尝试。按回车键返回..."
        else:
            self.message = "没有匹配，请继续尝试!"
        
        # 短暂显示两张牌
        self.print_board()
        input("按回车键继续...")
        
        # 重置选择
        self.first_card = None
        self.second_card = None
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 记忆翻牌游戏 ===")
        print(f"\n尝试次数: {self.attempts}")
        self.print_board()
        print(self.message)        

# ======================== 华容道 ========================= 
class SlidePuzzleGame:
    def __init__(self):
        self.size = 4  # 4x4的棋盘
        self.board = list(range(1, 16)) + [0]  # 0代表空格
        random.shuffle(self.board)
        self.empty_pos = self.board.index(0)
        self.moves = 0
        self.game_over = False
        self.message = "使用WASD移动数字或q退出:"
    
    def print_board(self):
        print("\n")
        for i in range(0, 16, 4):
            row = []
            for j in range(4):
                val = self.board[i + j]
                row.append(str(val).rjust(2) if val != 0 else "  ")
            print(" | ".join(row))
            if i < 12:
                print("-" * 23)
        print("\n")
    
    def move_tile(self, direction):
        direction = direction.lower()
        if direction == 'q':
            return 'quit'
        
        row, col = self.empty_pos // 4, self.empty_pos % 4
        new_pos = None
        
        if direction == 'w' and row > 0:  # 上
            new_pos = self.empty_pos - 4
        elif direction == 's' and row < 3:  # 下
            new_pos = self.empty_pos + 4
        elif direction == 'a' and col > 0:  # 左
            new_pos = self.empty_pos - 1
        elif direction == 'd' and col < 3:  # 右
            new_pos = self.empty_pos + 1
        else:
            self.message = "无效移动! 使用WASD或q退出:"
            return
        
        # 交换空格和相邻数字
        self.board[self.empty_pos], self.board[new_pos] = self.board[new_pos], self.board[self.empty_pos]
        self.empty_pos = new_pos
        self.moves += 1
        
        # 检查是否完成
        if self.board == list(range(1, 16)) + [0]:
            self.game_over = True
            self.message = f"恭喜完成! 共用了{self.moves}步。按回车键返回..."
        else:
            self.message = f"移动次数: {self.moves} 使用WASD或q退出:"
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 数字华容道 ===")
        print("\n将数字按顺序排列(1-15):")
        self.print_board()
        print(self.message)
        
# ======================== 迷宫 =========================
class MazeGame:
    def __init__(self):
        self.width = 15
        self.height = 15
        self.player_pos = [1, 1]  # 玩家起始位置
        self.exit_pos = [13, 13]  # 出口位置
        self.maze = self.generate_maze()
        self.message = "使用WASD移动，q退出:"
        self.visibility = 3  # 视野范围
        self.fog_of_war = True  # 是否开启战争迷雾
    
    def generate_maze(self):
        # 初始化全墙迷宫
        maze = [[1 for _ in range(self.width)] for _ in range(self.height)]
        
        # 使用深度优先搜索生成迷宫
        stack = [self.player_pos]
        maze[self.player_pos[1]][self.player_pos[0]] = 0
        
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]
        
        while stack:
            x, y = stack[-1]
            random.shuffle(directions)
            
            for dx, dy in directions:
                nx, ny = x + dx*2, y + dy*2
                
                if 0 < nx < self.width-1 and 0 < ny < self.height-1 and maze[ny][nx] == 1:
                    maze[ny][nx] = 0
                    maze[y+dy][x+dx] = 0
                    stack.append([nx, ny])
                    break
            else:
                stack.pop()
        
        # 确保出口可达
        self.ensure_path(maze, self.player_pos, self.exit_pos)
        
        # 添加更多通路
        for y in range(1, self.height-1):
            for x in range(1, self.width-1):
                if random.random() < 0.1 and maze[y][x] == 1:
                    if (maze[y-1][x] == 0 or maze[y+1][x] == 0 or 
                        maze[y][x-1] == 0 or maze[y][x+1] == 0):
                        maze[y][x] = 0
        
        # 确保出口是通路
        maze[self.exit_pos[1]][self.exit_pos[0]] = 0
        
        return maze
    
    def ensure_path(self, maze, start, end):
        # A*算法确保路径存在
        open_set = [start]
        came_from = {}
        g_score = {tuple(start): 0}
        f_score = {tuple(start): self.heuristic(start, end)}
        
        while open_set:
            current = min(open_set, key=lambda pos: f_score.get(tuple(pos), float('inf')))
            
            if current == end:
                return True
            
            open_set.remove(current)
            
            for dx, dy in [(0,1),(1,0),(0,-1),(-1,0)]:
                neighbor = [current[0]+dx, current[1]+dy]
                
                if (0 <= neighbor[0] < self.width and 
                    0 <= neighbor[1] < self.height and 
                    maze[neighbor[1]][neighbor[0]] == 0):
                    
                    tentative_g = g_score[tuple(current)] + 1
                    
                    if (tuple(neighbor) not in g_score or 
                        tentative_g < g_score[tuple(neighbor)]):
                        
                        came_from[tuple(neighbor)] = current
                        g_score[tuple(neighbor)] = tentative_g
                        f_score[tuple(neighbor)] = tentative_g + self.heuristic(neighbor, end)
                        
                        if neighbor not in open_set:
                            open_set.append(neighbor)
        
        # 如果没有路径，创建一条
        x, y = start
        end_x, end_y = end
        
        # 先横向移动
        step = 1 if end_x > x else -1
        for dx in range(x, end_x, step):
            maze[y][dx] = 0
        
        # 再纵向移动
        step = 1 if end_y > y else -1
        for dy in range(y, end_y, step):
            maze[dy][end_x] = 0
        
        # 修正拐角
        maze[y][end_x] = 0
    
    def heuristic(self, a, b):
        return abs(a[0] - b[0]) + abs(a[1] - b[1])
    
    def move_player(self, direction):
        direction = direction.lower()
        if direction == 'q':
            return 'quit'
        
        x, y = self.player_pos
        new_x, new_y = x, y
        
        if direction == 'w': new_y -= 1  # 上
        elif direction == 's': new_y += 1  # 下
        elif direction == 'a': new_x -= 1  # 左
        elif direction == 'd': new_x += 1  # 右
        else:
            self.message = "无效输入! 使用WASD或q退出:"
            return
        
        # 检查边界
        if new_x < 0 or new_x >= self.width or new_y < 0 or new_y >= self.height:
            self.message = "不能走出迷宫边界! 使用WASD或q退出:"
            return
        
        # 检查是否撞墙
        if self.maze[new_y][new_x] == 1:
            self.message = "撞墙了! 使用WASD或q退出:"
            return
        
        # 移动玩家
        self.player_pos = [new_x, new_y]
        self.message = "使用WASD移动，q退出:"
        
        # 检查是否到达出口
        if self.player_pos == self.exit_pos:
            self.message = "恭喜逃出迷宫! 按回车键返回..."
            return 'win'
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 15x15迷宫探险 ===")
        print("\n找到出口(E)! 墙壁(#)不能穿过\n")
        
        for y in range(self.height):
            row = []
            for x in range(self.width):
                if [x, y] == self.player_pos:
                    row.append('P')
                elif [x, y] == self.exit_pos:
                    row.append('E')
                elif self.maze[y][x] == 1:
                    if self.fog_of_war and abs(x-self.player_pos[0]) > self.visibility or abs(y-self.player_pos[1]) > self.visibility:
                        row.append('?')
                    else:
                        row.append('#')
                else:
                    if self.fog_of_war and abs(x-self.player_pos[0]) > self.visibility or abs(y-self.player_pos[1]) > self.visibility:
                        row.append('?')
                    else:
                        row.append('.')
            print(' '.join(row))
        
        print("\n" + self.message)
        
# ======================== 数独 =========================
class SudokuGame:
    def __init__(self):
        self.size = 9
        self.board = self.generate_puzzle()
        self.original = [[cell for cell in row] for row in self.board]  # 保存原始题目
        self.message = "输入行列和数字(如 1 2 3)或q退出:"
    
    def generate_puzzle(self):
        # 生成一个有效的数独谜题
        base = 3
        side = base*base
        
        # 生成基础模板
        def pattern(r,c): return (base*(r%base)+r//base+c)%side
        
        # 随机排列数字
        from random import sample
        def shuffle(s): return sample(s,len(s)) 
        rBase = range(base) 
        rows  = [ g*base + r for g in shuffle(rBase) for r in shuffle(rBase) ] 
        cols  = [ g*base + c for g in shuffle(rBase) for c in shuffle(rBase) ]
        nums  = shuffle(range(1,base*base+1))
        
        # 生成完整解
        board = [ [nums[pattern(r,c)] for c in cols] for r in rows ]
        
        # 挖空部分格子作为谜题
        squares = side*side
        empties = squares * 3//4  # 挖空75%的格子
        for p in sample(range(squares),empties):
            board[p//side][p%side] = 0
            
        return board
    
    def is_valid(self, row, col, num):
        # 检查行
        if num in self.board[row]:
            return False
        
        # 检查列
        if num in [self.board[i][col] for i in range(9)]:
            return False
        
        # 检查3x3宫格
        start_row, start_col = 3 * (row // 3), 3 * (col // 3)
        for i in range(3):
            for j in range(3):
                if self.board[start_row + i][start_col + j] == num:
                    return False
        return True
    
    def make_move(self, input_str):
        if input_str.lower() == 'q':
            return 'quit'
        
        try:
            parts = list(map(int, input_str.split()))
            if len(parts) != 3:
                raise ValueError
            row, col, num = parts
            row -= 1
            col -= 1
        except:
            self.message = "输入格式错误! 示例: 1 2 3 (表示第1行第2列填3)"
            return
        
        # 验证输入范围
        if not (0 <= row < 9 and 0 <= col < 9 and 1 <= num <= 9):
            self.message = "行、列和数字都必须是1-9!"
            return
        
        # 检查是否是原始题目中的固定数字
        if self.original[row][col] != 0:
            self.message = "不能修改初始数字!"
            return
        
        # 检查数字是否有效
        if not self.is_valid(row, col, num):
            self.message = f"数字{num}在此位置无效!"
            return
        
        # 填入数字
        self.board[row][col] = num
        self.message = "输入行列和数字(如 1 2 3)或q退出:"
        
        # 检查是否完成
        if all(0 not in row for row in self.board):
            self.message = "恭喜完成数独! 按回车键返回..."
            return 'win'
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 数独游戏 ===")
        print("\n规则: 每行、每列和每个3x3宫格都包含1-9不重复")
        print("注意: 不能修改初始数字(带*的)\n")
        
        for i in range(9):
            if i % 3 == 0 and i != 0:
                print("-" * 31)
            
            row = []
            for j in range(9):
                if j % 3 == 0 and j != 0:
                    row.append("|")
                
                if self.original[i][j] != 0:  # 初始数字
                    row.append(f"*{self.board[i][j]}*")
                elif self.board[i][j] == 0:  # 空格
                    row.append(" . ")
                else:  # 玩家填入的数字
                    row.append(f" {self.board[i][j]} ")
            
            print(" ".join(row))
        
        print("\n" + self.message)
        
# ======================== 扫雷 =========================
class MinesweeperGame:
    def __init__(self):
        self.size = 10  # 10x10的游戏区域
        self.mine_count = 15  # 15颗地雷
        self.board = [[' ' for _ in range(self.size)] for _ in range(self.size)]
        self.visible = [[False for _ in range(self.size)] for _ in range(self.size)]
        self.game_over = False
        self.message = "输入坐标(如 A5)或q退出:"
        self.first_move = True
        self.generate_mines()
    
    def generate_mines(self):
        # 首次点击后生成地雷，确保第一次点击不是地雷
        pass
    
    def place_mines(self, safe_row, safe_col):
        # 放置地雷，避开安全位置
        mines_placed = 0
        while mines_placed < self.mine_count:
            row = random.randint(0, self.size-1)
            col = random.randint(0, self.size-1)
            if (row != safe_row or col != safe_col) and self.board[row][col] != 'X':
                self.board[row][col] = 'X'
                mines_placed += 1
        
        # 计算每个格子周围的地雷数
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] != 'X':
                    count = 0
                    for r in range(max(0, row-1), min(self.size, row+2)):
                        for c in range(max(0, col-1), min(self.size, col+2)):
                            if self.board[r][c] == 'X':
                                count += 1
                    if count > 0:
                        self.board[row][col] = str(count)
    
    def make_move(self, input_str):
        if input_str.lower() == 'q':
            return 'quit'
        
        # 解析输入坐标 (如 A1, B5)
        try:
            col = input_str[0].upper()
            row = int(input_str[1:]) - 1
            col = ord(col) - ord('A')
            
            if not (0 <= row < self.size and 0 <= col < self.size):
                raise ValueError
        except:
            self.message = "输入格式错误! 示例: A5 (表示第A列第5行)"
            return
        
        # 如果是第一次点击，放置地雷
        if self.first_move:
            self.place_mines(row, col)
            self.first_move = False
        
        # 检查是否踩到地雷
        if self.board[row][col] == 'X':
            self.reveal_all()
            self.game_over = True
            self.message = "踩到地雷! 游戏结束. 按回车键返回..."
            return 'lose'
        
        # 揭开格子
        self.reveal_cells(row, col)
        
        # 检查是否胜利
        if self.check_win():
            self.reveal_all()
            self.game_over = True
            self.message = "恭喜你赢了! 按回车键返回..."
            return 'win'
        
        self.message = "输入坐标(如 A5)或q退出:"
    
    def reveal_cells(self, row, col):
        # 使用递归揭开空白区域
        if not (0 <= row < self.size and 0 <= col < self.size) or self.visible[row][col]:
            return
        
        self.visible[row][col] = True
        
        if self.board[row][col] == ' ':
            for r in range(max(0, row-1), min(self.size, row+2)):
                for c in range(max(0, col-1), min(self.size, col+2)):
                    self.reveal_cells(r, c)
    
    def check_win(self):
        # 检查是否所有非地雷格子都已揭开
        for row in range(self.size):
            for col in range(self.size):
                if self.board[row][col] != 'X' and not self.visible[row][col]:
                    return False
        return True
    
    def reveal_all(self):
        # 显示所有地雷
        for row in range(self.size):
            for col in range(self.size):
                self.visible[row][col] = True
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 扫雷游戏 ===")
        print(f"\n地雷数量: {self.mine_count}")
        print("X:地雷 数字:周围地雷数\n")
        
        # 打印列标
        print("   " + " ".join([chr(65+i) for i in range(self.size)]))
        print("  +" + "-" * (2*self.size + 1) + "+")
        
        for row in range(self.size):
            # 打印行号
            print(f"{row+1:2}|", end=" ")
            
            for col in range(self.size):
                if self.visible[row][col]:
                    print(self.board[row][col], end=" ")
                else:
                    print(".", end=" ")
            print("|")
        
        print("  +" + "-" * (2*self.size + 1) + "+")
        print("\n" + self.message)
        
# ======================== 2048 =========================
class Game2048:
    def __init__(self):
        self.size = 4  # 4x4的游戏板
        self.board = [[0 for _ in range(self.size)] for _ in range(self.size)]
        self.score = 0
        self.game_over = False
        self.message = "使用WASD移动数字，q退出:"
        self.add_random_tile()
        self.add_random_tile()
    
    def add_random_tile(self):
        # 在空格子上随机添加一个2或4
        empty_cells = [(i, j) for i in range(self.size) 
                      for j in range(self.size) if self.board[i][j] == 0]
        if empty_cells:
            row, col = random.choice(empty_cells)
            self.board[row][col] = 2 if random.random() < 0.9 else 4
    
    def move(self, direction):
        moved = False
        direction = direction.lower()
        
        if direction == 'w':  # 上
            for col in range(self.size):
                for row in range(1, self.size):
                    if self.board[row][col] != 0:
                        for k in range(row, 0, -1):
                            if self.board[k-1][col] == 0:
                                self.board[k-1][col] = self.board[k][col]
                                self.board[k][col] = 0
                                moved = True
                            elif self.board[k-1][col] == self.board[k][col]:
                                self.board[k-1][col] *= 2
                                self.score += self.board[k-1][col]
                                self.board[k][col] = 0
                                moved = True
                                break
                            else:
                                break
        elif direction == 's':  # 下
            for col in range(self.size):
                for row in range(self.size-2, -1, -1):
                    if self.board[row][col] != 0:
                        for k in range(row, self.size-1):
                            if self.board[k+1][col] == 0:
                                self.board[k+1][col] = self.board[k][col]
                                self.board[k][col] = 0
                                moved = True
                            elif self.board[k+1][col] == self.board[k][col]:
                                self.board[k+1][col] *= 2
                                self.score += self.board[k+1][col]
                                self.board[k][col] = 0
                                moved = True
                                break
                            else:
                                break
        elif direction == 'a':  # 左
            for row in range(self.size):
                for col in range(1, self.size):
                    if self.board[row][col] != 0:
                        for k in range(col, 0, -1):
                            if self.board[row][k-1] == 0:
                                self.board[row][k-1] = self.board[row][k]
                                self.board[row][k] = 0
                                moved = True
                            elif self.board[row][k-1] == self.board[row][k]:
                                self.board[row][k-1] *= 2
                                self.score += self.board[row][k-1]
                                self.board[row][k] = 0
                                moved = True
                                break
                            else:
                                break
        elif direction == 'd':  # 右
            for row in range(self.size):
                for col in range(self.size-2, -1, -1):
                    if self.board[row][col] != 0:
                        for k in range(col, self.size-1):
                            if self.board[row][k+1] == 0:
                                self.board[row][k+1] = self.board[row][k]
                                self.board[row][k] = 0
                                moved = True
                            elif self.board[row][k+1] == self.board[row][k]:
                                self.board[row][k+1] *= 2
                                self.score += self.board[row][k+1]
                                self.board[row][k] = 0
                                moved = True
                                break
                            else:
                                break
        elif direction == 'q':
            return 'quit'
        else:
            self.message = "无效输入! 使用WASD或q退出:"
            return
        
        if moved:
            self.add_random_tile()
            self.check_game_over()
        
        self.message = f"分数: {self.score} 使用WASD移动数字，q退出:"
    
    def check_game_over(self):
        # 检查是否还有空格子
        if any(0 in row for row in self.board):
            return
        
        # 检查是否还能合并
        for row in range(self.size):
            for col in range(self.size):
                if (row < self.size-1 and self.board[row][col] == self.board[row+1][col]) or \
                   (col < self.size-1 and self.board[row][col] == self.board[row][col+1]):
                    return
        
        self.game_over = True
        self.message = f"游戏结束! 最终分数: {self.score} 按回车键返回..."
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 2048数字合并 ===")
        print("\n规则: 使用方向键移动数字，相同数字会合并")
        print("目标: 合并出2048!\n")
        
        for row in self.board:
            print("+" + "-----+" * self.size)
            print("|", end="")
            for num in row:
                if num == 0:
                    print("     |", end="")
                else:
                    print(f"{num:5}|", end="")
            print()
        print("+" + "-----+" * self.size)
        
        print(f"\n分数: {self.score}")
        print(self.message)
        
# ======================== 五子棋 =========================
class GomokuGame:
    def __init__(self):
        self.size = 15  # 15x15的棋盘
        self.board = [['.' for _ in range(self.size)] for _ in range(self.size)]
        self.current_player = 'X'  # X先手
        self.game_over = False
        self.message = "玩家X的回合，输入坐标(如 A15)或q退出:"
    
    def make_move(self, input_str):
        if input_str.lower() == 'q':
            return 'quit'
        
        # 解析输入坐标 (如 A1, B15)
        try:
            col = input_str[0].upper()
            row = int(input_str[1:]) - 1
            col = ord(col) - ord('A')
            
            if not (0 <= row < self.size and 0 <= col < self.size):
                raise ValueError
        except:
            self.message = f"输入格式错误! 示例: A15 (表示第A列第15行) 当前玩家: {self.current_player}"
            return
        
        # 检查位置是否已有棋子
        if self.board[row][col] != '.':
            self.message = f"该位置已有棋子! 当前玩家: {self.current_player}"
            return
        
        # 放置棋子
        self.board[row][col] = self.current_player
        
        # 检查是否获胜
        if self.check_win(row, col):
            self.game_over = True
            self.message = f"玩家 {self.current_player} 获胜! 按回车键返回..."
            return 'win'
        
        # 检查是否平局
        if all(cell != '.' for row in self.board for cell in row):
            self.game_over = True
            self.message = "平局! 按回车键返回..."
            return 'draw'
        
        # 切换玩家
        self.current_player = 'O' if self.current_player == 'X' else 'X'
        self.message = f"玩家{self.current_player}的回合，输入坐标(如 A15)或q退出:"
    
    def check_win(self, row, col):
        directions = [
            [(0, 1), (0, -1)],  # 水平
            [(1, 0), (-1, 0)],  # 垂直
            [(1, 1), (-1, -1)], # 主对角线
            [(1, -1), (-1, 1)]  # 副对角线
        ]
        
        for dir_pair in directions:
            count = 1  # 当前落子的位置
            
            for dx, dy in dir_pair:
                x, y = row + dx, col + dy
                
                while 0 <= x < self.size and 0 <= y < self.size:
                    if self.board[x][y] == self.current_player:
                        count += 1
                        x += dx
                        y += dy
                    else:
                        break
            
            if count >= 5:
                return True
        
        return False
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 五子棋游戏 ===")
        print("\n规则: 先连成五子者胜 (横、竖、斜均可)\n")
        
        # 打印列标
        print("   " + " ".join([chr(65+i) for i in range(min(26, self.size))]))
        print("  +" + "-" * (2*self.size + 1) + "+")
        
        for i in range(self.size):
            # 打印行号
            print(f"{i+1:2}|", end=" ")
            
            for j in range(self.size):
                print(self.board[i][j], end=" ")
            print("|")
        
        print("  +" + "-" * (2*self.size + 1) + "+")
        print("\n" + self.message)
        
# ======================== 文字 =========================
class TextAdventureGame:
    def __init__(self):
        self.current_room = "大厅"
        self.inventory = []
        self.health = 100
        self.game_over = False
        self.message = ""
        
        # 游戏地图和物品配置
        self.rooms = {
            "大厅": {
                "description": "你站在一个古老的大厅中央。四周墙壁上挂着褪色的挂毯。",
                "exits": {"北": "图书馆", "东": "厨房", "南": "花园"},
                "items": ["火炬"]
            },
            "图书馆": {
                "description": "满是灰尘的书架排列在房间四周。空气中弥漫着霉味。",
                "exits": {"南": "大厅", "西": "密室"},
                "items": ["古书"],
                "locked": {"西": True}
            },
            "厨房": {
                "description": "一个破旧的厨房，灶台上积满了灰尘。",
                "exits": {"西": "大厅", "北": "储藏室"},
                "items": ["小刀"]
            },
            "储藏室": {
                "description": "黑暗的小房间，堆满了箱子和罐子。",
                "exits": {"南": "厨房"},
                "items": ["钥匙"]
            },
            "花园": {
                "description": "杂草丛生的花园，中央有一口枯井。",
                "exits": {"北": "大厅", "下": "地窖"},
                "enemy": {"name": "巨鼠", "health": 30, "damage": 10}
            },
            "地窖": {
                "description": "阴暗潮湿的地窖，角落里有什么东西在发光。",
                "exits": {"上": "花园"},
                "items": ["宝藏"],
                "game_win": True
            },
            "密室": {
                "description": "隐藏的房间，墙上刻着奇怪的符号。",
                "exits": {"东": "图书馆"},
                "items": ["护身符"]
            }
        }
    
    def process_command(self, command):
        command = command.lower().strip()
        self.message = ""
        
        if command == "q":
            return "quit"
        
        if command in ["n", "北"]:
            self.move("北")
        elif command in ["s", "南"]:
            self.move("南")
        elif command in ["e", "东"]:
            self.move("东")
        elif command in ["w", "西"]:
            self.move("西")
        elif command in ["u", "上"]:
            self.move("上")
        elif command in ["d", "下"]:
            self.move("下")
        elif command.startswith("拿") or command.startswith("捡"):
            item = command[1:].strip()
            self.pick_item(item)
        elif command.startswith("用"):
            item = command[1:].strip()
            self.use_item(item)
        elif command == "i" or command == "物品":
            self.show_inventory()
        elif command == "h" or command == "帮助":
            self.show_help()
        elif command == "攻击" or command == "战斗":
            self.fight_enemy()
        else:
            self.message = "无效命令! 输入'h'查看帮助"
    
    def move(self, direction):
        room = self.rooms[self.current_room]
        
        if direction in room["exits"]:
            next_room = room["exits"][direction]
            
            # 检查门是否上锁
            if room.get("locked", {}).get(direction, False):
                self.message = f"{direction}的门被锁住了!"
                return
            
            self.current_room = next_room
            self.message = f"你进入了{next_room}"
            
            # 检查新房间是否有敌人
            if "enemy" in self.rooms[next_room]:
                self.message += f"\n房间里有一只{self.rooms[next_room]['enemy']['name']}!"
        else:
            self.message = "你不能往那个方向走!"
    
    def pick_item(self, item_name):
        room = self.rooms[self.current_room]
        
        for item in room.get("items", []):
            if item == item_name:
                self.inventory.append(item)
                room["items"].remove(item)
                self.message = f"你捡起了{item_name}"
                return
        
        self.message = f"这里没有{item_name}"
    
    def use_item(self, item_name):
        if item_name not in self.inventory:
            self.message = f"你没有{item_name}"
            return
        
        room = self.rooms[self.current_room]
        
        if item_name == "钥匙":
            for direction, locked in room.get("locked", {}).items():
                if locked:
                    room["locked"][direction] = False
                    self.inventory.remove("钥匙")
                    self.message = f"你用钥匙打开了{direction}的门"
                    return
            
            self.message = "这里没有可以开的门"
        elif item_name == "小刀":
            if "enemy" in room:
                enemy = room["enemy"]
                enemy["health"] -= 25
                self.message = f"你用刀攻击了{enemy['name']}"
                
                if enemy["health"] <= 0:
                    del room["enemy"]
                    self.message += f"\n你击败了{enemy['name']}!"
                else:
                    self.health -= enemy["damage"]
                    self.message += f"\n{enemy['name']}反击了! 你失去了{enemy['damage']}点生命值"
                    
                    if self.health <= 0:
                        self.message += "\n你被击败了! 游戏结束."
                        self.game_over = True
            else:
                self.message = "这里没有敌人可以攻击"
        else:
            self.message = f"你不知道如何使用{item_name}"
    
    def fight_enemy(self):
        room = self.rooms[self.current_room]
        
        if "enemy" not in room:
            self.message = "这里没有敌人"
            return
        
        enemy = room["enemy"]
        enemy["health"] -= 10
        self.message = f"你攻击了{enemy['name']}"
        
        if enemy["health"] <= 0:
            del room["enemy"]
            self.message += f"\n你击败了{enemy['name']}!"
        else:
            self.health -= enemy["damage"]
            self.message += f"\n{enemy['name']}反击了! 你失去了{enemy['damage']}点生命值"
            
            if self.health <= 0:
                self.message += "\n你被击败了! 游戏结束."
                self.game_over = True
    
    def show_inventory(self):
        if not self.inventory:
            self.message = "你的物品栏是空的"
        else:
            self.message = "物品栏: " + ", ".join(self.inventory)
    
    def show_help(self):
        self.message = """可用命令:
n/北, s/南, e/东, w/西, u/上, d/下 - 移动
拿[物品名] - 捡起物品
用[物品名] - 使用物品
i/物品 - 查看物品栏
攻击/战斗 - 攻击敌人
h/帮助 - 显示帮助
q - 退出游戏"""
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 文字冒险游戏 ===")
        print(f"\n生命值: {self.health}")
        
        room = self.rooms[self.current_room]
        print(f"\n{room['description']}")
        
        # 显示出口
        exits = "出口: " + ", ".join(room["exits"].keys())
        print(f"\n{exits}")
        
        # 显示物品
        if "items" in room and room["items"]:
            items = "物品: " + ", ".join(room["items"])
            print(f"\n{items}")
        
        # 显示敌人
        if "enemy" in room:
            enemy = room["enemy"]
            print(f"\n敌人: {enemy['name']} (生命值: {enemy['health']})")
        
        # 游戏胜利检查
        if room.get("game_win", False) and "宝藏" in self.inventory:
            print("\n你找到了宝藏! 恭喜通关!")
            self.game_over = True
        
        # 显示消息
        if self.message:
            print(f"\n{self.message}")
        
        if self.game_over:
            print("\n按回车键返回主菜单...")
        else:
            print("\n输入命令(或'h'查看帮助):")
               
# ======================== 游戏渲染器 ========================
class GameRenderer:
    @staticmethod
    def clear_screen():
        os.system('cls' if os.name == 'nt' else 'clear')
    
    @staticmethod
    def render_tower(game_state):
        GameRenderer.clear_screen()
        
        print("=== 100层魔塔 ===")
        print(f"楼层: {game_state.floor}/100")
        
        print(f"\nHP: {game_state.hp}/{game_state.max_hp}")
        print(f"攻击: {game_state.attack}  防御: {game_state.defense}")
        print(f"金币: {game_state.gold}")
        
        print(f"\n钥匙: 黄×{game_state.keys['yellow']}  蓝×{game_state.keys['blue']}  红×{game_state.keys['red']}")
        
        if game_state.has_sword:
            print("装备: 传说宝剑")
        if game_state.has_shield:
            print("装备: 魔法盾牌")
        
        print("\n地图:")
        for y in range(10):
            row = []
            for x in range(10):
                if x == game_state.player_x and y == game_state.player_y:
                    row.append('@')
                else:
                    tile = game_state.floors[game_state.floor][y][x]
                    if tile == 0: row.append('.')
                    elif tile == 1: row.append('#')
                    elif tile == 2: row.append('E')
                    elif tile == 3: row.append('$')
                    elif tile == 4: row.append('↑')
                    elif tile == 5: row.append('↓')
                    elif tile == 6: row.append('B')
                    elif tile == 7: row.append('D')
                    elif tile == 8: row.append('M')
                    else: row.append('?')
            print(' '.join(row))
        
        if game_state.message:
            print(f"\n{game_state.message}")
        
        print("\n控制: w(上) a(左) s(下) d(右) q(返回主菜单)")

# ========================石头剪刀布========================
class RockPaperScissors:
    def __init__(self):
        self.choices = ['石头', '剪刀', '布']
        self.player_score = 0
        self.computer_score = 0
        self.rounds = 0
        self.message = "输入1(石头)、2(剪刀)、3(布)或q(退出):"
    
    def play_round(self, player_choice):
        if player_choice.lower() == 'q':
            return 'quit'
        
        try:
            player_index = int(player_choice) - 1
            if player_index not in [0, 1, 2]:
                self.message = "请输入1-3的数字或q退出!"
                return
        except ValueError:
            self.message = "请输入有效的数字(1-3)或q退出!"
            return
        
        computer_index = random.randint(0, 2)
        player_choice = self.choices[player_index]
        computer_choice = self.choices[computer_index]
        
        self.rounds += 1
        result = self.determine_winner(player_index, computer_index)
        
        if result == "玩家赢":
            self.player_score += 1
        elif result == "电脑赢":
           self.computer_score += 1
        
        self.message = f"你出{player_choice}, 电脑出{computer_choice}\n"
        self.message += f"结果: {result}\n"
        self.message += f"比分: 玩家 {self.player_score} - {self.computer_score} 电脑\n"
        self.message += "输入1(石头)、2(剪刀)、3(布)或q(退出):"
        
        return result
    
    def determine_winner(self, player, computer):
        if player == computer:
            return "平局"
        elif (player - computer) % 3 == 1:
            return "电脑赢"
        else:
            return "玩家赢"
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 石头剪刀布 ===")
        print(f"回合: {self.rounds}")
        print(f"比分: 玩家 {self.player_score} - {self.computer_score} 电脑")
        print("\n1. 石头")
        print("2. 剪刀")
        print("3. 布")
        print("\n" + self.message)
        
# ======================== 华容道3*3 =========================
class NumberPuzzleGame:
    def __init__(self):
        self.size = 3  # 3x3的拼图
        self.board = [['1', '2', '3'], 
                     ['4', '5', '6'], 
                     ['7', '8', ' ']]  # 空格用' '表示
        self.empty_pos = (2, 2)  # 初始空格位置
        self.moves = 0
        self.game_over = False
        self.message = "使用WASD移动空格或q退出:"
        
        # 打乱拼图
        self.shuffle_board()
    
    def shuffle_board(self):
        # 执行随机移动来打乱拼图
        for _ in range(50):
            directions = ['w', 'a', 's', 'd']
            random.shuffle(directions)
            for direction in directions:
                if self.can_move(direction):
                    self.move_empty(direction)
                    break
    
    def can_move(self, direction):
        """检查空格能否向指定方向移动"""
        row, col = self.empty_pos
        if direction == 'w':  # 空格上移
            return row > 0
        elif direction == 's':  # 空格下移
            return row < self.size - 1
        elif direction == 'a':  # 空格左移
            return col > 0
        elif direction == 'd':  # 空格右移
            return col < self.size - 1
        return False
    
    def move_empty(self, direction):
        """移动空格到指定方向"""
        if direction.lower() == 'q':
            return 'quit'
        
        if not self.can_move(direction):
            self.message = "不能向那个方向移动! 使用WASD或q退出:"
            return
        
        row, col = self.empty_pos
        new_row, new_col = row, col
        
        # 计算新位置
        if direction == 'w':  # 空格上移
            new_row -= 1
        elif direction == 's':  # 空格下移
            new_row += 1
        elif direction == 'a':  # 空格左移
            new_col -= 1
        elif direction == 'd':  # 空格右移
            new_col += 1
        
        # 交换空格和相邻数字
        self.board[row][col], self.board[new_row][new_col] = self.board[new_row][new_col], self.board[row][col]
        self.empty_pos = (new_row, new_col)
        self.moves += 1
        
        # 检查是否完成
        if self.check_win():
            self.game_over = True
            self.message = f"恭喜完成! 共用了{self.moves}步。按回车键返回..."
        else:
            self.message = f"移动次数: {self.moves} 使用WASD移动空格或q退出:"
    
    def check_win(self):
        # 检查是否按顺序排列
        flat_board = [cell for row in self.board for cell in row]
        return flat_board == ['1', '2', '3', '4', '5', '6', '7', '8', ' ']
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 数字华容道 ===")
        print("\n将数字按顺序排列(1-8)，使用WASD移动:")
        
        for i, row in enumerate(self.board):
            print(" " + "+---" * self.size + "+")
            print(str(i+1) + " |", end="")
            for cell in row:
                print(f" {cell if cell != ' ' else ' '} |", end="")
            print()
        print(" " + "+---" * self.size + "+")
        print("    A   B   C  (列)")
        
        print("\n控制说明:")
        print("W: 上  A: 左")
        print("S: 下  D: 右")
        print("\n" + self.message)
        
# ======================== 数字猜位置 =========================
class NumberPositionGame:
    def __init__(self):
        self.secret = self.generate_secret()
        self.attempts = 0
        self.max_attempts = 10
        self.game_over = False
        self.message = "猜一个4位不重复数字(如1234)或q退出:"
    
    def generate_secret(self):
        """生成4位不重复数字的密码"""
        digits = list("0123456789")
        random.shuffle(digits)
        return ''.join(digits[:4])
    
    def check_guess(self, guess):
        """检查猜测并返回AB结果"""
        a = b = 0
        for i in range(4):
            if guess[i] == self.secret[i]:
                a += 1
            elif guess[i] in self.secret:
                b += 1
        return a, b
    
    def make_guess(self, guess):
        if guess.lower() == 'q':
            return 'quit'
        
        # 验证输入
        if len(guess) != 4 or not guess.isdigit() or len(set(guess)) != 4:
            self.message = "请输入4位不重复的数字! 如1234"
            return
        
        self.attempts += 1
        a, b = self.check_guess(guess)
        
        if a == 4:
            self.message = f"恭喜! 你猜对了! 用了{self.attempts}次尝试\n密码是{self.secret}\n按回车键返回..."
            self.game_over = True
            return 'win'
        else:
            self.message = f"{guess}: {a}A{b}B\n"
            self.message += f"剩余尝试次数: {self.max_attempts - self.attempts}\n"
            
            if self.attempts >= self.max_attempts:
                self.message += f"游戏结束! 正确答案是{self.secret}\n按回车键返回..."
                self.game_over = True
            else:
                self.message += "再猜一次(或q退出):"
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 数字猜位置 ===")
        print("\n规则: 猜一个4位不重复数字")
        print("反馈: A表示数字位置都正确，B表示数字正确但位置不对")
        print(f"\n尝试次数: {self.attempts}/{self.max_attempts}")
        print("\n" + self.message)
        
# ======================== 排序 =========================
class NumberSortGame:
    def __init__(self):
        self.numbers = self.generate_random_sequence()
        self.steps = 0
        self.min_steps = self.calculate_min_steps()
        self.game_over = False
        self.message = "输入要交换的位置(如 1 2)或q退出:"

    def generate_random_sequence(self):
        """生成一个乱序的1-4数字序列"""
        seq = [1, 2, 3, 4]
        swaps = [(0, 1), (1, 2), (2, 3), (0, 3)]  # 几种可能的交换
        for _ in range(10):
            idx = _ % 4
            i, j = swaps[idx]
            seq[i], seq[j] = seq[j], seq[i]
        return seq

    def calculate_min_steps(self):
        """计算最少需要的交换次数（冒泡排序思想）"""
        seq = self.numbers.copy()
        steps = 0
        for i in range(len(seq)):
            for j in range(len(seq) - 1):
                if seq[j] > seq[j + 1]:
                    seq[j], seq[j + 1] = seq[j + 1], seq[j]
                    steps += 1
        return steps

    def swap_numbers(self, pos1, pos2):
        """交换两个相邻数字"""
        if abs(pos1 - pos2) != 1:
            self.message = "只能交换相邻数字！"
            return False
        self.numbers[pos1], self.numbers[pos2] = self.numbers[pos2], self.numbers[pos1]
        self.steps += 1
        return True

    def check_win(self):
        """检查是否排序完成"""
        for i in range(len(self.numbers) - 1):
            if self.numbers[i] > self.numbers[i + 1]:
                return False
        return True

    def make_move(self, input_str):
        if input_str.lower() == 'q':
            return 'quit'
        
        try:
            pos1, pos2 = map(int, input_str.split())
            pos1 -= 1
            pos2 -= 1
            if not (0 <= pos1 < 4 and 0 <= pos2 < 4):
                raise ValueError
        except:
            self.message = "输入格式错误！示例: 1 2 (交换第1和第2个数字)"
            return
        
        if not self.swap_numbers(pos1, pos2):
            return
        
        if self.check_win():
            self.message = f"恭喜！排序完成！\n步数: {self.steps} (最少需要 {self.min_steps} 步)\n按回车键返回..."
            self.game_over = True
            return 'win'
        else:
            self.message = f"当前序列: {self.numbers}\n步数: {self.steps}\n继续交换(或q退出):"

    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 数字排序游戏 ===")
        print("\n规则: 交换相邻数字，使其升序排列")
        print(f"当前序列: {self.numbers}")
        print(f"步数: {self.steps} (最少需要 {self.min_steps} 步)")
        print("\n" + self.message)
        
# ======================== 猜单词 =========================
class WordSpellingGame:
    def __init__(self):
        self.words = [
            "apple", "banana", "computer", "driver", "elephant",
            "fuck", "give", "have", "in", "just",
            "know", "light", "minecraft", "no", "off",
            "plus", "quickly", "read", "sun", "time"
        ]
        self.current_word = ""
        self.player_input = ""
        self.score = 0
        self.time_limit = 30  # 30秒时间限制
        self.start_time = 0
        self.game_over = False
        self.message = "准备好开始了吗? (按回车键开始)"
    
    def start_game(self):
        self.current_word = random.choice(self.words)
        self.player_input = ""
        self.start_time = time.time()
        self.message = f"拼写这个单词: {self.current_word.upper()}"
    
    def check_spelling(self):
        if self.player_input.lower() == self.current_word:
            self.score += 1
            self.message = f"正确! 当前得分: {self.score}\n下一个单词..."
            time.sleep(1)  # 短暂暂停让玩家看到反馈
            self.current_word = random.choice(self.words)
            self.player_input = ""
            self.message = f"拼写这个单词: {self.current_word.upper()}"
        else:
            self.message = f"错误! 应该是: {self.current_word.upper()}\n当前得分: {self.score}"
            time.sleep(1)  # 短暂暂停让玩家看到反馈
            self.current_word = random.choice(self.words)
            self.player_input = ""
            self.message = f"拼写这个单词: {self.current_word.upper()}"
    
    def update(self):
        if self.start_time > 0 and time.time() - self.start_time > self.time_limit:
            self.game_over = True
            self.message = f"时间到! 最终得分: {self.score}\n按回车键返回主菜单"
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 单词拼写挑战 ===")
        print(f"\n得分: {self.score}")
        
        if self.start_time > 0:
            remaining_time = max(0, self.time_limit - (time.time() - self.start_time))
            print(f"剩余时间: {int(remaining_time)}秒")
        
        print("\n" + self.message)
        
        if not self.game_over and self.start_time > 0:
            print(f"\n你的输入: {self.player_input}")
            
# ======================== 颜色匹配 =========================
class ColorMatchGame:
    def __init__(self):
        self.colors = {
            "红": "\033[91m",    # 红色
            "绿": "\033[92m",    # 绿色
            "黄": "\033[93m",    # 黄色
            "蓝": "\033[94m",    # 蓝色
            "紫": "\033[95m",    # 紫色
            "青": "\033[96m"     # 青色
        }
        self.color_names = list(self.colors.keys())
        self.current_color = ""
        self.current_word = ""
        self.score = 0
        self.rounds = 0
        self.max_rounds = 10
        self.game_over = False
        self.message = "颜色匹配挑战! 按颜色名称输入(红/绿/黄/蓝/紫/青)"
    
    def start_round(self):
        # 随机选择颜色和文字(可能不一致)
        self.current_color = random.choice(self.color_names)
        self.current_word = random.choice(self.color_names)
        self.rounds += 1
        
        # 50%概率文字和颜色匹配
        if random.random() > 0.5:
            self.current_word = self.current_color
    
    def check_answer(self, answer):
        # 玩家需要根据文字颜色回答，而不是文字内容
        if answer == self.current_color:
            self.score += 1
            self.message = "正确! " + self.colors[self.current_color] + self.current_word + "\033[0m"
        else:
            self.message = "错误! 颜色是: " + self.colors[self.current_color] + self.current_color + "\033[0m"
        
        if self.rounds < self.max_rounds:
            time.sleep(1)  # 短暂显示结果
            self.start_round()
        else:
            self.game_over = True
            self.message = f"游戏结束! 得分: {self.score}/{self.max_rounds}"
    
    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 颜色匹配挑战 ===")
        print(f"\n回合: {self.rounds}/{self.max_rounds}  得分: {self.score}")
        
        if not self.game_over and self.rounds > 0:
            # 显示彩色文字
            print("\n文字颜色是: " + self.colors[self.current_color] + self.current_word + "\033[0m")
        
        print("\n" + self.message)
        
        if not self.game_over:
            print("\n可选颜色: 红, 绿, 黄, 蓝, 紫, 青")
            print("输入颜色名称或q退出:")
            
# ======================== 消除小游戏 =========================
class ShapeEliminationGame:
    def __init__(self):
        self.width = 8
        self.height = 8
        self.shapes = ['▲', '■', '●', '◆', '★', '♥']
        self.board = []
        self.selected = None
        self.score = 0
        self.time_limit = 60  # 60秒游戏时间
        self.start_time = 0
        self.game_over = False
        self.message = "形状消除游戏! 选择两个相同形状交换消除"
        self.initialize_board()

    def initialize_board(self):
        # 初始化游戏板，确保没有初始匹配
        while True:
            self.board = [[random.choice(self.shapes) for _ in range(self.width)] 
                         for _ in range(self.height)]
            if not self.find_matches():
                break

    def find_matches(self):
        # 检查当前板上的匹配项
        matches = set()
        
        # 检查水平匹配
        for y in range(self.height):
            for x in range(self.width - 2):
                if self.board[y][x] == self.board[y][x+1] == self.board[y][x+2]:
                    matches.add((y, x))
                    matches.add((y, x+1))
                    matches.add((y, x+2))
        
        # 检查垂直匹配
        for x in range(self.width):
            for y in range(self.height - 2):
                if self.board[y][x] == self.board[y+1][x] == self.board[y+2][x]:
                    matches.add((y, x))
                    matches.add((y+1, x))
                    matches.add((y+2, x))
        
        return matches

    def swap_tiles(self, pos1, pos2):
        y1, x1 = pos1
        y2, x2 = pos2
        self.board[y1][x1], self.board[y2][x2] = self.board[y2][x2], self.board[y1][x1]
        
        # 检查交换后是否有匹配
        matches = self.find_matches()
        if matches:
            self.process_matches(matches)
            return True
        else:
            # 如果没有匹配，交换回来
            self.board[y1][x1], self.board[y2][x2] = self.board[y2][x2], self.board[y1][x1]
            return False

    def process_matches(self, matches):
        # 处理匹配项并计算分数
        match_count = len(matches)
        self.score += match_count * 10
        
        # 移除匹配的形状
        for y, x in matches:
            self.board[y][x] = None
        
        # 让上面的形状下落
        self.drop_tiles()
        
        # 填充新的形状
        self.fill_empty()
        
        # 检查连锁反应
        new_matches = self.find_matches()
        if new_matches:
            self.process_matches(new_matches)

    def drop_tiles(self):
        # 让形状下落填补空缺
        for x in range(self.width):
            column = [self.board[y][x] for y in range(self.height) if self.board[y][x] is not None]
            column = [None] * (self.height - len(column)) + column
            for y in range(self.height):
                self.board[y][x] = column[y]

    def fill_empty(self):
        # 填充顶部的空位
        for x in range(self.width):
            for y in range(self.height):
                if self.board[y][x] is None:
                    self.board[y][x] = random.choice(self.shapes)

    def update(self):
        # 更新时间检查
        if self.start_time > 0 and time.time() - self.start_time > self.time_limit:
            self.game_over = True
            self.message = f"时间到! 最终得分: {self.score}\n按回车键返回主菜单"

    def render(self):
        os.system('cls' if os.name == 'nt' else 'clear')
        print("=== 形状消除 ===")
        print(f"\n得分: {self.score}")
        
        if self.start_time > 0:
            remaining_time = max(0, self.time_limit - (time.time() - self.start_time))
            print(f"剩余时间: {int(remaining_time)}秒")
        
        print("\n" + self.message)
        print("\n   " + " ".join(str(i) for i in range(self.width)))
        print("  +" + "--" * self.width + "+")
        
        for y in range(self.height):
            print(f"{y} |", end=" ")
            for x in range(self.width):
                if self.selected == (y, x):
                    print(f"\033[93m{self.board[y][x]}\033[0m", end=" ")  # 黄色高亮选中项
                else:
                    print(self.board[y][x], end=" ")
            print("|")
        
        print("  +" + "--" * self.width + "+")
        print("\n输入坐标(如 3 4)选择形状，或q退出:")
        
# ======================== 主菜单和游戏循环 ========================
def show_main_menu():
    os.system('cls' if os.name == 'nt' else 'clear')
    print("=== 游戏主菜单 ===")
    print("1. 100层魔塔")
    print("2. 贪吃蛇")
    print("3. 猜数字")
    print("4. 石头剪刀布")
    print("5. 井字棋")
    print("6. 记忆游戏")
    print("7. 4*4华容道")
    print("8. 迷宫探险")
    print("9. 数独游戏")
    print("10. 扫雷游戏")
    print("11. 2048")
    print("12. 五子棋")
    print("13. 文字游戏")
    print("14. 3*3华容道")
    print("15. 数字猜位置")
    print("16. 数字排序")
    print("17. 单词游戏")
    print("18. 颜色游戏")
    print("19. 消除游戏")
    print("20. 退出游戏")
    print("\n请选择游戏(输入1-20):")

def main():
    while True:
        show_main_menu()
        choice = input("> ")
        
        if choice == '1':
            # 魔塔游戏
            tower_game = GameState()
            renderer = GameRenderer()
            
            while True:
                renderer.render_tower(tower_game)
                
                cmd = input("> ").lower()
                
                if cmd == 'q':
                    break
                
                if cmd == 'w':
                    tower_game.move_player(0, -1)
                elif cmd == 'a':
                    tower_game.move_player(-1, 0)
                elif cmd == 's':
                    tower_game.move_player(0, 1)
                elif cmd == 'd':
                    tower_game.move_player(1, 0)
                else:
                    tower_game.message = "无效命令! 使用w,a,s,d移动"
        
        elif choice == '2':
            # 贪吃蛇游戏
            snake_game = SnakeGame()
            
            while True:
                snake_game.render()
                
                cmd = input("> ").lower()
                
                if cmd == 'q':
                    break
                
                if not snake_game.game_over:
                    if cmd == 'w':
                        snake_game.change_direction(0, -1)
                    elif cmd == 'a':
                        snake_game.change_direction(-1, 0)
                    elif cmd == 's':
                        snake_game.change_direction(0, 1)
                    elif cmd == 'd':
                        snake_game.change_direction(1, 0)
                    
                    snake_game.update()
                else:
                    if cmd == 'q':
                        break
        
        elif choice == '3':
            # 猜数字游戏
            guess_game = GuessNumberGame()
            
            while True:
                guess_game.render()
                
                if guess_game.game_over:
                    input()
                    break
                
                cmd = input("> ").lower()
                
                if cmd == 'q':
                    break
                
                guess_game.make_guess(cmd)
        
        elif choice == '4':
            # 石头剪刀布游戏
            rps_game = RockPaperScissors()
            
            while True:
                rps_game.render()
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                
                rps_game.play_round(cmd)
        
        elif choice == '5':
            # 井字棋游戏
            ttt_game = TicTacToeGame()
            
            while True:
                ttt_game.render()
                
                if ttt_game.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                ttt_game.make_move(cmd)
        
        elif choice == '6':
           # 记忆翻牌游戏
           mem_game = MemoryGame()
    
           while True:
               mem_game.render()
        
               if mem_game.game_over:
                    input()
                    break
        
               cmd = input("> ")
        
               if cmd.lower() == 'q':
                    break
            
               mem_game.flip_card(cmd)
        
        elif choice == '7':
            # 数字华容道游戏
            puzzle_game = SlidePuzzleGame()
            
            while True:
                puzzle_game.render()
                
                if puzzle_game.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                puzzle_game.move_tile(cmd)
                
        elif choice == '8':
            # 迷宫探险游戏
            maze_game = MazeGame()
            
            while True:
                maze_game.render()
                
                if maze_game.player_pos == maze_game.exit_pos:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                result = maze_game.move_player(cmd)
                if result == 'win':
                    input()
                    break
                    
        elif choice == '9':
            # 数独游戏
            sudoku_game = SudokuGame()
                
            while True:
                sudoku_game.render()
                
                if all(0 not in row for row in sudoku_game.board):
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                result = sudoku_game.make_move(cmd)
                if result == 'win':
                    input()
                    break
                    
        elif choice == '10':
            # 扫雷游戏
            minesweeper = MinesweeperGame()
            
            while True:
                minesweeper.render()
                
                if minesweeper.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                result = minesweeper.make_move(cmd)
                if result in ['win', 'lose']:
                    input()
                    break 
                    
        elif choice == '11':
            # 2048游戏
            game2048 = Game2048()
            
            while True:
                game2048.render()
                
                if game2048.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                result = game2048.move(cmd)
                if result == 'quit':
                    break
                    
        elif choice == '12':
            # 五子棋游戏
            gomoku = GomokuGame()
            
            while True:
                gomoku.render()
                
                if gomoku.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                result = gomoku.make_move(cmd)
                if result in ['win', 'draw']:
                    input()
                    break
                    
        elif choice == '13':
            # 文字冒险游戏
            adventure = TextAdventureGame()
            
            while True:
                adventure.render()
                
                if adventure.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                adventure.process_command(cmd)
                
        elif choice == '14':
            # 数字华容道游戏
            puzzle_game = NumberPuzzleGame()
            
            while True:
                puzzle_game.render()
                
                if puzzle_game.game_over:
                    input()
                    break
                    
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                puzzle_game.move_empty(cmd)
                
        elif choice == '15':
            # 数字猜位置游戏
            np_game = NumberPositionGame()
            
            while True:
                np_game.render()
                
                if np_game.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                np_game.make_guess(cmd)
                
        elif choice == '16':
            # 数字排序游戏
            sort_game = NumberSortGame()
            
            while True:
                sort_game.render()
                
                if sort_game.game_over:
                    input()
                    break
                    
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                    
                sort_game.make_move(cmd)
                
        elif choice == '17':
            # 单词拼写游戏
            word_game = WordSpellingGame()
            
            while True:
                word_game.render()
                word_game.update()
                
                if word_game.game_over:
                    input()
                    break
                
                cmd = input("> ")
                
                if cmd.lower() == 'q':
                    break
                
                if word_game.start_time == 0:
                    # 游戏尚未开始，按回车开始
                    word_game.start_game()
                else:
                    # 游戏进行中，接收玩家输入
                    word_game.player_input = cmd
                    word_game.check_spelling()
                    
        elif choice == '18':
            # 颜色匹配游戏
            color_game = ColorMatchGame()
            color_game.start_round()  # 开始第一轮
            
            while True:
                color_game.render()
                
                if color_game.game_over:
                    input("\n按回车键返回主菜单...")
                    break
                
                cmd = input("> ").strip()
                
                if cmd.lower() == 'q':
                    break
                
                if cmd in color_game.color_names:
                    color_game.check_answer(cmd)
                else:
                    color_game.message = "无效输入! 请输入颜色名称"
                    
        elif choice == '19':
            # 形状消除游戏
            shape_game = ShapeEliminationGame()
            shape_game.start_time = time.time()  # 开始计时
            
            while True:
                shape_game.update()
                shape_game.render()
                
                if shape_game.game_over:
                    input()
                    break
                
                cmd = input("> ").lower()
                
                if cmd == 'q':
                    break
                
                try:
                    parts = list(map(int, cmd.split()))
                    if len(parts) == 2:
                        y, x = parts
                        if 0 <= y < shape_game.height and 0 <= x < shape_game.width:
                            if shape_game.selected is None:
                                shape_game.selected = (y, x)
                                shape_game.message = f"已选择({y},{x})，请选择另一个形状"
                            else:
                                prev_y, prev_x = shape_game.selected
                                # 检查是否相邻
                                if (abs(y - prev_y) == 1 and x == prev_x) or \
                                   (abs(x - prev_x) == 1 and y == prev_y):
                                    if shape_game.swap_tiles((prev_y, prev_x), (y, x)):
                                        shape_game.message = "匹配成功! 继续选择形状"
                                    else:
                                        shape_game.message = "没有形成匹配，请重试"
                                else:
                                    shape_game.message = "只能选择相邻的形状交换"
                                shape_game.selected = None
                        else:
                            shape_game.message = "坐标超出范围!"
                    else:
                        shape_game.message = "请输入两个数字坐标(如 3 4)"
                except ValueError:
                    shape_game.message = "无效输入! 请输入坐标(如 3 4)或q退出"
        
        elif choice == '20':
            print("退出游戏...")
            break
        
        else:
            input("无效选择，按回车键继续...")               

if __name__ == "__main__":
    main()