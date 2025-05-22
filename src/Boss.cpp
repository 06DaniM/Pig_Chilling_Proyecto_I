#include "Boss.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Boss::Bullet_Boss() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Boss::Boss()
    : rect{ 0, 0, 430, 256 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, life(200), active(true), dying(false), attackRutine(0), laserAttackNormal(false),
    laserAttackHeavy(false), shooting(false), attackTime(0.0f), attackingTimer(0.0f), attackCooldown(0.0f),  
    playerPos{ 0.0f, 0.0f }, rotation(0.0f), idle(false), random(false), canAttack(true), hasArribed(false), 
    enemyDeathFramesCounter(0), currentEnemyDeathFrame(0), enemyStartX(0) {
}

const int screenWidth = 1152;
const int screenHeight = 896;

void Boss::BossSpawn()
{
    Boss boss;

    // Spawn position
    float startX = screenWidth / 2 + boss.rect.width / 2;
    float startY = screenHeight - 300;

    // Position to move for starting the fight
    float targetX = startX;
    float targetY = screenHeight +300;
}

void Boss::BossManager()
{

}