#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <ncurses.h>

using namespace std;

const int MAP_W = 60;
const int MAP_H = 20;
const int WIN_SCORE = 12;

struct Item
{
    int x;
    int y;
    char type;
};

struct Enemy
{
    int x;
    int y;
    int health;
    int attack;
    char symbol;
};

struct Player
{
    int x;
    int y;
    int health;
    int maxHealth;
    int attack;
    int defense;
    int wood;
    int stone;
    int food;
    int gold;
    int walls;
    int day;
    int kills;
    int score;
};

bool inBounds(int x, int y)
{
    return x >= 0 && x < MAP_W && y >= 0 && y < MAP_H;
}

void resetGame(Player& player, vector<Item>& items, vector<Enemy>& enemies, bool wallMap[MAP_H][MAP_W])
{
    player.x = MAP_W / 2;
    player.y = MAP_H / 2;
    player.health = 100;
    player.maxHealth = 100;
    player.attack = 12;
    player.defense = 6;
    player.wood = 0;
    player.stone = 0;
    player.food = 4;
    player.gold = 0;
    player.walls = 0;
    player.day = 1;
    player.kills = 0;
    player.score = 0;

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            wallMap[y][x] = false;
        }
    }

    items.clear();
    enemies.clear();

    for (int i = 0; i < 24; i++)
    {
        Item item;
        item.x = rand() % MAP_W;
        item.y = rand() % MAP_H;
        char types[] = {'W', 'S', 'F', 'G'};
        item.type = types[rand() % 4];
        items.push_back(item);
    }

    for (int i = 0; i < 6; i++)
    {
        Enemy enemy;
        enemy.x = rand() % MAP_W;
        enemy.y = rand() % MAP_H;
        enemy.health = 18 + rand() % 20;
        enemy.attack = 6 + rand() % 8;
        enemy.symbol = 'E';
        enemies.push_back(enemy);
    }
}

void drawMap(const Player& player, const vector<Item>& items, const vector<Enemy>& enemies, const bool wallMap[MAP_H][MAP_W], const string& message)
{
    clear();

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            bool drawn = false;

            for (const auto& item : items)
            {
                if (item.x == x && item.y == y)
                {
                    mvaddch(y, x, item.type);
                    drawn = true;
                    break;
                }
            }

            if (drawn)
            {
                continue;
            }

            for (const auto& enemy : enemies)
            {
                if (enemy.x == x && enemy.y == y)
                {
                    mvaddch(y, x, enemy.symbol);
                    drawn = true;
                    break;
                }
            }

            if (drawn)
            {
                continue;
            }

            if (wallMap[y][x])
            {
                mvaddch(y, x, '#');
                continue;
            }

            mvaddch(y, x, '.');
        }
    }

    mvaddch(player.y, player.x, '@');

    mvprintw(0, MAP_W + 2, "FOREVER WORLD");
    mvprintw(1, MAP_W + 2, "Health: %d/%d", player.health, player.maxHealth);
    mvprintw(2, MAP_W + 2, "Attack: %d", player.attack);
    mvprintw(3, MAP_W + 2, "Defense: %d", player.defense);
    mvprintw(4, MAP_W + 2, "Wood: %d", player.wood);
    mvprintw(5, MAP_W + 2, "Stone: %d", player.stone);
    mvprintw(6, MAP_W + 2, "Food: %d", player.food);
    mvprintw(7, MAP_W + 2, "Gold: %d", player.gold);
    mvprintw(8, MAP_W + 2, "Walls: %d", player.walls);
    mvprintw(9, MAP_W + 2, "Day: %d", player.day);
    mvprintw(10, MAP_W + 2, "Kills: %d", player.kills);
    mvprintw(11, MAP_W + 2, "Score: %d/%d", player.score, WIN_SCORE);

    mvprintw(MAP_H - 1, 0, "%s", message.c_str());
    refresh();
}

void addResource(Player& player, char type)
{
    if (type == 'W')
    {
        player.wood += 2;
    }
    else if (type == 'S')
    {
        player.stone += 2;
    }
    else if (type == 'F')
    {
        player.food += 3;
    }
    else if (type == 'G')
    {
        player.gold += 2;
    }

    player.score++;
}

void moveEnemyToward(Enemy& enemy, const Player& player)
{
    int dx = player.x - enemy.x;
    int dy = player.y - enemy.y;

    if (abs(dx) > abs(dy))
    {
        enemy.x += (dx > 0) ? 1 : -1;
    }
    else if (dy != 0)
    {
        enemy.y += (dy > 0) ? 1 : -1;
    }

    if (!inBounds(enemy.x, enemy.y))
    {
        enemy.x = max(0, min(MAP_W - 1, enemy.x));
        enemy.y = max(0, min(MAP_H - 1, enemy.y));
    }
}

void attackEnemy(Player& player, vector<Enemy>& enemies)
{
    for (size_t i = 0; i < enemies.size(); ++i)
    {
        if (enemies[i].x == player.x && enemies[i].y == player.y)
        {
            int damage = max(1, player.attack + rand() % 6 - enemies[i].health / 10);
            enemies[i].health -= damage;

            if (enemies[i].health <= 0)
            {
                player.kills++;
                player.gold += 2;
                enemies.erase(enemies.begin() + i);
                return;
            }

            int enemyDamage = max(1, enemies[i].attack - player.defense + rand() % 4);
            player.health -= enemyDamage;
            return;
        }
    }
}

