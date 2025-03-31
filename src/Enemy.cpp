#include "Enemy.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Enemy::Bullet_Enemy() : rect{ 0, 0, 0, 0 }, active(false) {}

// Enemy constructor
Enemy::Enemy()
    : rect{ 0, 0, 48, 48 }, active(true), isAttacking(false), attackTime(0.0f),
    attackingTimer(0.0f), attackCooldown(0.0f), targetPosition1{ 0.0f, 0.0f },
    targetFinalPosition{ 0.0f, 0.0f }, targetIdlePosition{ 0.0f, 0.0f },
    attackPlayerPos(0.0f), entryTime(0.0f), index(0), loopDirection(1),
    currentEnemies(5), enemyInitialState(true), enemyLoopState(false), manual(true),
    idle(false), random(false), right(false), playerOnRight(false), canAttack (true) {
}

const int screenWidth = 1152;
const int screenHeight = 896;

// Función para generar enemigos en una ola
void Enemy::SpawnEnemies(std::vector<Enemy>& enemies, int numberEnemies, int currentEnemies,float baseHeight, float baseWidth, int direction, float targetx, float targety)
{
    enemies.clear();
    for (int i = 0; i < numberEnemies; i++)
    {
        float delay = i * 0.35f;
        float startX = baseWidth;
        float startY = baseHeight;
        float targetX = targetx;
        float targetY = targety;
        float idletargetX = screenWidth / 6.0f * (i + 0.75f) + 50;
        float finaltargetX = screenWidth / 6.0f * (i + 0.75f) - 50;
        float finaltargetY = baseHeight + 20.0f;

        Enemy newEnemy;
        newEnemy.rect = { startX, startY, 48, 48 };
        newEnemy.targetPosition1 = { targetX, targetY };
        newEnemy.targetFinalPosition = { finaltargetX, finaltargetY };
        newEnemy.targetIdlePosition = { idletargetX, finaltargetY };
        newEnemy.index = i;
        newEnemy.loopDirection = direction;
        newEnemy.entryTime = -delay;

        enemies.push_back(newEnemy);
    }
    currentEnemies = numberEnemies;
}

