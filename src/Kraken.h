#ifndef KRAKEN_H
#define KRAKEN_H

#include <vector>
#include "raylib.h"
#include "Enemy.h"

class KrakenEnemy : public Enemy {
public:
    // Constructor
    KrakenEnemy();
    
    void KrakenAttackManager(Enemy& enemy);
};

#endif // KRAKEN_H
