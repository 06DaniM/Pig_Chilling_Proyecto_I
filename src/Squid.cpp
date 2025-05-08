#include "Squid.h"
#include <cmath>
#include <iostream>

using namespace std;

// Enemy constructor
SquidEnemy::SquidEnemy() {}

const int screenWidth = 1152;
const int screenHeight = 896;

void SquidEnemy::SquidAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetX, float baseWidth, bool gameOver)
{
    enemy.entryTime += deltaTime; // Timer when the enemy has started moving

    // === NEW ENEMY MOVEMENT ===

    float delayTime = 0.0f; // Delay for the enemy.entrytime

    // Enemy start de movement with a delay 
    if (enemy.entryTime >= delayTime)
    {
        // === 1ST OBJECTIVE ===
        if (enemy.enemyInitialState)
        {
            // Incrementar 't' para el movimiento circular
            float velocity = 1500.0f; // Velocidad de movimiento
            float moveSpeed = velocity * deltaTime;

            // Calcular la distancia entre la posición actual y el objetivo
            float distX = 0;

            if (baseWidth < screenWidth)
            {
                distX = 1300 - enemy.rect.x;
            }

            else
            {
                distX = -148 - enemy.rect.x;
            }

            float distance = sqrt(distX * distX); // Distancia total al objetivo

            enemy.enemyLoopState = false;
            if (distance > moveSpeed)
            {
                // Normalizar la dirección
                float directionX = distX / distance;

                enemy.rotation = -90; // Convertir a grados

                // Mover al enemigo hacia el objetivo
                enemy.rect.x += directionX * moveSpeed;
            }

            else
            {
                // End of initial state
                enemy.rect.y = 60;

                enemy.enemyInitialState = false;
                enemy.manual = true;

                // Starting loop
            }
        }

        else if (enemy.manual)
        {
            // Incrementar 't' para el movimiento circular
            float velocity = 1000.0f; // Velocidad de movimiento
            float moveSpeed = velocity * deltaTime;

            float distX = enemy.targetFinalPosition.x + globalEnemyOffsetX - enemy.rect.x; // X Distance to the final position of patrol movement
            float distY = 60 - enemy.rect.y; // Y Distance to the final position of patrol movement

            float distance = sqrt(distX * distX + distY * distY); // Total distance to the objective

            if (distance > moveSpeed)
            {
                // Normalize the direction
                float directionX = distX / distance;
                float directionY = distY / distance;

                enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90; // Convertir a grados

                // Move the enemy to the objective
                enemy.rect.x += directionX * moveSpeed;
                enemy.rect.y += directionY * moveSpeed;
            }

            else
            {
                // Finishin patrol state
                enemy.rect.y = 60;

                enemy.manual = false;
                enemy.idle = true;

                //Starting idle movement
            }
        }

        else if (enemy.idle = true)
        {
            enemy.rotation = Lerp(enemy.rotation, 0.0f, 0.2f);

            if (fabs(enemy.rotation) < 0.02f)
                enemy.rotation = 0.0f;

            // Actualizar la posición de todos los enemigos a la vez
            for (Enemy& enemy : enemies)
            {
                if (!enemy.random && enemy.idle && enemy.enemyClass == Squid)
                {
                    enemy.rect.x = enemy.targetFinalPosition.x + globalEnemyOffsetX;
                }
            }

            // === Attacing manager ===
            if (!gameOver)
            {
                if (enemy.attackCooldown >= 1.5f)
                {
                    enemy.attackTime = (float)GetRandomValue(1, 500); // Random attack time

                    if (enemy.attackTime <= 10)
                    {
                        // End random state

                        enemy.attackCooldown = 0; // Reset the cooldown
                        Bullet_Enemy newBullets_Enemy;

                        newBullets_Enemy.rect = { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 16, 12 };
                        newBullets_Enemy.active = true;

                        enemyBullets.push_back({ newBullets_Enemy });

                        // Start attacing patrol
                    }
                }

                else if (enemy.attackCooldown < 1.5f)
                {
                    enemy.attackCooldown += deltaTime;
                }
            }
        }
    }
}