#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>
#include <vector>

using namespace std;

struct Player
{
    string name;
    int maxHealth;
    int health;
    int attack;
    int defense;
    int wood;
    int stone;
    int food;
    int gold;
    int walls;
    int wins;
    int losses;
    int day;
};

struct Enemy
{
    string name;
    int health;
    int attack;
    int defense;
    int goldReward;
};

int randomNumber(int lowest, int highest)
{
    return lowest + rand() % (highest - lowest + 1);
}

void printDivider()
{
    cout << "\n========================================\n";
}

void showStats(const Player& player)
{
    printDivider();
    cout << "WORLD STATUS: " << player.name << "\n";
    cout << "Day: " << player.day << "\n";
    cout << "Health: " << player.health << "/" << player.maxHealth << "\n";
    cout << "Attack: " << player.attack << "\n";
    cout << "Defense: " << player.defense << "\n";
    cout << "Wood: " << player.wood << "\n";
    cout << "Stone: " << player.stone << "\n";
    cout << "Food: " << player.food << "\n";
    cout << "Gold: " << player.gold << "\n";
    cout << "Walls: " << player.walls << "\n";
    cout << "Wins: " << player.wins << "\n";
    cout << "Losses: " << player.losses << "\n";
}

void gatherResources(Player& player)
{
    int woodGain = randomNumber(6, 15);
    int stoneGain = randomNumber(2, 7);
    int foodGain = randomNumber(4, 10);
    int goldGain = randomNumber(0, 3);

    player.wood += woodGain;
    player.stone += stoneGain;
    player.food += foodGain;
    player.gold += goldGain;

    cout << "You scavenge the forest and ruins.\n";
    cout << "+" << woodGain << " wood, +" << stoneGain << " stone, +" << foodGain << " food, +" << goldGain << " gold\n";
}

void buildWall(Player& player)
{
    int woodCost = 8;
    int stoneCost = 5;

    if (player.wood < woodCost || player.stone < stoneCost)
    {
        cout << "You need " << woodCost << " wood and " << stoneCost << " stone to build a wall.\n";
        return;
    }

    player.wood -= woodCost;
    player.stone -= stoneCost;
    player.walls++;
    player.defense += 1;

    cout << "A sturdy wall rises around your camp.\n";
    cout << "Defense increased.\n";
}

void heal(Player& player)
{
    int foodCost = 4;

    if (player.food < foodCost)
    {
        cout << "You need " << foodCost << " food to cook a healing meal.\n";
        return;
    }

    player.food -= foodCost;
    int healAmount = randomNumber(12, 25);
    player.health = min(player.maxHealth, player.health + healAmount);

    cout << "You cook and rest. " << player.name << " recovers " << healAmount << " health.\n";
}

void upgradeGear(Player& player)
{
    int woodCost = 10;
    int stoneCost = 8;
    int goldCost = 12;

    if (player.wood < woodCost || player.stone < stoneCost || player.gold < goldCost)
    {
        cout << "You need " << woodCost << " wood, " << stoneCost << " stone, and " << goldCost << " gold to upgrade gear.\n";
        return;
    }

    player.wood -= woodCost;
    player.stone -= stoneCost;
    player.gold -= goldCost;
    player.attack += 2;
    player.defense += 1;

    cout << "Your gear is sharper. Attack +2, Defense +1.\n";
}

Enemy generateEnemy(int day)
{
    Enemy enemy;

    int tier = randomNumber(1, 3);

    if (tier == 1)
    {
        enemy.name = "Raider";
        enemy.health = 30 + day * 3;
        enemy.attack = 8 + day;
        enemy.defense = 2 + day / 2;
        enemy.goldReward = 12 + day * 2;
    }
    else if (tier == 2)
    {
        enemy.name = "Brute";
        enemy.health = 42 + day * 4;
        enemy.attack = 11 + day;
        enemy.defense = 4 + day / 2;
        enemy.goldReward = 16 + day * 2;
    }
    else
    {
        enemy.name = "Warden";
        enemy.health = 35 + day * 5;
        enemy.attack = 10 + day * 2;
        enemy.defense = 5 + day;
        enemy.goldReward = 18 + day * 3;
    }

    return enemy;
}

