#include "Enemy.h"
#include "Draconoida.h"
#include "Mantis.h"
#include "Kraken.h"
#include "Squid.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Enemy::Bullet_Enemy() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Enemy::Enemy()
    : enemyClass(), rect{ 0, 0, 64, 64 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, active(true), isAttacking(false), attackTime(0.0f),
    attackingTimer(0.0f), attackCooldown(0.0f), targetPosition1{ 0.0f, 0.0f }, targetFinalPosition{ 0.0f, 0.0f }, 
    targetIdlePosition{ 0.0f, 0.0f }, playerPos{ 0.0f, 0.0f }, attackPlayerPos(0.0f), entryTime(0.0f), rotation(0.0f), index(0),
    loopDirectionX(0), loopDirectionY(0), loopTime(0.0f), currentEnemies(0), enemyInitialState(true), 
    enemyLoopState(false), manual(true), idle(false), random(false), right(false), playerOnRight(false), 
    canAttack(true), gotHit(false), krakenHit(false), hasArribed(false), enemyDeathFramesCounter(0), currentEnemyDeathFrame(0), 
    enemyStartX(0), picked(NULL) { }

DraconoidaEnemy draconoida;
MantisEnemy mantis;
SquidEnemy squid;
KrakenEnemy kraken;

const int screenWidth = 1152;
const int screenHeight = 896;

// Variables globales (fuera del bucle principal y de la clase Enemy)
float globalEnemyOffsetXN = 0.0f;
float globalEnemyDirectionN = 1.0f;
float maxEnemyOffsetN = 60.0f;
float enemyMoveSpeedN = 150.0f; // píxeles por segundo

float globalEnemyOffsetX = 0.0f;
float globalEnemyDirection = 1.0f;
float maxEnemyOffset = 352;
float enemyMoveSpeed = 450.0f; // píxeles por segundo

float Enemy::Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void Enemy::UpdateEnemyOffsetNormal(float deltaTime)
{
    globalEnemyOffsetXN += globalEnemyDirectionN * enemyMoveSpeedN * deltaTime;

    if (globalEnemyOffsetXN > maxEnemyOffsetN)
    {
        globalEnemyOffsetXN = maxEnemyOffsetN;
        globalEnemyDirectionN *= -1;
    }
    else if (globalEnemyOffsetXN < -maxEnemyOffsetN)
    {
        globalEnemyOffsetXN = -maxEnemyOffsetN;
        globalEnemyDirectionN *= -1;
    }
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
    int j = 1;
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

        if (newEnemy.enemyClass == Squid)
        {
            newEnemy.enemyStartX = startX;
            startY = screenHeight / 2 - 150;
            targetY = startY;

            finaltargetX = screenWidth / 3 * (j) - 32; // Seconds target X after the first loop (Idle movement)
            j++;
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

void Enemy::UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, float deltaTime, Rectangle& player, bool gameOver, bool isInvencibilityDelayTimerStarted)
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
        if (enemy.enemyClass != Squid)
        {
            int attackdirnum = 0;

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
                        float distX = enemy.targetFinalPosition.x + globalEnemyOffsetXN - enemy.rect.x; // X Distance to the final position of patrol movement
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
                        if (!enemy.random && enemy.idle && enemy.enemyClass != Squid)
                        {
                            enemy.rect.x = enemy.targetFinalPosition.x + globalEnemyOffsetXN;
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
                                if (enemy.enemyClass == Mantis) attackdirnum = GetRandomValue(0, 1);
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

                // === Starts attack phase === 
                else if (enemy.idle && enemy.random)
                {
                    if (enemy.enemyClass == Draconoida)
                    {
                        draconoida.DraconoidaAttackManager(enemyBullets, enemies, enemy, player, GetFrameTime(), globalEnemyOffsetXN);
                    }

                    else if (enemy.enemyClass == Mantis)
                    {
                        mantis.MantisAttackManager(enemyBullets, enemies, enemy, player, GetFrameTime(), globalEnemyOffsetXN, attackdirnum);
                    }

                    else if (enemy.enemyClass == Kraken)
                    {
                        kraken.KrakenAttackManager(enemyBullets, enemies, enemy, player, GetFrameTime(), globalEnemyOffsetXN, isInvencibilityDelayTimerStarted);
                    }
                }
            }
        }

        // === SQUID === // 
        else
        {
            squid.SquidAttackManager(enemyBullets, enemies, enemy, player, GetFrameTime(), globalEnemyOffsetX, enemy.enemyStartX, gameOver);
        }
        if (kraken.playerPicked) picked = true;
        else picked = false;
    }
}