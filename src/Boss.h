#ifndef BOSS_H
#define BOSS_H
#include <vector>
#include "raylib.h"

class Bullet_Boss {
public:
    Rectangle rect;
    bool active;

    Bullet_Boss();
};

class Boss {
public:
    Rectangle rect;
    Rectangle enemyDeathFrameRec;
    int life;
    bool active;
    bool dying;
    int attackRutine;
    bool laserAttackNormal;
    bool laserAttackHeavy;
    bool shooting;
    float attackTime;
    float attackingTimer;
    float attackCooldown;
    Vector2 playerPos;
    float rotation;
    bool idle;
    bool random;
    bool canAttack;
    bool hasArribed;
    int enemyDeathFramesCounter;
    int currentEnemyDeathFrame;
    float enemyStartX;

    // Constructor
    Boss();

    void BossSpawn();
    void BossManager();
};

#endif // BOSS_H