void showEnemy(const Enemy& enemy)
{
    cout << "Enemy: " << enemy.name << "\n";
    cout << "Health: " << enemy.health << "\n";
    cout << "Attack: " << enemy.attack << "\n";
    cout << "Defense: " << enemy.defense << "\n";
}

void battle(Player& player)
{
    Enemy enemy = generateEnemy(player.day);
    int turn = 1;
    int shield = 0;

    printDivider();
    cout << "A wild " << enemy.name << " appears!\n";
    showEnemy(enemy);

    while (player.health > 0 && enemy.health > 0)
    {
        int choice;

        printDivider();
        cout << "Turn " << turn << "\n";
        cout << "1 - Attack\n";
        cout << "2 - Defend\n";
        cout << "3 - Use food\n";
        cout << "4 - Flee\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            int damage = randomNumber(player.attack, player.attack + 8) - enemy.defense;
            if (damage < 1) damage = 1;
            enemy.health -= damage;
            cout << "You strike for " << damage << " damage.\n";
        }
        else if (choice == 2)
        {
            shield = 5 + player.defense / 2;
            cout << "You brace yourself and gain " << shield << " armor for the enemy's next attack.\n";
        }
        else if (choice == 3)
        {
            if (player.food <= 0)
            {
                cout << "You have no food left.\n";
                continue;
            }

            player.food--;
            int healAmount = randomNumber(8, 16);
            player.health = min(player.maxHealth, player.health + healAmount);
            cout << "You eat a ration and recover " << healAmount << " health.\n";
        }
        else if (choice == 4)
        {
            cout << "You retreat before the battle is won.\n";
            return;
        }
        else
        {
            cout << "Invalid choice. You hesitate.\n";
        }

        if (enemy.health <= 0)
        {
            break;
        }

        int enemyDamage = randomNumber(enemy.attack - 2, enemy.attack + 4) - player.defense - shield;
        if (enemyDamage < 1) enemyDamage = 1;
        player.health -= enemyDamage;

        if (shield > 0)
        {
            cout << "Your defense reduces the impact.\n";
            shield = 0;
        }

        cout << enemy.name << " hits for " << enemyDamage << " damage.\n";
        cout << "Your health: " << max(0, player.health) << "/" << player.maxHealth << "\n";
        turn++;
    }

    printDivider();

    if (player.health > 0)
    {
        player.wins++;
        player.gold += enemy.goldReward;
        player.food += randomNumber(2, 5);
        player.day += 1;
        cout << "Victory! You defeated the " << enemy.name << ".\n";
        cout << "Rewards: +" << enemy.goldReward << " gold, +" << randomNumber(2, 5) << " food\n";
    }
    else
    {
        player.losses++;
        player.day += 1;
        cout << "You were defeated. The world does not forget.\n";
        player.health = max(1, player.maxHealth / 2);
        cout << "You recover at camp and return with half health.\n";
    }
}

void startGame()
{
    Player player;
    player.name = "";
    player.maxHealth = 100;
    player.health = 100;
    player.attack = 12;
    player.defense = 6;
    player.wood = 20;
    player.stone = 12;
    player.food = 8;
    player.gold = 10;
    player.walls = 0;
    player.wins = 0;
    player.losses = 0;
    player.day = 1;

    cout << "Welcome to Forever World\n";
    cout << "Enter your name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, player.name);

    if (player.name.empty())
    {
        player.name = "Warden";
    }

    bool playing = true;

    while (playing)
    {
        printDivider();
        cout << "FOREVER WORLD MENU\n";
        cout << "1 - Explore the wild\n";
        cout << "2 - Gather resources\n";
        cout << "3 - Build a wall\n";
        cout << "4 - Upgrade gear\n";
        cout << "5 - Rest and heal\n";
        cout << "6 - View stats\n";
        cout << "7 - Quit\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            battle(player);
        }
        else if (choice == 2)
        {
            gatherResources(player);
        }
        else if (choice == 3)
        {
            buildWall(player);
        }
        else if (choice == 4)
        {
            upgradeGear(player);
        }
        else if (choice == 5)
        {
            heal(player);
        }
        else if (choice == 6)
        {
            showStats(player);
        }
        else if (choice == 7)
        {
            cout << "Thanks for playing, " << player.name << "!\n";
            playing = false;
        }
        else
        {
            cout << "Invalid selection. Try again.\n";
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    startGame();
    return 0;
}
