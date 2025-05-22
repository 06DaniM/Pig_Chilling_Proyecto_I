#ifndef MANTIS_H
#define MANTIS_H

#include <vector>
#include "raylib.h"
#include "Enemy.h"

class MantisEnemy : public Enemy {
public:
    // Constructor
    MantisEnemy();
    
    void MantisAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN, int attackdirnum);
};

#endif // MANTIS_H