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

class Enemy {
public:
    Rectangle rect;
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
    int index;
    int loopDirection;
    int currentEnemies;
    bool enemyInitialState;
    bool enemyLoopState;
    bool manual;
    bool idle;
    bool random;
    bool right;
    bool playerOnRight;
    bool canAttack;

    // Constructor
    Enemy();

    // Método para actualizar la posición del enemigo
    void UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, Enemy& enemy, float deltaTime, Rectangle& player);
    void SpawnEnemies(std::vector<Enemy>& enemies, int numberEnemies, int currentEnemies,float baseHeight, float baseWidth, int direction, float targetX, float targetY);
};

#endif // ENEMY_H
