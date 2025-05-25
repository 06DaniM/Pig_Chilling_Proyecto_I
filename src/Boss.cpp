#include "Boss.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>


using namespace std;

// Bullet_Enemy constructor
Bullet_Boss::Bullet_Boss() : rect{ 0, 0, 0, 0 }, pos{ 0,0 }, lifetime(0.0f), active(false) {}

// Enemy constructor
Boss::Boss()
    : rect{ 0, 0, 432, 256 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, life(0), initialLife(100), active(false), dying(false), dead(false), attackRutine(0),
    laserAttackNormal(false), laserAttackHeavy(false), shooting(false), attackTime(0.0f), attackingTimer(0.0f), attackCooldown(0.0f),
    playerPos{ 0.0f, 0.0f }, rotation(0.0f), appearance(false), idle(false), random(false), canAttack(true), hasArribed(false),
    enemyDeathFramesCounter(0), currentEnemyDeathFrame(0), start({ 0,0 }), target({ 0,0 }), currentPattern(ATTACK_NONE), patternTimer(0.0f), patternCooldown(0.0f),
    centerLaserRect({ 0,0,0,0 }), leftDiagonalRect({ 0,0,0,0 }), rightDiagonalRect({ 0,0 }), laserActive(false), laserDamageActive(false), laserTimer(0), 
    wideBeamDamageActive(false), wideBeamActive(false), wideBeamTimer(0.0f), wideBeamRect({ 0,0,0,0 }), bulletDodgeActive(false), bulletLaserDamageActive(false), 
    leftRayRect({ 0,0,0,0 }), rightRayRect({ 0,0,0,0 }), bulletDodgeTimer(0.0f), bulletShootingTime(0.0f), bulletSpawnCooldown(0.0f), warningStarted(false), 
    warningTimer(0.0f), warningDurationLaserDiagonal(2.8f), warningDurationWideBeam(2.8f), warningDurationBulletDodge(3), warningAnimTexture({}), 
    warningAnimFrame({0,0,0,0}), warningAnimFrameIndex(0), warningAnimTimer(0.0f), warningAnimSpeed(0.05f), warningAnimPlaying(false), warningAnimFinished(false), 
    warningAnimReversing(false), diagonalLaserSFXActive(false), diagonalLaserSFXPlayed(false), wideBeamSFXActive(false), wideBeamSFXPlayed(false), 
    bulletDodgeSFXActive(false), bulletDodgeSFXPlayed(false), shieldActive(false), shieldTimer(0.0f), shieldDuration(0.0f), shieldRect({ 0 })
{}

const int screenWidth = 1152;
const int screenHeight = 896;

void Boss::BossSpawn()
{
    // Spawn position
    start.x = (screenWidth - rect.width) / 2;
    start.y = -rect.height;

    // Position to move for starting the fight
    target.x = start.x;
    target.y = 50;

    rect = { start.x, start.y, 432, 256};

    life = initialLife;

    dead = false;
    active = true;
    appearance = true;
}

void Boss::SelectNextPattern()
{
    // Aleatorio:
    currentPattern = static_cast<BossAttackPattern>(GetRandomValue(0, 2));
}

void Boss::PlayWarningAnimation()
{
    if (!warningAnimPlaying) return;

    warningAnimTimer += GetFrameTime();
    if (warningAnimTimer >= warningAnimSpeed)
    {
        warningAnimTimer = 0.0f;

        if (!warningAnimReversing)
        {
            warningAnimFrameIndex++;
            if (warningAnimFrameIndex >= 9)
            {
                warningAnimFrameIndex = 8; // Mantener el último frame visible
                warningAnimPlaying = false;
                warningAnimFinished = true;

                if (currentPattern != ATTACK_BULLET_DODGE) warningStarted = true;
            }
        }

        else // Si está reproduciendo en reversa
        {
            warningAnimFrameIndex--;
            if (warningAnimFrameIndex < 0)
            {
                warningAnimFrameIndex = 0;
                warningAnimPlaying = false;
                warningAnimReversing = false;

                // Termina la animación completamente
                warningAnimFinished = false;
            }
        }
    }

    warningAnimFrame = {
        (float)(warningAnimFrameIndex * 432),
        0.0f,
        432.0f,
        256.0f
    };
}

void Boss::LaserDiagonalPattern()
{
    if (!laserActive)
    {
        laserActive = true;
        laserTimer = 7.8f;

        // Iniciar advertencia
        warningTimer = warningDurationLaserDiagonal;

        // Iniciar animación
        warningAnimPlaying = true;
        warningAnimFinished = false;
        warningAnimReversing = false;
        warningAnimFrameIndex = 0;
        warningAnimTimer = 0.0f;

        laserDamageActive = false;
    }

    // 🎞️ Dibujar animación de advertencia
    if (warningAnimPlaying || warningAnimFinished)
    {
        PlayWarningAnimation();
    }

    if (warningAnimFinished)
    {
        // ⚠️ Dibujar advertencias visuales (círculos)
        Vector2 origin = { rect.x + rect.width / 2, rect.y + rect.height };
        DrawCircle((int)origin.x, (int)origin.y, 60, RED);
        DrawCircle((int)origin.x - 190, (int)origin.y, 60, RED);
        DrawCircle((int)origin.x + 190, (int)origin.y, 60, RED);
    }

    // 🛑 Fase de advertencia
    if (warningStarted)
    {
        diagonalLaserSFXActive = true;
        warningTimer -= GetFrameTime();
        if (warningTimer <= 0.0f)
        {
            warningStarted = false;
            laserDamageActive = true;
        }

        return; // Aún no empieza el ataque
    }

    if (!warningStarted && laserDamageActive)
    {
        // 🔥 Ataque activo
        Vector2 origin = { rect.x + rect.width / 2, rect.y + rect.height };

        DrawRectangle(origin.x - 50, origin.y - 50, 100, 800, RED); // rayo central
        DrawLineEx({ origin.x - 190, origin.y }, { origin.x + 950, origin.y + 800 }, 50, RED); // izquierda
        DrawLineEx({ origin.x + 190, origin.y }, { origin.x - 950, origin.y + 800 }, 50, RED); // derecha

        // 💥 Definir zonas de daño
        centerLaserRect = { origin.x - 50, origin.y - 50, 100, 800 };

        leftDiagonalRect = {
            205, 600,
            160, 50
        };

        rightDiagonalRect = {
            screenWidth - 365, 600,
            160, 50
        };

        DrawRectangleLines(leftDiagonalRect.x, leftDiagonalRect.y, leftDiagonalRect.width, leftDiagonalRect.height, GREEN);
        DrawRectangleLines(rightDiagonalRect.x, rightDiagonalRect.y, rightDiagonalRect.width, rightDiagonalRect.height, GREEN);

        // ⏲️ Timer del ataque
        laserTimer -= GetFrameTime();
    }

    if (laserTimer <= 0)
    {
        diagonalLaserSFXActive = false;
        diagonalLaserSFXPlayed = false;
        laserActive = false;
        laserDamageActive = false;
        patternCooldown = 3.0f;

        // ▶️ Reproducir animación en reversa
        warningAnimPlaying = true;
        warningAnimReversing = true;
        warningAnimTimer = 0.0f;
        warningAnimFrameIndex = 8;

        currentPattern = ATTACK_NONE;
    }
}

void Boss::WideBeamAttack()
{
    if (!wideBeamActive)
    {
        wideBeamActive = true;
        wideBeamTimer = 3.0f;

        warningStarted = true;
        warningTimer = warningDurationWideBeam;
        wideBeamDamageActive = false;  // AÚN NO es peligroso

        wideBeamRect = {
            rect.x + rect.width / 2 - 300,
            rect.y + rect.height,
            600,
            800
        };
    }

    wideBeamSFXActive = true;
    DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height), 80, ORANGE);

    if (warningStarted) {
        warningTimer -= GetFrameTime();

        if (warningTimer <= 0.0f) {
            warningStarted = false;
            wideBeamDamageActive = true;  // 🔥 AHORA sí hace daño
        }

        return;  // Todavía no dibujar el rayo ni aplicar daño
    }

    // Dibujar el rayo ancho
    DrawRectangleRec(wideBeamRect, ORANGE);

    // Timer para desactivarlo
    wideBeamTimer -= GetFrameTime();
    if (wideBeamTimer <= 0)
    {
        wideBeamActive = false;
        wideBeamSFXActive = false;
        wideBeamSFXPlayed = false;
        wideBeamDamageActive = false;
        patternCooldown = 3.0f;
        currentPattern = ATTACK_NONE; // o un estado de espera
    }
}

