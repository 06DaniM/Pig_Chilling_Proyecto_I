#ifndef DRACONOIDA_H
#define DRACONOIDA_H

#include <vector>
#include "raylib.h"
#include "Enemy.h"

class DraconoidaEnemy : public Enemy {
public:
    // Constructor
    DraconoidaEnemy();
    
    void DraconoidaAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN);
};

#endif // DRACONOIDA_H