#include "Draconoida.h"
#include <cmath>
#include <iostream>

using namespace std;

// Enemy constructor
DraconoidaEnemy::DraconoidaEnemy() {}

const int screenWidth = 1152;
const int screenHeight = 896;

void DraconoidaEnemy::DraconoidaAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN)
{
    float velocity2 = 300.0f; // Movement speed
    float moveSpeed = velocity2 * deltaTime;
    enemy.attackingTimer += deltaTime;

    float t = enemy.attackingTimer;
    float loopT = t * 0.5f;

    // Mini loop
    if (enemy.attackingTimer <= 1.2f)
    {
        float dx = cos(loopT * PI * 2.5f) * 5;
        float dy = -sin(loopT * PI * 2.5f) * 2.5f;

        // Movimiento
        enemy.rect.x += dx;
        enemy.rect.y += dy;

        // Rotación
        enemy.rotation = atan2(dy, dx) * (180.0f / PI) + 90;

        if (enemy.rect.x < player.x)
        {
            enemy.playerOnRight = false;
        }
        else enemy.playerOnRight = true;

        enemy.attackPlayerPos = player.x;
        enemy.enemyLoopState = true;
    }

    else
    {
        float distX1;

        // Move the enemy to the target

        if (enemy.playerOnRight) distX1 = enemy.targetFinalPosition.x - 200 - enemy.rect.x;
        else distX1 = enemy.targetFinalPosition.x + 200 - enemy.rect.x;

        float distY1 = player.y - 100 - enemy.rect.y;
        float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Total distance to the objective

        float distX2 = enemy.targetFinalPosition.x + globalEnemyOffsetXN - enemy.rect.x;
        float distY2 = enemy.targetFinalPosition.y - enemy.rect.y;
        float distance2 = sqrt(distX2 * distX2 + distY2 * distY2); // Total distance to the objective

        if (enemy.attackingTimer >= 2.5f && enemy.canAttack)
        {
            // === Enemy shooter manager ===

            Bullet_Enemy newBullets_Enemy;

            newBullets_Enemy.rect = { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 16, 12 };
            newBullets_Enemy.active = true;

            enemyBullets.push_back({ newBullets_Enemy });

            enemy.canAttack = false;
        }

        if (distance1 >= moveSpeed && enemy.enemyLoopState)
        {
            // Normalize the direction
            float directionX = distX1 / distance1;
            float directionY = distY1 / distance1;

            enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90;

            // Move the enemy to the objective
            enemy.rect.x += directionX * moveSpeed;
            enemy.rect.y += directionY * moveSpeed;

            if (enemy.rect.y >= player.y - 150)
            {
                enemy.enemyLoopState = false;
            }
        }

        else if (!enemy.enemyLoopState)
        {
            if (distance2 > moveSpeed)
            {
                // Normalize the direction
                float directionX = distX2 / distance2;
                float directionY = distY2 / distance2;

                enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90; // Convertir a grados

                // Move the enemy to the objective
                enemy.rect.x += directionX * moveSpeed;
                enemy.rect.y += directionY * moveSpeed;
            }

            else
            {
                // Ends attacking state

                enemy.random = false;
                enemy.attackingTimer = 0.0f;

                // Restart random state
            }
        }
    }
}