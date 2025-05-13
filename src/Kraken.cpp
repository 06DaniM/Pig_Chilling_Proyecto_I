#include "Kraken.h"
#include <cmath>
#include <iostream>

using namespace std;

// Enemy constructor
KrakenEnemy::KrakenEnemy() : attackCollider({ 0, 0, 0, 10 }), playerPicked(false) {}
 
const int screenWidth = 1152;
const int screenHeight = 896;

void KrakenEnemy::KrakenAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN)
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

        enemy.attackPlayerPos = player.x;
        enemy.enemyLoopState = true;
    }

    else
    {
        float distX1 = player.x + 10 - enemy.rect.x;

        // Move the enemy to the target
        float distY1 = player.y - enemy.rect.y - 90;
        float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Total distance to the objective

        float distX2 = enemy.targetFinalPosition.x + globalEnemyOffsetXN - enemy.rect.x;
        float distY2 = enemy.targetFinalPosition.y - enemy.rect.y;
        float distance2 = sqrt(distX2 * distX2 + distY2 * distY2); // Total distance to the objective

        if (distance1 >= moveSpeed && enemy.enemyLoopState)
        {
            // Normalize the direction
            float directionX = distX1 / distance1;
            float directionY = distY1 / distance1;

            enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90;

            // Move the enemy to the objective
            enemy.rect.x += directionX * moveSpeed;
            enemy.rect.y += directionY * moveSpeed;

            if (enemy.rect.y >= player.y - 95)
            {
                enemy.enemyLoopState = false;
                enemy.canAttack = true;
            }
        }

        else if (!enemy.enemyLoopState)
        {
            if (enemy.canAttack)
            {
                // === ATTACK LOGIC === //
                attackCollider.x = enemy.rect.x + 20;
                attackCollider.y = player.y;

                float distX = attackCollider.x - player.x;
                float distance = sqrt(distX * distX);

                float distX1 = enemy.targetFinalPosition.x - enemy.rect.x;
                float distY1 = enemy.targetFinalPosition.y - 300 - enemy.rect.y;
                float distance1 = sqrt(distX1 * distX1 + distY1 * distY1);

                float directionX = distX1 / distance1;
                float directionY = distY1 / distance1;

                enemy.rotation = Lerp(enemy.rotation, 0.0f, 0.2f);

                if (fabs(enemy.rotation) < 0.02f)
                    enemy.rotation = 0.0f;

                if (CheckCollisionRecs(attackCollider, player) && !playerPicked)
                {
                    if (player.x != enemy.rect.x)
                    {
                        player.x = attackCollider.x - player.width/2+7.5f;
                    }
                    playerPicked = true;
                }

                if (playerPicked && distance1 > moveSpeed)
                {
                    player.x = attackCollider.x - player.width / 2 + 7.5f;
                    player.y = enemy.rect.y + 90;

                    enemy.rect.x += directionX * moveSpeed;
                    enemy.rect.y += directionY * moveSpeed;
                }

                else if (player.x < screenHeight - 90)
                {
                    enemy.canAttack = false;
                    enemy.rect = player;
                }
            }

            //else if (distance2 > moveSpeed && !enemy.canAttack)
            //{
            //    // Normalize the direction
            //    float directionX = distX2 / distance2;
            //    float directionY = distY2 / distance2;

            //    enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90; // Convertir a grados

            //    // Move the enemy to the objective
            //    enemy.rect.x += directionX * moveSpeed;
            //    enemy.rect.y += directionY * moveSpeed;
            //}

            else
            {
                // Ends attacking state

                enemy.active = false;

                // Restart random state
            }
        }
    }
}