bool handlePlayerMove(Player& player, vector<Item>& items, vector<Enemy>& enemies, bool wallMap[MAP_H][MAP_W], int dx, int dy, string& message)
{
    int newX = player.x + dx;
    int newY = player.y + dy;

    if (!inBounds(newX, newY))
    {
        message = "You hit the edge of the world.";
        return false;
    }

    if (wallMap[newY][newX])
    {
        message = "A wall blocks your path.";
        return false;
    }

    player.x = newX;
    player.y = newY;

    for (size_t i = 0; i < items.size(); ++i)
    {
        if (items[i].x == player.x && items[i].y == player.y)
        {
            addResource(player, items[i].type);
            items.erase(items.begin() + i);
            message = "Resource collected!";
            break;
        }
    }

    if (!enemies.empty())
    {
        for (const auto& enemy : enemies)
        {
            if (enemy.x == player.x && enemy.y == player.y)
            {
                attackEnemy(player, enemies);
                message = "You fight a monster!";
                break;
            }
        }
    }

    return true;
}

void buildWall(Player& player, bool wallMap[MAP_H][MAP_W], string& message)
{
    if (player.wood < 3 || player.stone < 2)
    {
        message = "Need 3 wood and 2 stone to build a wall.";
        return;
    }

    int x = player.x;
    int y = player.y;

    if (wallMap[y][x])
    {
        message = "There is already a wall there.";
        return;
    }

    player.wood -= 3;
    player.stone -= 2;
    wallMap[y][x] = true;
    player.walls++;
    message = "Wall built!";
}

void startNewDay(Player& player, bool wallMap[MAP_H][MAP_W], vector<Item>& items, vector<Enemy>& enemies)
{
    player.day += 1;
    player.food = max(0, player.food - 1);
    player.health = min(player.maxHealth, player.health + 8);

    if (player.food <= 0)
    {
        player.health -= 10;
    }

    if (rand() % 3 == 0)
    {
        Enemy enemy;
        enemy.x = rand() % MAP_W;
        enemy.y = rand() % MAP_H;
        enemy.health = 20 + player.day * 3;
        enemy.attack = 8 + player.day;
        enemy.symbol = 'E';
        enemies.push_back(enemy);
    }

    if (items.size() < 20)
    {
        Item item;
        item.x = rand() % MAP_W;
        item.y = rand() % MAP_H;
        char types[] = {'W', 'S', 'F', 'G'};
        item.type = types[rand() % 4];
        items.push_back(item);
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    Player player;
    vector<Item> items;
    vector<Enemy> enemies;
    bool wallMap[MAP_H][MAP_W];

    resetGame(player, items, enemies, wallMap);

    string message = "The wilds are waiting. Gather resources and survive.";
    bool running = true;
    int frameCounter = 0;

    while (running)
    {
        drawMap(player, items, enemies, wallMap, message);

        int ch = getch();
        bool moved = false;

        if (ch == 'q')
        {
            running = false;
        }
        else if (ch == 'w')
        {
            moved = handlePlayerMove(player, items, enemies, wallMap, 0, -1, message);
        }
        else if (ch == 's')
        {
            moved = handlePlayerMove(player, items, enemies, wallMap, 0, 1, message);
        }
        else if (ch == 'a')
        {
            moved = handlePlayerMove(player, items, enemies, wallMap, -1, 0, message);
        }
        else if (ch == 'd')
        {
            moved = handlePlayerMove(player, items, enemies, wallMap, 1, 0, message);
        }
        else if (ch == 'b')
        {
            buildWall(player, wallMap, message);
        }
        else if (ch == 'h')
        {
            if (player.food >= 1)
            {
                player.food--;
                player.health = min(player.maxHealth, player.health + 12);
                message = "You eat and recover 12 health.";
            }
            else
            {
                message = "You have no food left.";
            }
        }

        if (moved)
        {
            frameCounter++;
            if (frameCounter >= 3)
            {
                startNewDay(player, wallMap, items, enemies);
                frameCounter = 0;
            }
        }

        for (auto& enemy : enemies)
        {
            moveEnemyToward(enemy, player);
            if (enemy.x == player.x && enemy.y == player.y)
            {
                int strike = max(1, enemy.attack - player.defense + rand() % 5);
                player.health -= strike;
                message = "An enemy attacks you!";
            }
        }

        if (player.health <= 0)
        {
            message = "You were defeated. Press r to restart or q to quit.";
            drawMap(player, items, enemies, wallMap, message);
            int key = getch();
            if (key == 'r')
            {
                resetGame(player, items, enemies, wallMap);
                message = "Fresh start. The wilds await.";
            }
            else if (key == 'q')
            {
                running = false;
            }
        }

        if (player.score >= WIN_SCORE)
        {
            message = "You survived and won the world! Press q to quit.";
            drawMap(player, items, enemies, wallMap, message);
            int key = getch();
            if (key == 'q')
            {
                running = false;
            }
        }

        napms(100);
    }

    endwin();
    return 0;
}
