#include "Enemy.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Enemy::Bullet_Enemy() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Enemy::Enemy()
    : rect{ 0, 0, 32, 32 }, active(true), isAttacking(false), attackTime(0.0f),
    attackingTimer(0.0f), attackCooldown(0.0f), targetPosition1{ 0.0f, 0.0f },
    targetFinalPosition{ 0.0f, 0.0f }, targetIdlePosition{ 0.0f, 0.0f },
    attackPlayerPos(0.0f), entryTime(0.0f), rotation(0.0f), index(0), loopDirection(1),
    currentEnemies(5), enemyInitialState(true), enemyLoopState(false), manual(true),
    idle(false), random(false), right(false), playerOnRight(false), canAttack(true) {
}

const int screenWidth = 1152;
const int screenHeight = 896;

// Function to spawn the enemies
void Enemy::SpawnEnemies(std::vector<Enemy>& enemies, int numberEnemies, int currentEnemies, float baseHeight, float baseWidth, int direction, float targetx, float targety)
{
    enemies.clear();
    for (int i = 0; i < numberEnemies; i++)
    {
        float delay = i * 0.35f; // Delay entry of the enemies
        float startX = baseWidth; // Start X point
        float startY = baseHeight; // Start Y point
        float targetX = targetx; // Target X for the first loop
        float targetY = targety; // Target Y for the first loop
        float idletargetX = screenWidth / 6.0f * (i + 0.75f) + 50; // First target X after the first loop (Idle movement)
        float finaltargetX = screenWidth / 6.0f * (i + 0.75f) - 50; // Seconds target X after the first loop (Idle movement)
        float finaltargetY = baseHeight + 20.0f; // Height after the first loop (Idle movement)

        Enemy newEnemy;
        newEnemy.rect = { startX, startY, 52, 52 };
        newEnemy.targetPosition1 = { targetX, targetY };
        newEnemy.targetFinalPosition = { finaltargetX, finaltargetY };
        newEnemy.targetIdlePosition = { idletargetX, finaltargetY };
        newEnemy.index = i;
        newEnemy.loopDirection = direction;
        newEnemy.entryTime = -delay;

        enemies.push_back(newEnemy);
    }
    currentEnemies = numberEnemies; // Will be change to += when wave timer is applied
}