void Boss::BulletDodgePattern()
{
    float leftRayX = rect.x;
    float rightRayX = rect.x + rect.width;
    float gap = rightRayX - leftRayX;
    float bulletSpawnPositions[9] = {
        leftRayX + gap * 0.1f, leftRayX + gap * 0.2f, leftRayX + gap * 0.3f,
        leftRayX + gap * 0.4f, leftRayX + gap * 0.5f, leftRayX + gap * 0.6f,
        leftRayX + gap * 0.7f, leftRayX + gap * 0.8f, leftRayX + gap * 0.9f
    };

    leftRayRect = { rect.x - 400, rect.y + rect.height, 400, 800 };
    rightRayRect = { rect.x + rect.width, rect.y + rect.height, 400, 800 };

    // 1. INICIALIZACIÓN DEL PATRÓN
    if (!bulletDodgeActive && !warningAnimPlaying && !warningAnimReversing) {
        bulletDodgeActive = true;
        warningAnimPlaying = true;
        warningAnimReversing = false;
        warningAnimFrameIndex = 0;
        warningAnimTimer = 0.0f;
        warningStarted = false;
        bulletLaserDamageActive = false;
        bulletShootingTime = 0.0f;
        bulletSpawnCooldown = 0.0f;
        dodgeBullets.clear();
        return;
    }

    // 2. ANIMACIÓN DE ADVERTENCIA INICIAL
    if (warningAnimPlaying) {
        PlayWarningAnimation();

        // Cuando la animación termina este frame:
        if (!warningAnimPlaying && !warningAnimReversing && !warningStarted) {
            warningTimer = warningDurationBulletDodge;
            warningStarted = true;

            return;  // Evita que se reste el timer este frame
        }
        return; // Mientras la animación sigue, salir
    }

    // 3. MOSTRAR CÍRCULOS DURANTE ADVERTENCIA
    if (warningStarted && !bulletLaserDamageActive) {
        shieldActive = true;

        bulletDodgeSFXActive = true;
        DrawCircle((int)(rect.x - 200), (int)(rect.y + rect.height), 40, BLUE);
        DrawCircle((int)(rect.x + rect.width + 200), (int)(rect.y + rect.height), 40, BLUE);

        warningTimer -= GetFrameTime();
        if (warningTimer <= 0.0f && !bulletLaserDamageActive) {
            bulletLaserDamageActive = true;
            bulletShootingTime = (life >= initialLife * 0.5f) ? 15.0f :
                (life >= initialLife * 0.33f) ? 20.0f : 30.0f;
        }
        return;
    }

    // 4. RAYOS ACTIVOS
    if (bulletLaserDamageActive) {

        DrawRectangle(leftRayRect.x, leftRayRect.y, leftRayRect.width, 800, BLUE);
        DrawRectangle(rightRayRect.x, rightRayRect.y, rightRayRect.width, 800, BLUE);

        // Dibujar también los círculos
        DrawCircle((int)(rect.x - 200), (int)(rect.y + rect.height), 40, BLUE);
        DrawCircle((int)(rect.x + rect.width + 200), (int)(rect.y + rect.height), 40, BLUE);

        // 5. DISPARO DE BALAS
        if (bulletShootingTime > 0.0f) {
            bulletSpawnCooldown -= GetFrameTime();
            if (bulletSpawnCooldown <= 0.0f) {
                std::vector<int> validIndices;
                do {
                    int indices[] = { 0,1,2,3,4,5,6,7,8 };
                    std::shuffle(std::begin(indices), std::end(indices), std::default_random_engine((unsigned int)(GetTime() * 1000)));
                    validIndices = { indices[0], indices[1], indices[2], indices[3], indices[4] };
                    std::sort(validIndices.begin(), validIndices.end());

                    int consecutive = 1;
                    bool valid = true;
                    for (int i = 1; i < validIndices.size(); i++) {
                        if (validIndices[i] == validIndices[i - 1] + 1) {
                            consecutive++;
                            if (consecutive >= 3) {
                                valid = false;
                                break;
                            }
                        }
                        else {
                            consecutive = 1;
                        }
                    }

                    if (valid) break;
                } while (true);

                for (int i = 0; i < 5; i++) {
                    Bullet_Boss b;
                    b.pos = { bulletSpawnPositions[validIndices[i]], rect.y + rect.height - 50 };
                    b.lifetime = 0.0f;
                    b.active = true;
                    b.rect = { b.pos.x - 8, b.pos.y - 8, 16, 16 };
                    dodgeBullets.push_back(b);
                }

                bulletSpawnCooldown = 0.6f;
            }

            bulletShootingTime -= GetFrameTime();
        }

        // Movimiento de balas
        for (auto& b : dodgeBullets) 
        {
            if (!b.active) continue;
            b.lifetime += GetFrameTime();
            b.pos.y += (life >= initialLife * 0.5f ? 250 : 300) * GetFrameTime();
            b.rect = { b.pos.x - 8, b.pos.y - 8, 16, 16 };
            if (b.pos.y > 928) b.active = false;
        }

        // Limpiar balas inactivas
        dodgeBullets.erase(
            std::remove_if(dodgeBullets.begin(), dodgeBullets.end(), [](const Bullet_Boss& b) {
                return !b.active;
                }),
            dodgeBullets.end()
        );

        // 6. FINALIZAR PATRÓN
        if (bulletShootingTime <= 0.0f && dodgeBullets.empty()) {
            shieldActive = false;
            bulletDodgeSFXActive = false;
            bulletDodgeSFXPlayed = false;
            bulletLaserDamageActive = false;
            bulletDodgeActive = false;
            warningStarted = false;
            patternCooldown = 3.0f;

            warningAnimPlaying = true;
            warningAnimReversing = true;
            warningAnimFrameIndex = 8;
            warningAnimTimer = 0.0f;

            currentPattern = ATTACK_NONE;
        }
    }
}

void Boss::BossManager()
{
    if (warningAnimPlaying)
        PlayWarningAnimation();

    // Apareance state
    if (appearance)
    {
        float moveSpeed = 100.0f;
        float distY = target.y - rect.y;

        if (fabs(distY) > 1.0f)
            rect.y += moveSpeed * GetFrameTime();
        else
        {
            rect.y = target.y;
            appearance = false;
        }
    }

    patternCooldown -= GetFrameTime();
    if (!appearance && patternCooldown <= 0) {
        SelectNextPattern();
        patternCooldown = 10.0f;
    }

    shieldRect = {
    rect.x - 25, rect.y + rect.height,
    rect.width + 50, 20
    };

    // Ejecutar el patrón actual
    switch (currentPattern)
    {
    case ATTACK_LASER_DIAGONAL:
        LaserDiagonalPattern();
        break;
    case ATTACK_WIDE_BEAM:
        WideBeamAttack();
        break;
    case ATTACK_BULLET_DODGE:
        BulletDodgePattern();
        break;
    }
}