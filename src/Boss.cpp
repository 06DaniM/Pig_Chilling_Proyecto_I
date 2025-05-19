#include "Boss.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Boss::Bullet_Boss() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Boss::Boss()
    : rect{ 0, 0, 64, 64 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, active(true), attackRutine(0), laserAttackNormal(false),
    laserAttackHeavy(false), shooting(false), attackTime(0.0f), attackingTimer(0.0f), attackCooldown(0.0f),  
    playerPos{ 0.0f, 0.0f }, rotation(0.0f), idle(false), random(false), canAttack(true), hasArribed(false), 
    enemyDeathFramesCounter(0), currentEnemyDeathFrame(0), enemyStartX(0) {
}

void Boss::BossManager()
{

}