void Enemy::UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, Enemy& enemy, float deltaTime, Rectangle& player)
{
    float midX = screenWidth / 2.0f;
    float midY = enemy.rect.y;

    enemy.entryTime += deltaTime;

    // === NEW ENEMY MOVEMENT ===

    // El retraso se calcula dependiendo del índice del enemigo
    float delayTime = 0.0f; // Por ejemplo, 0.5 segundos de retraso por cada enemigo

    // Enemy start de movement with a delay 

    if (enemy.entryTime >= delayTime)
    {
        // === NEW ENEMY MOVEMENT ===

        // Calcular la posición circular
        float radius = 150.0f;  // Radio del círculo
        float centerX = enemy.targetPosition1.x; // Centro de la órbita
        float centerY = enemy.targetPosition1.y + radius;

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

                    // Mover al enemigo hacia el objetivo
                    enemy.rect.x += directionX * moveSpeed;
                    enemy.rect.y += directionY * moveSpeed;
                }

                else
                {
                    enemy.enemyInitialState = false;
                    enemy.enemyLoopState = true;
                }
            }

            // CORREGIR PUNTO DE INICIO DEL LOOP

            // === LOOP MOVEMENT ===
            else if (!enemy.enemyInitialState && enemy.enemyLoopState)
            {
                float t = enemy.entryTime;
                float loopT = t * 0.5f;  // Controlar la velocidad angular (ajusta este valor si es necesario)

                enemy.rect.x -= cos(loopT * PI * 2) * 5; // Movimiento en X
                enemy.rect.y -= sin(loopT * PI * 2) * 5 * enemy.loopDirection; // Movimiento en Y

                if (enemy.entryTime > 4.5f)
                {
                    enemy.enemyLoopState = false;
                }
            }

            // === FINAL STATE ==
            else if (!enemy.enemyInitialState && !enemy.enemyLoopState)
            {
                float distX = enemy.targetFinalPosition.x - enemy.rect.x;
                float distY = enemy.targetFinalPosition.y - enemy.rect.y;

                float distance = sqrt(distX * distX + distY * distY); // Distancia total al objetivo

                if (distance > moveSpeed)
                {
                    // Normalizar la dirección
                    float directionX = distX / distance;

                    float directionY = distY / distance;
                    // Mover al enemigo hacia el objetivo
                    enemy.rect.x += directionX * moveSpeed;
                    enemy.rect.y += directionY * moveSpeed;
                }

                else
                {
                    enemy.rect.x = enemy.targetFinalPosition.x;
                    enemy.rect.y = enemy.targetFinalPosition.y;

                    enemy.manual = false;
                    enemy.idle = true;
                }
            }
        }

        // === RANDOM STATE

        else if (enemy.idle && !enemy.random)
        {
            // Incrementar 't' para el movimiento circular
            float velocity2 = 200.0f; // Velocidad de movimiento
            float moveSpeed = velocity2 * deltaTime;

            float distX1 = enemy.targetIdlePosition.x - enemy.rect.x;

            float distance1 = sqrt(distX1 * distX1); // Distancia total al objetivo
            float directionX1 = distX1 / distance1;

            float distX2 = enemy.targetFinalPosition.x - enemy.rect.x;

            float distance2 = sqrt(distX2 * distX2); // Distancia total al objetivo
            float directionX2 = distX2 / distance2;

            if (!enemy.right)
            {
                if (distance1 > moveSpeed)
                {
                    // Mover al enemigo hacia el objetivo
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
                    // Mover al enemigo hacia el objetivo
                    enemy.rect.x += directionX2 * moveSpeed;
                }

                else
                {
                    enemy.right = false;
                }
            }

            if (enemy.attackCooldown >= 1.5f)
            {
                enemy.attackTime = GetRandomValue(1, 5000);
                if (enemy.attackTime <= 10)
                {
                    enemy.attackCooldown = 0;
                    enemy.canAttack = true;
                    enemy.random = true;
                }
            }

            else if (enemy.attackCooldown < 1.5f)
            {
                enemy.attackCooldown += deltaTime;
            }
        }

        else if (enemy.random)
        {
            float velocity2 = 300.0f; // Velocidad de movimiento
            float moveSpeed = velocity2 * deltaTime;
            enemy.attackingTimer += deltaTime;

            float t = enemy.attackingTimer;
            float loopT = t * 0.5f;

            if (enemy.attackingTimer <= 1.2f)
            {
                if (enemy.attackingTimer <= 0.7f)
                {
                    enemy.rect.x += cos(loopT * PI * 2.5f) * 5; // Movimiento en X
                    enemy.rect.y -= sin(loopT * PI * 2.5f) * 2.5f; // Movimiento en Y
                }

                else
                {
                    enemy.rect.x += cos(loopT * PI * 2.5f) * 5;
                    enemy.rect.y -= sin(loopT * PI * 2.5f) * 2.5f;
                }

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
                if (enemy.playerOnRight) distX1 = enemy.targetIdlePosition.x - 400 - enemy.rect.x;
                else distX1 = enemy.targetIdlePosition.x + 400 - enemy.rect.x;
                float distY1 = player.y - 100 - enemy.rect.y;
                float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Distancia total al objetivo

                float distX2 = enemy.targetIdlePosition.x - enemy.rect.x;
                float distY2 = enemy.targetFinalPosition.y - enemy.rect.y;
                float distance2 = sqrt(distX2 * distX2 + distY2 * distY2); // Distancia total al objetivo

                cout << enemy.attackingTimer << endl;

                if (enemy.attackingTimer >= 2.5f && enemy.canAttack )
                {
                    cout << "Technically attacking";
                    
                    Bullet_Enemy newBullets_Enemy;

                    newBullets_Enemy.rect = { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 16, 12 };
                    newBullets_Enemy.active = true;

                    enemyBullets.push_back({ newBullets_Enemy });

                    enemy.canAttack = false;
                }

                if (distance1 >= moveSpeed && enemy.enemyLoopState)
                {
                    // Normalizar la dirección
                    float directionX = distX1 / distance1;
                    float directionY = distY1 / distance1;

                    // Mover al enemigo hacia el objetivo
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
                        // Normalizar la dirección
                        float directionX = distX2 / distance2;
                        float directionY = distY2 / distance2;

                        // Mover al enemigo hacia el objetivo
                        enemy.rect.x += directionX * moveSpeed;
                        enemy.rect.y += directionY * moveSpeed;
                    }

                    else
                    {
                        enemy.random = false;
                        enemy.attackingTimer = 0.0f;
                    }
                }
            }
        }
    }
}
