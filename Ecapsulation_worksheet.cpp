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

class CombatLogger {
public:
    void logAttack(const std::string& attackerName, const std::string& targetName, 
                   const std::string& weaponName) const {
        std::cout << attackerName << " attacks " << targetName 
                  << " with " << weaponName << "\n";
    }

    void logDamage(const std::string& characterName, int damage, int remainingHealth) const {
        std::cout << characterName << " takes damage " << damage 
                  << ". Health: " << remainingHealth << "\n";
    }

    void logHealing(const std::string& characterName, int healAmount, int currentHealth) const {
        std::cout << characterName << " healed by " << healAmount 
                  << " points. Health: " << currentHealth << "\n";
    }

    void logDefeat(const std::string& characterName) const {
        std::cout << characterName << " has been defeated.\n";
    }

    void logBattleStart(const std::string& fighter1Name, const std::string& fighter2Name) const {
        std::cout << "Game started: " << fighter1Name << " vs " << fighter2Name << "\n";
    }

    void logError(const std::string& message) const {
        std::cout << message << "\n";
    }
};

class HealthManager {
private:
    int health;
    CombatLogger& logger;
    std::string ownerName;

public:
    HealthManager(int initialHealth, CombatLogger& combatLogger, const std::string& name)
        : health(initialHealth), logger(combatLogger), ownerName(name) {}

    bool isAlive() const {
        return health > 0;
    }

    int getCurrentHealth() const {
        return health;
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
        logger.logDamage(ownerName, damage, health);
    }

    void heal(int amount) {
        if (!isAlive()) return;
        health += amount;
        logger.logHealing(ownerName, amount, health);
    }
};

// Character now focuses only on combat-related behavior
class Character {
protected:
    std::string name;
    HealthManager healthManager;
    int strength; // multiplier for weapon damage
    Weapon* currentWeapon;
    CombatLogger& logger;

    int calculateDamage() const {
        if (!currentWeapon) return 0;
        return currentWeapon->getDamage() * strength;
    }

public:
    Character(const std::string& characterName, int characterHealth, 
              int characterStrength, CombatLogger& combatLogger)
        : name(characterName), 
          healthManager(characterHealth, combatLogger, characterName),
          strength(characterStrength), 
          currentWeapon(nullptr),
          logger(combatLogger) {}

    void setWeapon(Weapon* weapon) {
        currentWeapon = weapon;
    }

    std::string getName() const { return name; }

    bool isAlive() const {
        return healthManager.isAlive();
    }

    bool hasWeapon() const {
        return currentWeapon != nullptr;
    }

    bool performAttack(Character& target) {
        if (!currentWeapon) return false;
        
        int totalDamage = calculateDamage();
        logger.logAttack(name, target.getName(), currentWeapon->getName());
        target.takeDamage(totalDamage);
        return true;
    }

    void takeDamage(int damage) {
        healthManager.takeDamage(damage);
    }

    void applyHealing(int amount) {
        healthManager.heal(amount);
    }

    void announceDefeat() const {
        logger.logDefeat(name);
    }
};

class Player : public Character {
public:
    Player(const std::string& playerName, int playerHealth, 
           int characterStrength, CombatLogger& logger)
        : Character(playerName, playerHealth, characterStrength, logger) {}

    void applyRandomHealing() {
        int healAmount = std::rand() % 50 + 1;
        applyHealing(healAmount);
    }
};

class Enemy : public Character {
public:
    Enemy(const std::string& enemyName, int enemyHealth, 
          int characterStrength, CombatLogger& logger)
        : Character(enemyName, enemyHealth, characterStrength, logger) {}
};

// Weapon inventory management - single responsibility
class WeaponManager {
private:
    std::vector<Weapon> weapons;

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

    bool equipWeaponToCharacter(Character& character, int weaponIndex) {
        if (!isValidWeaponIndex(weaponIndex)) return false;
        
        character.setWeapon(&weapons[weaponIndex]);
        return true;
    }

    bool equipRandomWeaponToCharacter(Character& character) {
        if (isEmpty()) return false;
        
        int randomIndex = std::rand() % weapons.size();
        return equipWeaponToCharacter(character, randomIndex);
    }
};

class BattleValidator {
private:
    CombatLogger& logger;

public:
    BattleValidator(CombatLogger& combatLogger) : logger(combatLogger) {}

    bool validateBattleReadiness(const Character& fighter1, const Character& fighter2) const {
        if (!fighter1.hasWeapon() || !fighter2.hasWeapon()) {
            logger.logError("Weapon not equipped. Cannot fight.");
            return false;
        }
        return true;
    }
};

// Combat execution - single responsibility
class BattleManager {
private:
    CombatLogger& logger;
    BattleValidator validator;

    bool executeCombatRound(Character& attacker, Character& defender) {
        attacker.performAttack(defender);
        return defender.isAlive();
    }

    void conductBattle(Character& fighter1, Character& fighter2, Player& playerRef) {
        while (fighter1.isAlive() && fighter2.isAlive()) {
            if (!executeCombatRound(fighter1, fighter2)) break;
            if (!executeCombatRound(fighter2, fighter1)) break;

            playerRef.applyRandomHealing();
        }
    }

    int determineBattleOutcome(Character& fighter1, Character& fighter2) const {
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

public:
    BattleManager(CombatLogger& combatLogger) 
        : logger(combatLogger), validator(combatLogger) {}

    int executeBattle(Character& fighter1, Character& fighter2, Player& playerRef) {
        logger.logBattleStart(fighter1.getName(), fighter2.getName());

        if (!validator.validateBattleReadiness(fighter1, fighter2)) {
            return -1;
        }

        conductBattle(fighter1, fighter2, playerRef);

        return determineBattleOutcome(fighter1, fighter2);
    }
};

// Game coordination - single responsibility: orchestrate game components
class GameManager {
private:
    CombatLogger logger;
    Player player;
    Enemy enemy;
    WeaponManager weaponManager;
    BattleManager battleManager;

    void equipCharacter(Character& character, int weaponIndex) {
        weaponManager.equipWeaponToCharacter(character, weaponIndex);
    }

public:
    GameManager(const std::string& playerName, int playerHealth, int playerStrength,
                const std::string& enemyName, int enemyHealth, int enemyStrength)
        : player(playerName, playerHealth, playerStrength, logger),
          enemy(enemyName, enemyHealth, enemyStrength, logger),
          battleManager(logger) {
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
    GameManager game("Hero", 300, 2, "Goblin", 150, 4);

    Weapon sword("Sword", 15);
    Weapon axe("Axe", 20);
    Weapon dagger("Dagger", 10);
    Weapon bow("Bow", 25);

    game.addWeapon(sword);
    game.addWeapon(axe);
    game.addWeapon(dagger);
    game.addWeapon(bow);

    game.equipPlayerWeapon(0); // Equip sword to player
    game.equipEnemyWeapon(1);  // Equip axe to enemy

    game.startGame();

    return 0;
}
