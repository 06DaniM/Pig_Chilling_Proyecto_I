#ifndef ENEMY_H
#define ENEMY_H

#include <vector>
#include "raylib.h"

class Bullet_Enemy {
public:
    Rectangle rect;
    bool active;

    Bullet_Enemy();
};

enum EnemyClass {
    Draconoida,
    Mantis,
    Squid,
    Kraken
};

class Enemy {
public:
    EnemyClass enemyClass;
    Rectangle rect;
    Rectangle enemyDeathFrameRec;
    bool active;
    bool isAttacking;
    float attackTime;
    float attackingTimer;
    float attackCooldown;
    Vector2 targetPosition1;
    Vector2 targetFinalPosition;
    Vector2 targetIdlePosition;
    float attackPlayerPos;
    float entryTime;
    float rotation;
    int index;
    int loopDirectionX;
    int loopDirectionY;
    float loopTime;
    int currentEnemies;
    bool enemyInitialState;
    bool enemyLoopState;
    bool manual;
    bool idle;
    bool random;
    bool right;
    bool playerOnRight;
    bool canAttack;
    bool gotHit;
    int enemyDeathFramesCounter;
    int currentEnemyDeathFrame;

    // Constructor
    Enemy();

    // Método para actualizar la posición del enemigo
    void UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, float deltaTime, Rectangle& player, bool gameOver);
    void SpawnEnemies(std::vector<Enemy>& enemies, int numberEnemies, float baseHeight, float baseWidth, int directionX, int directionY, float loopTime, float targetX, float targetY, int currentEnemies, int enemyPreset);
    void UpdateEnemyOffset(float deltaTime);
};

#endif // ENEMY_H
