#include "Enemy.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Enemy::Bullet_Enemy() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Enemy::Enemy()
    : enemyClass(), rect{ 0, 0, 64, 64 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, active(true), isAttacking(false), attackTime(0.0f),
    attackingTimer(0.0f), attackCooldown(0.0f), targetPosition1{ 0.0f, 0.0f }, targetFinalPosition{ 0.0f, 0.0f }, 
    targetIdlePosition{ 0.0f, 0.0f },attackPlayerPos(0.0f), entryTime(0.0f), rotation(0.0f), index(0), 
    loopDirectionX(0), loopDirectionY(0), loopTime(0.0f), currentEnemies(0), enemyInitialState(true), 
    enemyLoopState(false), manual(true), idle(false), random(false), right(false), playerOnRight(false), 
    canAttack(true), gotHit(false), enemyDeathFramesCounter(0), currentEnemyDeathFrame(0){}

const int screenWidth = 1152;
const int screenHeight = 896;

// Variables globales (fuera del bucle principal y de la clase Enemy)
float globalEnemyOffsetX = 0.0f;
float globalEnemyDirection = 1.0f;
float maxEnemyOffset = 60.0f;
float enemyMoveSpeed = 150.0f; // píxeles por segundo

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void Enemy::UpdateEnemyOffset(float deltaTime)
{
    globalEnemyOffsetX += globalEnemyDirection * enemyMoveSpeed * deltaTime;

    if (globalEnemyOffsetX > maxEnemyOffset)
    {
        globalEnemyOffsetX = maxEnemyOffset;
        globalEnemyDirection *= -1;
    }
    else if (globalEnemyOffsetX < -maxEnemyOffset)
    {
        globalEnemyOffsetX = -maxEnemyOffset;
        globalEnemyDirection *= -1;
    }
}

// === ENEMY PRESETS ===
// 1: Draconoida, Mantis, Draconoida, Mantis, Draconoida
// 2: Draconoida, Mantis, Mantis, Mantis, Draconoida
// 3: Squid, Draconoida, Mantis, Draconoida, Squid
// 4: Kraken, Kraken, Squid, Kraken, Kraken
// 5: Kraken, Draconoida, Mantis, Draconoida, Mantis
// 6: Squid, Mantis, Squid, Mantis, Squid

// Function to spawn the enemies
void Enemy::SpawnEnemies(std::vector<Enemy>& enemies, int numberEnemies, float baseHeight, float baseWidth, int directionX, int directionY, float loopTime, float targetx, float targety, int currentEnemies, int enemyPreset)
{
    for (int i = 0; i < numberEnemies; i++)
    {
        float delay = i * 0.35f; // Delay entry of the enemies
        float startX = baseWidth; // Start X point
        float startY = baseHeight; // Start Y point
        float targetX = targetx; // Target X for the first loop
        float targetY = targety; // Target Y for the first loop
        float idletargetX = screenWidth / 6.0f * (i + 0.75f) - 25; // First target X after the first loop (Idle movement)
        float finaltargetX = screenWidth / 6.0f * (i + 0.75f) + 25; // Seconds target X after the first loop (Idle movement)
        float finaltargetY = baseHeight + 20.0f; // Height after the first loop (Idle movement)

        Enemy newEnemy;

        if (enemyPreset == 1)
        {
            if(i == 0) newEnemy.enemyClass = Draconoida;
            else if (i == 1) newEnemy.enemyClass = Mantis;
            else if (i == 2) newEnemy.enemyClass = Draconoida;
            else if (i == 3) newEnemy.enemyClass = Mantis;
            else newEnemy.enemyClass = Draconoida;
        }

        if (enemyPreset == 2)
        {
            if (i == 0) newEnemy.enemyClass = Draconoida;
            else if (i == 1) newEnemy.enemyClass = Mantis;
            else if (i == 2) newEnemy.enemyClass = Mantis;
            else if (i == 3) newEnemy.enemyClass = Mantis;
            else newEnemy.enemyClass = Draconoida;
        }

        if (enemyPreset == 3)
        {
            if (i == 0) newEnemy.enemyClass = Squid;
            else if (i == 1) newEnemy.enemyClass = Draconoida;
            else if (i == 2) newEnemy.enemyClass = Mantis;
            else if (i == 3) newEnemy.enemyClass = Draconoida;
            else newEnemy.enemyClass = Squid;
        }

        if (enemyPreset == 4)
        {
            if (i == 0) newEnemy.enemyClass = Kraken;
            else if (i == 1) newEnemy.enemyClass = Kraken;
            else if (i == 2) newEnemy.enemyClass = Squid;
            else if (i == 3) newEnemy.enemyClass = Kraken;
            else newEnemy.enemyClass = Kraken;
        }

        if (enemyPreset == 5)
        {
            if (i == 0) newEnemy.enemyClass = Kraken;
            else if (i == 1) newEnemy.enemyClass = Draconoida;
            else if (i == 2) newEnemy.enemyClass = Mantis;
            else if (i == 3) newEnemy.enemyClass = Draconoida;
            else newEnemy.enemyClass = Mantis;
        }

        if (enemyPreset == 6)
        {
            if (i == 0) newEnemy.enemyClass = Squid;
            else if (i == 1) newEnemy.enemyClass = Mantis;
            else if (i == 2) newEnemy.enemyClass = Squid;
            else if (i == 3) newEnemy.enemyClass = Mantis;
            else newEnemy.enemyClass = Squid;
        }

        newEnemy.rect = { startX, startY, 64, 64 };
        newEnemy.targetPosition1 = { targetX, targetY };
        newEnemy.targetFinalPosition = { finaltargetX, finaltargetY };
        newEnemy.targetIdlePosition = { idletargetX, finaltargetY };
        newEnemy.index = i;
        newEnemy.loopDirectionX = directionX;
        newEnemy.loopDirectionY = directionY;
        newEnemy.loopTime = loopTime;
        newEnemy.entryTime = -delay;

        enemies.push_back(newEnemy);
    }
}

void Enemy::UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, float deltaTime, Rectangle& player, bool gameOver)
{
    if (enemy.gotHit)
    {
        enemy.enemyDeathFramesCounter++;

        if (enemy.enemyDeathFramesCounter >= (3))
        {
            enemy.enemyDeathFramesCounter = 0;
            enemy.currentEnemyDeathFrame++;

            if (enemy.currentEnemyDeathFrame > 3) enemy.active = false;

            enemy.enemyDeathFrameRec.x = (float)enemy.currentEnemyDeathFrame * 54;
        }
    }

    else 
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
                    float dx = cos(loopT * PI * 2) * 5 * enemy.loopDirectionX;
                    float dy = -sin(loopT * PI * 2) * 5 * enemy.loopDirectionY;

                    enemy.rect.x += dx;
                    enemy.rect.y += dy;

                    // Rotación (atan2 toma (y, x))
                    enemy.rotation = atan2(dy, dx) * (180.0f / PI) + 90;

                    if (enemy.entryTime > enemy.loopTime)
                    {
                        enemy.enemyLoopState = false; // End the loop
                    }
                }

                // === FINAL STATE ==
                else if (!enemy.enemyInitialState && !enemy.enemyLoopState)
                {
                    float distX = enemy.targetFinalPosition.x + globalEnemyOffsetX - enemy.rect.x; // X Distance to the final position of patrol movement
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
                enemy.rotation = Lerp(enemy.rotation, 0.0f, 0.2f);

                if (fabs(enemy.rotation) < 0.02f)
                    enemy.rotation = 0.0f;

                // Dentro del bucle de actualización del juego

                // Actualizar la posición de todos los enemigos a la vez
                for (Enemy& enemy : enemies)
                {
                    if (!enemy.random && enemy.idle)
                    {
                        enemy.rect.x = enemy.targetFinalPosition.x + globalEnemyOffsetX;
                    }
                }

                // === Attacing manager ===
                if (!gameOver)
                {
                    if (enemy.attackCooldown >= 1.5f)
                    {
                        enemy.attackTime = (float)GetRandomValue(1, 5000); // Random attack time

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
            }

            else if (enemy.idle && enemy.random)
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

                    if (enemy.playerOnRight) distX1 = enemy.targetFinalPosition.x - 200 - enemy.rect.x;
                    else distX1 = enemy.targetFinalPosition.x + 200 - enemy.rect.x;

                    float distY1 = player.y - 100 - enemy.rect.y;
                    float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Total distance to the objective

                    float distX2 = enemy.targetFinalPosition.x + globalEnemyOffsetX - enemy.rect.x;
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
}