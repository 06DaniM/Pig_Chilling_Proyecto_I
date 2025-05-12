#ifndef KRAKEN_H
#define KRAKEN_H

#include <vector>
#include "raylib.h"
#include "Enemy.h"

class KrakenEnemy : public Enemy {
public:
    // Constructor
    KrakenEnemy();
    
    void KrakenAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN);
};

#endif // KRAKEN_H
