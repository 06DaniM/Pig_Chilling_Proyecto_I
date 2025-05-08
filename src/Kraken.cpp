#include "Kraken.h"
#include <cmath>
#include <iostream>

using namespace std;

// Enemy constructor
KrakenEnemy::KrakenEnemy() {}

const int screenWidth = 1152;
const int screenHeight = 896;

void KrakenEnemy::KrakenAttackManager(Enemy& enemy)
{
    enemy.rect.x = screenWidth / 2;
    enemy.rect.y = screenHeight / 2;
}