void Enemy::UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, Enemy& enemy, float deltaTime, Rectangle& player)
{
    float midX = screenWidth / 2.0f; // Middle of the screen in X axis

    enemy.entryTime += deltaTime; // Timer when the enemy has started moving

    // === NEW ENEMY MOVEMENT ===

    float delayTime = 0.0f; // Delay for the enemy.entrytime

    // Enemy start de movement with a delay 
    if (enemy.entryTime >= delayTime)
    {
        // === NEW ENEMY MOVEMENT ===

        float radius = 150.0f;  // Radius of the loop
        float centerX = enemy.targetPosition1.x; // Center of the X orbit
        float centerY = enemy.targetPosition1.y + radius; // Center of the Y orbit

        // === PATROL STATE === 

        if (enemy.manual)
        {
            // Incrementar 't' para el movimiento circular
            float velocity = 500.0f; // Velocidad de movimiento
            float moveSpeed = velocity * deltaTime;

            // === 1ST OBJECTIVE ===
            if (enemy.enemyInitialState)
            {
                // Calcular la distancia entre la posición actual y el objetivo
                float distX = enemy.targetPosition1.x - enemy.rect.x;
                float distY = enemy.targetPosition1.y - enemy.rect.y;

                float distance = sqrt(distX * distX + distY * distY); // Distancia total al objetivo

                enemy.enemyLoopState = false;
                if (distance > moveSpeed)
                {
                    // Normalizar la dirección
                    float directionX = distX / distance;
                    float directionY = distY / distance;

                    enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90; // Convertir a grados

                    // Mover al enemigo hacia el objetivo
                    enemy.rect.x += directionX * moveSpeed;
                    enemy.rect.y += directionY * moveSpeed;
                }

                else
                {
                    // End of initial state

                    enemy.enemyInitialState = false;
                    enemy.enemyLoopState = true;

                    // Starting loop
                }
            }

            // CORREGIR PUNTO DE INICIO DEL LOOP

            // === LOOP MOVEMENT ===
            else if (!enemy.enemyInitialState && enemy.enemyLoopState)
            {
                float t = enemy.entryTime;
                float loopT = t * 0.5f;  // Speed of the loop movment

                // Movimiento
                float dx = -cos(loopT * PI * 2) * 5;
                float dy = -sin(loopT * PI * 2) * 5 * enemy.loopDirection;

                enemy.rect.x += dx;
                enemy.rect.y += dy;

                // Rotación (atan2 toma (y, x))
                enemy.rotation = atan2(dy, dx) * (180.0f / PI) + 90;

                if (enemy.entryTime > 4.5f)
                {
                    enemy.enemyLoopState = false; // End the loop
                }
            }

            // === FINAL STATE ==
            else if (!enemy.enemyInitialState && !enemy.enemyLoopState)
            {
                float distX = enemy.targetFinalPosition.x - enemy.rect.x; // X Distance to the final position of patrol movement
                float distY = enemy.targetFinalPosition.y - enemy.rect.y; // Y Distance to the final position of patrol movement

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

                    enemy.rect.x = enemy.targetFinalPosition.x;
                    enemy.rect.y = enemy.targetFinalPosition.y;

                    enemy.manual = false;
                    enemy.idle = true;

                    //Starting idle movement
                }
            }
        }

        // === RANDOM STATE

        else if (enemy.idle && !enemy.random)
        {
            enemy.rotation = 0;

            // Añadir offset para que se muevan al mismo tiempo
            float velocity2 = 200.0f; // Movement speed
            float moveSpeed = velocity2 * deltaTime;

            float distX1 = enemy.targetIdlePosition.x - enemy.rect.x;

            float distance1 = sqrt(distX1 * distX1); // Total distance to the objective
            float directionX1 = distX1 / distance1;

            float distX2 = enemy.targetFinalPosition.x - enemy.rect.x;

            float distance2 = sqrt(distX2 * distX2); // Total distance to the objective
            float directionX2 = distX2 / distance2;

            // === Logic for movint the enemy to the right and left automatically ===
            if (!enemy.right)
            {
                if (distance1 > moveSpeed)
                {
                    // Move the enemy to the objective
                    enemy.rect.x += directionX1 * moveSpeed;
                }

                else
                {
                    enemy.right = true;
                }
            }

            else
            {
                if (distance2 > moveSpeed)
                {
                    // Move the enemy to the objective
                    enemy.rect.x += directionX2 * moveSpeed;
                }

                else
                {
                    enemy.right = false;
                }
            }

            // === Attacing manager ===
            if (enemy.attackCooldown >= 1.5f)
            {
                enemy.attackTime = GetRandomValue(1, 5000); // Random attack time

                if (enemy.attackTime <= 10)
                {
                    // End random state

                    enemy.attackCooldown = 0; // Reset the cooldown
                    enemy.canAttack = true;
                    enemy.random = true;

                    // Start attacing patrol
                }
            }

            else if (enemy.attackCooldown < 1.5f)
            {
                enemy.attackCooldown += deltaTime;
            }
        }

        else if (enemy.random)
        {
            float velocity2 = 300.0f; // Movement speed
            float moveSpeed = velocity2 * deltaTime;
            enemy.attackingTimer += deltaTime;

            float t = enemy.attackingTimer;
            float loopT = t * 0.5f;

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

                if (enemy.playerOnRight) distX1 = enemy.targetIdlePosition.x - 400 - enemy.rect.x;
                else distX1 = enemy.targetIdlePosition.x + 400 - enemy.rect.x;

                float distY1 = player.y - 100 - enemy.rect.y;
                float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Total distance to the objective

                float distX2 = enemy.targetIdlePosition.x - enemy.rect.x;
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
    }
}