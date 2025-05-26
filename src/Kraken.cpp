#include "Kraken.h"
#include <cmath>
#include <iostream>
#include "Globals.h"

using namespace std;

// Enemy constructor
KrakenEnemy::KrakenEnemy() : attackCollider({ 0, 0, 0, 10 }) {}

const int screenWidth = 1152;
const int screenHeight = 896;

void KrakenEnemy::KrakenAttackManager(std::vector<Bullet_Enemy>& enemyBullets, std::vector<Enemy>& enemies, Enemy& enemy, Rectangle& player, Vector2& spritePos, float deltaTime, float globalEnemyOffsetXN, bool isInvencibilityDelayTimerStarted)
{
    float velocity2 = 300.0f;
    float moveSpeed = velocity2 * deltaTime;
    enemy.attackingTimer += deltaTime;

    float t = enemy.attackingTimer;
    float loopT = t * 0.5f;

    // Mini loop
    if (enemy.attackingTimer <= 1.2f)
    {
        float dx = cos(loopT * PI * 2.5f) * 5;
        float dy = -sin(loopT * PI * 2.5f) * 2.5f;

        enemy.rect.x += dx;
        enemy.rect.y += dy;

        enemy.rotation = atan2(dy, dx) * (180.0f / PI) + 90;
        enemy.attackPlayerPos = spritePos.x;
        enemy.enemyLoopState = true;
    }

    else
    {
        // 1. Movimiento hacia posici�n de ataque (debajo del jugador)
        float dxToAttackPos = spritePos.x + 10 - enemy.rect.x;
        float dyToAttackPos = screenHeight / 1.5f - enemy.rect.y - 90;
        float distToAttackPos = sqrt(dxToAttackPos * dxToAttackPos + dyToAttackPos * dyToAttackPos);

        float dirXToAttack = dxToAttackPos / distToAttackPos;
        float dirYToAttack = dyToAttackPos / distToAttackPos;

        // 2. Direcci�n para volver a la posici�n original (targetFinalPosition)
        float dxToFinal = enemy.targetFinalPosition.x + globalEnemyOffsetXN - enemy.rect.x;
        float dyToFinal = enemy.targetFinalPosition.y - enemy.rect.y;
        float distToFinal = sqrt(dxToFinal * dxToFinal + dyToFinal * dyToFinal);

        float dirXToFinal = dxToFinal / distToFinal;
        float dirYToFinal = dyToFinal / distToFinal;

        // --- Fase 1: Movimiento inicial hacia debajo del jugador
        if (distToAttackPos >= moveSpeed && enemy.enemyLoopState)
        {
            enemy.rotation = atan2(dirYToAttack, dirXToAttack) * (180.0f / PI) + 90;

            enemy.rect.x += dirXToAttack * moveSpeed;
            enemy.rect.y += dirYToAttack * moveSpeed;

            if (enemy.rect.y >= player.y - 105)
            {
                enemy.enemyLoopState = false;
                enemy.canAttack = true;
            }
        }

        // --- Fase 2: Ataque o regreso
        else if (!enemy.enemyLoopState)
        {
            if (enemy.canAttack)
            {
                // Posicionar el collider de ataque
                attackCollider.x = enemy.rect.x;
                attackCollider.y = player.y;

                // Intentar agarrar al jugador (solo si a�n no fue agarrado)
                if (CheckCollisionRecs(attackCollider, { spritePos.x, spritePos.y, 80, 80 }) &&
                    !playerPicked && !isInvencibilityDelayTimerStarted && !isPicked)
                {
                    // Mover el sprite del jugador con el Kraken
                    spritePos.x = enemy.rect.x;
                    spritePos.y = enemy.rect.y + 90;

                    playerPicked = true;
                    isPicked = true;
                    grabbingKraken = &enemy;
                }

                // Si este Kraken es el que tiene agarrado al jugador
                if (playerPicked && grabbingKraken == &enemy)
                {
                    // Movimiento de escape hacia arriba
                    float dxToEscape = enemy.targetFinalPosition.x - enemy.rect.x;
                    float dyToEscape = -300.0f - enemy.rect.y;
                    float distToEscape = sqrt(dxToEscape * dxToEscape + dyToEscape * dyToEscape);
                    float dirXEscape = dxToEscape / distToEscape;
                    float dirYEscape = dyToEscape / distToEscape;

                    enemy.rotation = Lerp(enemy.rotation, 0.0f, 0.2f);
                    if (fabs(enemy.rotation) < 0.02f) enemy.rotation = 0.0f;

                    if (distToEscape > moveSpeed)
                    {
                        // Mover al Kraken
                        enemy.rect.x += dirXEscape * moveSpeed;
                        enemy.rect.y += dirYEscape * moveSpeed;

                        // Mover el sprite del jugador con el Kraken
                        spritePos.x = enemy.rect.x - 7.5f;
                        spritePos.y = enemy.rect.y + 90;
                    }
                    else
                    {
                        // Suelta al jugador
                        playerPicked = false;
                        isPicked = false;
                        grabbingKraken = nullptr;

                        enemy.canAttack = false;
                        enemy.attackingTimer = 0;
                        enemy.random = false;

                        // Deja al jugador donde termin� el Kraken
                        spritePos.x = enemy.rect.x;
                        spritePos.y = enemy.rect.y;
                    }
                }

                // Si no lo agarr�, regresar al targetFinalPosition
                else
                {
                    if (distToFinal > moveSpeed)
                    {
                        // ROTAR hacia la direcci�n de movimiento
                        enemy.rotation = atan2(dirYToFinal, dirXToFinal) * (180.0f / PI) + 90;

                        // MOVER hacia la posici�n final
                        enemy.rect.x += dirXToFinal * moveSpeed;
                        enemy.rect.y += dirYToFinal * moveSpeed;
                    }
                    else
                    {
                        // Lleg� al destino, reinicia
                        enemy.canAttack = false;
                        enemy.attackingTimer = 0;
                        enemy.random = false;
                    }
                }
            }
        }
    }
}