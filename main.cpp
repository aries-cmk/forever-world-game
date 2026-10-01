#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>

using namespace std;

const int MAX_HEALTH = 100;
const int WIN_GOAL = 5;

struct Player
{
    string name;
    int health;
    int maxHealth;
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
    int campLevel;
};

struct Enemy
{
    string name;
    int health;
    int attack;
    int defense;
    int reward;
};

int randomNumber(int lowest, int highest)
{
    return lowest + rand() % (highest - lowest + 1);
}

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printDivider()
{
    cout << "\n========================================\n";
}

void showStats(const Player& player)
{
    printDivider();
    cout << "PLAYER: " << player.name << "\n";
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
    cout << "Camp Level: " << player.campLevel << "\n";
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

    cout << "You gather supplies from the forest and ruins.\n";
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
    player.walls += 1;
    player.defense += 1;

    cout << "A wall goes up around your camp. Defense +1\n";
}

void restAndHeal(Player& player)
{
    int foodCost = 3;

    if (player.food < foodCost)
    {
        cout << "You need " << foodCost << " food to rest and recover.\n";
        return;
    }

    player.food -= foodCost;
    int healAmount = randomNumber(12, 25);
    player.health = min(player.maxHealth, player.health + healAmount);

    cout << "You rest by the fire and recover " << healAmount << " health.\n";
}

void upgradeGear(Player& player)
{
    int woodCost = 10;
    int stoneCost = 8;
    int goldCost = 12;

    if (player.wood < woodCost || player.stone < stoneCost || player.gold < goldCost)
    {
        cout << "You need " << woodCost << " wood, " << stoneCost << " stone, and " << goldCost << " gold.\n";
        return;
    }

    player.wood -= woodCost;
    player.stone -= stoneCost;
    player.gold -= goldCost;
    player.attack += 2;
    player.defense += 1;

    cout << "Your gear is improved. Attack +2, Defense +1\n";
}

void upgradeCamp(Player& player)
{
    int woodCost = 12;
    int stoneCost = 10;
    int goldCost = 15;

    if (player.wood < woodCost || player.stone < stoneCost || player.gold < goldCost)
    {
        cout << "You need " << woodCost << " wood, " << stoneCost << " stone, and " << goldCost << " gold to improve the camp.\n";
        return;
    }

    player.wood -= woodCost;
    player.stone -= stoneCost;
    player.gold -= goldCost;
    player.campLevel += 1;
    player.maxHealth += 10;
    player.health = player.maxHealth;

    cout << "Your camp grows stronger. Max health +10 and health restored.\n";
}

Enemy generateEnemy(int day)
{
    Enemy enemy;
    int tier = randomNumber(1, 4);

    if (tier == 1)
    {
        enemy.name = "Scout";
        enemy.health = 22 + day * 4;
        enemy.attack = 7 + day;
        enemy.defense = 2 + day / 2;
        enemy.reward = 10 + day * 2;
    }
    else if (tier == 2)
    {
        enemy.name = "Raider";
        enemy.health = 30 + day * 5;
        enemy.attack = 9 + day;
        enemy.defense = 3 + day / 2;
        enemy.reward = 14 + day * 2;
    }
    else if (tier == 3)
    {
        enemy.name = "Brute";
        enemy.health = 40 + day * 6;
        enemy.attack = 12 + day;
        enemy.defense = 4 + day / 2;
        enemy.reward = 18 + day * 3;
    }
    else
    {
        enemy.name = "Warden";
        enemy.health = 50 + day * 7;
        enemy.attack = 15 + day;
        enemy.defense = 6 + day;
        enemy.reward = 22 + day * 4;
    }

    return enemy;
}

void showEnemy(const Enemy& enemy)
{
    cout << "Enemy: " << enemy.name << "\n";
    cout << "Health: " << enemy.health << "\n";
    cout << "Attack: " << enemy.attack << "\n";
    cout << "Defense: " << enemy.defense << "\n";
    cout << "Reward: " << enemy.reward << " gold\n";
}

