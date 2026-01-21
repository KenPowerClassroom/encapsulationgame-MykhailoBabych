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

// New class: Manages weapon inventory
class WeaponManager {
private:
    std::vector<Weapon> weapons;

public:
    void addWeapon(const Weapon& weapon) {
        weapons.push_back(weapon);
    }

    void equipWeapon(Character& character, int weaponIndex) {
        if (weaponIndex >= 0 && weaponIndex < weapons.size()) {
            character.setWeapon(&weapons[weaponIndex]);
        }
    }

    void equipRandomWeapon(Character& character) {
        if (weapons.empty()) return;
        
        int randomIndex = std::rand() % weapons.size();
        character.setWeapon(&weapons[randomIndex]);
    }

    bool hasWeapons() const {
        return !weapons.empty();
    }
};

// New class: Manages combat between two characters
class BattleManager {
public:
    int runBattle(Character& fighter1, Character& fighter2, Player& player) {
        std::cout << "Game started: " << fighter1.getName() << " vs " << fighter2.getName() << "\n";

        // Check if both have weapons
        if (!fighter1.hasWeapon() || !fighter2.hasWeapon()) {
            std::cout << "Weapon not equipped. Cannot fight.\n";
            return -1;
        }

        // Battle loop
        while (fighter1.isAlive() && fighter2.isAlive()) {
            fighter1.attackTarget(fighter2);
            
            if (!fighter2.isAlive()) break;

            fighter2.attackTarget(fighter1);

            // Apply random healing to player
            applyRandomHealing(player);
        }

        return determineWinner(fighter1, fighter2);
    }

private:
    void applyRandomHealing(Player& player) {
        int healAmount = std::rand() % 50 + 1;
        player.heal(healAmount);
    }

    int determineWinner(Character& fighter1, Character& fighter2) {
        if (!fighter1.isAlive()) {
            std::cout << fighter1.getName() << " has been defeated.\n";
            return 1;
        }
        else if (!fighter2.isAlive()) {
            std::cout << fighter2.getName() << " has been defeated.\n";
            return 0;
        }
        
        return -1;
    }
};

// Simplified GameManager - now only coordinates between managers
class GameManager {
private:
    Player player;
    Enemy enemy;
    WeaponManager weaponManager;
    BattleManager battleManager;

public:
    GameManager(const Player& p, const Enemy& e)
        : player(p), enemy(e) {
        std::srand(std::time(0));
    }

    void addWeapon(const Weapon& weapon) {
        weaponManager.addWeapon(weapon);
    }

    void equipPlayerWeapon(int weaponIndex) {
        weaponManager.equipWeapon(player, weaponIndex);
    }

    void equipEnemyWeapon(int weaponIndex) {
        weaponManager.equipWeapon(enemy, weaponIndex);
    }

    int startGame() {
        return battleManager.runBattle(player, enemy, player);
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
