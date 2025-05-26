#include "Mantis.h"
#include <cmath>
#include <iostream>

using namespace std;

// Enemy constructor
MantisEnemy::MantisEnemy() {}

const int screenWidth = 1152;
const int screenHeight = 896;

void MantisEnemy::MantisAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, float deltaTime, float globalEnemyOffsetXN, int attackdirnum)
{
    float distX1;
    float moveSpeed = 300 * deltaTime;
    enemy.attackingTimer += deltaTime;

    // Move the enemy to the target

    if (attackdirnum == 0) distX1 = enemy.targetFinalPosition.x - 100 - enemy.rect.x;
    else distX1 = enemy.targetFinalPosition.x + 100 - enemy.rect.x;

    float distY1 = screenHeight / 2 - enemy.rect.y;
    float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Total distance to the objective

    float distX2 = enemy.targetFinalPosition.x + globalEnemyOffsetXN - enemy.rect.x;
    float distY2 = enemy.targetFinalPosition.y - enemy.rect.y;
    float distance2 = sqrt(distX2 * distX2 + distY2 * distY2); // Total distance to the objective

    if (enemy.attackingTimer <= 1)
    {
        // Guardamos la dirección solo si no estamos en el último frame antes del cambio
        if (enemy.attackingTimer <= 0.5f) // margen de seguridad
        {
            float directionX = distX1 / distance1;
            float directionY = distY1 / distance1;

            enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90;
        }

        // Seguir moviendo igual
        float directionX = distX1 / distance1;
        float directionY = distY1 / distance1;

        enemy.rect.x += directionX * moveSpeed;
        enemy.rect.y += directionY * moveSpeed;

        enemy.playerPos.x = player.x;
        enemy.playerPos.y = player.y;
    }

    else if (enemy.attackingTimer > 1 && !enemy.hasArribed)
    {
        // Normalize the direction
        float playerPosX = enemy.playerPos.x - enemy.rect.x;
        float playerPosY = enemy.playerPos.y - enemy.rect.y;
        float distanceToPlayer = sqrt(playerPosX * playerPosX + playerPosY * playerPosY);

        float directionX = playerPosX / distanceToPlayer;
        float directionY = playerPosY / distanceToPlayer;

        enemy.rotation = atan2(directionY, directionX) * (180.0f / PI) + 90; // Convertir a grados

        // Move the enemy to the objective
        enemy.rect.x += directionX * moveSpeed;
        enemy.rect.y += directionY * moveSpeed;

        if (enemy.rect.y >= player.y - 1) enemy.hasArribed = true;
    }

    else if (enemy.hasArribed)
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
            enemy.hasArribed = false;

            // Restart random state
        }
    }
}