void battle(Player& player)
{
    Enemy enemy = generateEnemy(player.day);
    int turn = 1;
    int blockAmount = 0;

    printDivider();
    cout << "A wild " << enemy.name << " appears!\n";
    showEnemy(enemy);

    while (player.health > 0 && enemy.health > 0)
    {
        int choice;

        printDivider();
        cout << "TURN " << turn << "\n";
        cout << "1 - Attack\n";
        cout << "2 - Guard\n";
        cout << "3 - Eat food\n";
        cout << "4 - Run\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            int damage = randomNumber(player.attack, player.attack + 8) - enemy.defense;
            if (damage < 1)
            {
                damage = 1;
            }

            enemy.health -= damage;
            cout << "You hit for " << damage << " damage.\n";
        }
        else if (choice == 2)
        {
            blockAmount = player.defense + 5;
            cout << "You guard and reduce incoming damage by " << blockAmount << " next turn.\n";
        }
        else if (choice == 3)
        {
            if (player.food <= 0)
            {
                cout << "You have no food left.\n";
                continue;
            }

            player.food--;
            int healAmount = randomNumber(8, 18);
            player.health = min(player.maxHealth, player.health + healAmount);
            cout << "You eat and recover " << healAmount << " health.\n";
        }
        else if (choice == 4)
        {
            cout << "You retreat from the battle and save your strength.\n";
            return;
        }
        else
        {
            cout << "Invalid choice. You hesitate.\n";
            continue;
        }

        if (enemy.health <= 0)
        {
            break;
        }

        int enemyDamage = randomNumber(max(1, enemy.attack - 2), enemy.attack + 4) - (player.defense + blockAmount);
        if (enemyDamage < 1)
        {
            enemyDamage = 1;
        }

        player.health -= enemyDamage;

        if (blockAmount > 0)
        {
            cout << "Your guard reduces the hit.\n";
            blockAmount = 0;
        }

        cout << enemy.name << " hits you for " << enemyDamage << " damage.\n";
        cout << "Your health: " << max(0, player.health) << "/" << player.maxHealth << "\n";
        turn++;
    }

    printDivider();

    if (player.health > 0)
    {
        player.wins++;
        player.gold += enemy.reward;
        player.food += randomNumber(2, 5);
        player.day += 1;

        cout << "Victory! You defeated the " << enemy.name << "!\n";
        cout << "+" << enemy.reward << " gold and +" << randomNumber(2, 5) << " food\n";

        if (player.wins >= WIN_GOAL)
        {
            cout << "\nYou have survived long enough to win the world!\n";
            cout << "Congratulations, " << player.name << "!\n";
            cout << "Press any key to exit...\n";
            clearInput();
            exit(0);
        }
    }
    else
    {
        player.losses++;
        player.day += 1;
        player.health = max(1, player.maxHealth / 2);
        cout << "You were defeated. You crawl back to camp and recover half health.\n";
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    Player player;
    player.name = "";
    player.maxHealth = MAX_HEALTH;
    player.health = MAX_HEALTH;
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
    player.campLevel = 1;

    cout << "========================================\n";
    cout << "FOREVER WORLD: LAST STAND\n";
    cout << "========================================\n";
    cout << "Enter your name: ";
    getline(cin, player.name);

    if (player.name.empty())
    {
        player.name = "Warden";
    }

    bool running = true;

    while (running)
    {
        printDivider();
        cout << "MAIN MENU\n";
        cout << "1 - Explore the wild\n";
        cout << "2 - Gather resources\n";
        cout << "3 - Build wall\n";
        cout << "4 - Eat and recover\n";
        cout << "5 - Upgrade gear\n";
        cout << "6 - Upgrade camp\n";
        cout << "7 - View stats\n";
        cout << "8 - Quit\n";
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
            restAndHeal(player);
        }
        else if (choice == 5)
        {
            upgradeGear(player);
        }
        else if (choice == 6)
        {
            upgradeCamp(player);
        }
        else if (choice == 7)
        {
            showStats(player);
        }
        else if (choice == 8)
        {
            cout << "Thanks for playing, " << player.name << "!\n";
            running = false;
        }
        else
        {
            cout << "Invalid selection. Try again.\n";
        }

        clearInput();
    }

    return 0;
}
