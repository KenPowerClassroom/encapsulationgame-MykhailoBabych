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

    // Helper to calculate damage (DRY - used by performAttack)
    int calculateDamage() const {
        if (!currentWeapon) return 0;
        return currentWeapon->getDamage() * strength;
    }

    // Helper to clamp health (DRY - used by receiveDamage and applyHealing)
    void adjustHealth(int amount) {
        health += amount;
        if (health < 0) health = 0;
    }

public:
    Character(const std::string& characterName, int characterHealth, int characterStrength)
        : name(characterName), health(characterHealth), 
          strength(characterStrength), currentWeapon(nullptr) {}

    void setWeapon(Weapon* weapon) {
        currentWeapon = weapon;
    }

    std::string getName() const { return name; }

    bool isAlive() const {
        return health > 0;
    }

    bool hasWeapon() const {
        return currentWeapon != nullptr;
    }

    // Tell the character to attack - it handles everything internally
    bool performAttack(Character& target) {
        if (!currentWeapon) return false;
        
        int totalDamage = calculateDamage();
        std::cout << name << " attacks " << target.getName() 
                  << " with " << currentWeapon->getName() << "\n";
        target.receiveDamage(totalDamage);
        return true;
    }

    // Tell the character to receive damage - it handles the logic
    void receiveDamage(int damage) {
        adjustHealth(-damage);
        std::cout << name << " takes damage " << damage << ". Health: " << health << "\n";
    }

    // Tell the character to heal - it decides if healing is valid
    void applyHealing(int amount) {
        if (!isAlive()) return;
        adjustHealth(amount);
        std::cout << name << " healed by " << amount << " points. Health: " << health << "\n";
    }

    // Tell the character to announce defeat
    void announceDefeat() const {
        std::cout << name << " has been defeated.\n";
    }
};

class Player : public Character {
public:
    Player(const std::string& playerName, int playerHealth, int characterStrength)
        :Character(playerName, playerHealth, characterStrength) {}

    // Player knows how to apply random healing to itself
    void applyRandomHealing() {
        int healAmount = std::rand() % 50 + 1;
        applyHealing(healAmount);
    }
};

class Enemy : public Character {
public:
    Enemy(const std::string& EnemyName, int EnemyHealth, int characterStrength)
        :Character(EnemyName, EnemyHealth, characterStrength) {}
};

// Manages weapon inventory
class WeaponManager {
private:
    std::vector<Weapon> weapons;

    // Helper to validate weapon index (DRY)
    bool isValidWeaponIndex(int weaponIndex) const {
        return weaponIndex >= 0 && weaponIndex < weapons.size();
    }

public:
    void addWeapon(const Weapon& weapon) {
        weapons.push_back(weapon);
    }

    bool isEmpty() const {
        return weapons.empty();
    }

    // Tell the manager to equip a weapon - it handles validation
    bool equipWeaponToCharacter(Character& character, int weaponIndex) {
        if (!isValidWeaponIndex(weaponIndex)) return false;
        
        character.setWeapon(&weapons[weaponIndex]);
        return true;
    }

    // Tell the manager to equip a random weapon
    bool equipRandomWeaponToCharacter(Character& character) {
        if (isEmpty()) return false;
        
        int randomIndex = std::rand() % weapons.size();
        return equipWeaponToCharacter(character, randomIndex);
    }
};

// Manages combat between two characters
class BattleManager {
private:
    // Helper to execute a combat round (DRY - avoids repeating attack logic)
    bool executeCombatRound(Character& attacker, Character& defender) {
        attacker.performAttack(defender);
        return defender.isAlive();
    }

public:
    // Tell the battle manager to run a battle - it orchestrates the flow
    int executeBattle(Character& fighter1, Character& fighter2, Player& playerRef) {
        announceBattleStart(fighter1, fighter2);

        if (!validateBattleReadiness(fighter1, fighter2)) {
            return -1;
        }

        conductBattle(fighter1, fighter2, playerRef);

        return announceBattleResult(fighter1, fighter2);
    }

private:
    void announceBattleStart(const Character& fighter1, const Character& fighter2) const {
        std::cout << "Game started: " << fighter1.getName() << " vs " << fighter2.getName() << "\n";
    }

    // Ask minimal question to validate - but don't extract and decide externally
    bool validateBattleReadiness(const Character& fighter1, const Character& fighter2) const {
        if (!fighter1.hasWeapon() || !fighter2.hasWeapon()) {
            std::cout << "Weapon not equipped. Cannot fight.\n";
            return false;
        }
        return true;
    }

    void conductBattle(Character& fighter1, Character& fighter2, Player& playerRef) {
        while (fighter1.isAlive() && fighter2.isAlive()) {
            // Execute fighter1's attack
            if (!executeCombatRound(fighter1, fighter2)) break;

            // Execute fighter2's attack
            if (!executeCombatRound(fighter2, fighter1)) break;

            // Tell player to heal itself
            playerRef.applyRandomHealing();
        }
    }

    int announceBattleResult(Character& fighter1, Character& fighter2) const {
        if (!fighter1.isAlive()) {
            fighter1.announceDefeat();
            return 1;
        }
        else if (!fighter2.isAlive()) {
            fighter2.announceDefeat();
            return 0;
        }
        
        return -1;
    }
};

// Coordinates between managers
class GameManager {
private:
    Player player;
    Enemy enemy;
    WeaponManager weaponManager;
    BattleManager battleManager;

    // Helper method to equip any character (DRY - eliminates equipPlayerWeapon/equipEnemyWeapon duplication)
    void equipCharacter(Character& character, int weaponIndex) {
        weaponManager.equipWeaponToCharacter(character, weaponIndex);
    }

public:
    GameManager(const Player& p, const Enemy& e)
        : player(p), enemy(e) {
        std::srand(std::time(0));
    }

    void addWeapon(const Weapon& weapon) {
        weaponManager.addWeapon(weapon);
    }

    void equipPlayerWeapon(int weaponIndex) {
        equipCharacter(player, weaponIndex);
    }

    void equipEnemyWeapon(int weaponIndex) {
        equipCharacter(enemy, weaponIndex);
    }

    int startGame() {
        return battleManager.executeBattle(player, enemy, player);
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
