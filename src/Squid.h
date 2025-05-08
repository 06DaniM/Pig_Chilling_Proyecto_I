#ifndef SQUID_H
#define SQUID_H

#include <vector>
#include "raylib.h"
#include "Enemy.h"

class SquidEnemy : public Enemy {
public:
    // Constructor
    SquidEnemy();
    
    void SquidAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetX, float baseWidth, bool gameOver);
};

#endif // SQUID_H
