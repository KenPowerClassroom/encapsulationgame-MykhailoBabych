#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

class Weapon {
private:
    std::string name;
    int damage;

public:
    Weapon(const std::string& weaponName, int weaponDamage)
        : name(weaponName), damage(weaponDamage) {}

    std::string getName() const { return name; }
    int getDamage() const { return damage; }

    void setDamage(int newDamage) { damage = newDamage; }
};

class Character {
protected:
    std::string name;
    int health;
    int strength; // multiplier for weapon damage
    Weapon* currentWeapon;

public:
    Character(const std::string& characterName, int characterHealth, int characterStrength)
        : name(characterName), health(characterHealth), 
          strength(characterStrength), currentWeapon(nullptr) {}

    void setWeapon(Weapon* weapon) {
        currentWeapon = weapon;
    }

    std::string getName() const { return name; }
    
    int getHealth() const { return health; }

    bool isAlive() const {
        return health > 0;
    }

    bool hasWeapon() const {
        return currentWeapon != nullptr;
    }

    // Character knows how to attack another character
    void attackTarget(Character& target) {
        if (!currentWeapon) return;
        
        int totalDamage = currentWeapon->getDamage() * strength;
        std::cout << name << " attacks " << target.getName() 
                  << " with " << currentWeapon->getName() << "\n";
        target.takeDamage(totalDamage);
        std::cout << target.getName() << " health: " << target.getHealth() << "\n";
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
        std::cout << name << " take damage " << damage << "\n";
    }

    // Character knows how to heal itself
    void heal(int amount) {
        if (health > 0) {
            health += amount;
            std::cout << name << " healed by " << amount << " points.\n";
        }
    }
};

class Player : public Character {
public:
    Player(const std::string& playerName, int playerHealth, int characterStrength)
        :Character(playerName, playerHealth, characterStrength) {}
};

class Enemy : public Character {
public:
    Enemy(const std::string& EnemyName, int EnemyHealth, int characterStrength)
        :Character(EnemyName, EnemyHealth, characterStrength) {}

};

class GameManager {
private:
    Player player;
    Enemy enemy;
    std::vector<Weapon> weapons;

public:
    GameManager(const Player& p, const Enemy& e)
        : player(p), enemy(e) {
        std::srand(std::time(0)); // Seed for random number generation
    }

    void addWeapon(const Weapon& weapon) {
        weapons.push_back(weapon);
    }

    int startGame() {
        std::cout << "Game started: " << player.getName() << " vs " << enemy.getName() << "\n";

        // Check if both have weapons
        if (!player.hasWeapon() || !enemy.hasWeapon()) {
            std::cout << "Weapon not equipped. Cannot fight.\n";
            return -1;
        }

        // Player and enemy health checks
        while (player.isAlive() && enemy.isAlive()) {
            // Player attacks enemy - character handles its own attack
            player.attackTarget(enemy);
            
            if (!enemy.isAlive()) break;

            // Enemy attacks player - character handles its own attack
            enemy.attackTarget(player);

            randomlyHealPlayer();
        }

        if (!player.isAlive()) {
            std::cout << player.getName() << " has been defeated.\n";
            return 1;
        }
        else if (!enemy.isAlive()) {
            std::cout << enemy.getName() << " has been defeated.\n";
            return 0;
        }
        
        return -1;
    }

    void equipPlayerWeapon(int weaponIndex) {
        if (weaponIndex >= 0 && weaponIndex < weapons.size()) {
            player.setWeapon(&weapons[weaponIndex]);
        }
    }

    void equipEnemyWeapon(int weaponIndex) {
        if (weaponIndex >= 0 && weaponIndex < weapons.size()) {
            enemy.setWeapon(&weapons[weaponIndex]);
        }
    }

    Weapon* equipRandomWeapon(Character& character) {
        if (weapons.empty()) {
            return nullptr;
        }
        int randomIndex = std::rand() % weapons.size();
        Weapon* selectedWeapon = &weapons[randomIndex];
        character.setWeapon(selectedWeapon);
        return selectedWeapon;
    }

    void randomlyHealPlayer() {
        int healAmount = std::rand() % 50 + 1; // heal between 1 and 50 point
        player.heal(healAmount);
    }
};

// Main Function
int main() {

    Player player("Hero", 300, 2);
    Enemy enemy("Goblin", 150, 4);

    Weapon sword("Sword", 15);
    Weapon axe("Axe", 20);
    Weapon dagger("Dagger", 10);
    Weapon bow("Bow", 25);

    GameManager game(player, enemy);

    game.addWeapon(sword);
    game.addWeapon(axe);
    game.addWeapon(dagger);
    game.addWeapon(bow);

    // Equip weapons
    game.equipPlayerWeapon(0); // Equip sword to player
    game.equipEnemyWeapon(1);  // Equip axe to enemy


    game.startGame();

    return 0;
}
