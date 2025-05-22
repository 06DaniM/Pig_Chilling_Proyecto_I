#include "Boss.h"
#include <cmath>
#include <iostream>

using namespace std;

// Bullet_Enemy constructor
Bullet_Boss::Bullet_Boss() : rect{ 0, 0, 0, 0 }, pos{ 0,0 }, lifetime(0.0f), active(false) {}

// Enemy constructor
Boss::Boss()
    : rect{ 0, 0, 256, 256 }, enemyDeathFrameRec{ 0, 0, 54, 54 }, life(100), active(false), dying(false), dead(false), attackRutine(0),
    laserAttackNormal(false), laserAttackHeavy(false), shooting(false), attackTime(0.0f), attackingTimer(0.0f), attackCooldown(0.0f),
    playerPos{ 0.0f, 0.0f }, rotation(0.0f), appearance(false), idle(false), random(false), canAttack(true), hasArribed(false),
    enemyDeathFramesCounter(0), currentEnemyDeathFrame(0), start({ 0,0 }), target({ 0,0 }), currentPattern(ATTACK_NONE), patternTimer(0.0f), patternCooldown(0.0f),
    laserActive(false), laserTimer(0.0f), wideBeamActive(false), wideBeamTimer(0.0f), wideBeamRect({ 0,0 }), bulletDodgeActive(false), bulletDodgeTimer(0.0f), 
    bulletSpawnCooldown(0.0f)
{}

const int screenWidth = 1152;
const int screenHeight = 896;
std::vector<Bullet_Boss> dodgeBullets;

void Boss::BossSpawn()
{
    cout << "Boss spawned" << endl;

    // Spawn position
    start.x = (screenWidth - rect.width) / 2;
    start.y = -rect.height;


    // Position to move for starting the fight
    target.x = start.x;
    target.y = 100;

    rect = { start.x, start.y, 256, 256};
    active = true;
    appearance = true;
}

void Boss::SelectNextPattern()
{
    // Aleatorio:
    currentPattern = static_cast<BossAttackPattern>(GetRandomValue(0, 2));
}

void Boss::LaserDiagonalPattern()
{
    if (!laserActive)
    {
        // Activar rayos
        laserActive = true;
        laserTimer = 4.0f; // Duran 2 segundos
        // Aquí podrías activar sonido, animaciones, etc.
    }

    // Dibujar rayos
    Vector2 origin = { rect.x + rect.width / 2, rect.y + rect.height };

    // Central recto
    DrawRectangle(origin.x - 50, origin.y, 100, 800, RED);

    // Izquierda diagonal (simulada con líneas)
    DrawLineEx({ origin.x - 300, origin.y }, { origin.x - 50, origin.y + 800 }, 50, RED);

    // Derecha diagonal
    DrawLineEx({ origin.x + 300, origin.y }, { origin.x + 50, origin.y + 800 }, 50, RED);

    // Timer para apagar los rayos
    laserTimer -= GetFrameTime();
    if (laserTimer <= 0)
    {
        laserActive = false;
        // Termina el patrón después de mostrar los rayos
        patternCooldown = 3.0f; // tiempo hasta el siguiente patrón
        currentPattern = ATTACK_NONE; // o cualquier estado de espera
    }
}

void Boss::WideBeamAttack()
{
    if (!wideBeamActive)
    {
        wideBeamActive = true;
        wideBeamTimer = 1.5f; // Duración del ataque

        // Posición del rayo ancho
        float beamWidth = 300.0f;
        float beamX = rect.x + rect.width / 2 - beamWidth / 2;
        wideBeamRect = { beamX, rect.y + rect.height, beamWidth, 800 }; // hacia abajo
    }

    // Dibujar el rayo ancho
    DrawRectangleRec(wideBeamRect, ORANGE);

    // Timer para desactivarlo
    wideBeamTimer -= GetFrameTime();
    if (wideBeamTimer <= 0)
    {
        wideBeamActive = false;
        patternCooldown = 3.0f;
        currentPattern = ATTACK_NONE; // o un estado de espera
    }
}

void Boss::BulletDodgePattern()
{
    if (!bulletDodgeActive)
    {
        bulletDodgeActive = true;
        bulletDodgeTimer = 4.0f;
        dodgeBullets.clear();
        bulletSpawnCooldown = 0.0f;
    }

    // Rayos laterales (solo visuales aquí)
    float laserHeight = 800;
    DrawRectangle(rect.x + 20, rect.y + rect.height, 10, laserHeight, BLUE); // izquierda
    DrawRectangle(rect.x + rect.width - 30, rect.y + rect.height, 10, laserHeight, BLUE); // derecha

    // Disparo de balas en el centro en onda
    bulletSpawnCooldown -= GetFrameTime();
    if (bulletSpawnCooldown <= 0.0f)
    {
        Bullet_Boss b;
        b.pos = { rect.x + rect.width / 2, rect.y + rect.height };
        b.lifetime = 0.0f;
        b.active = true;
        dodgeBullets.push_back(b);
        bulletSpawnCooldown = 0.2f; // disparar cada 0.2 segundos
    }

    // Actualizar y dibujar balas
    for (auto& b : dodgeBullets)
    {
        if (!b.active) continue;

        b.lifetime += GetFrameTime();
        b.pos.y += 250 * GetFrameTime();
        b.pos.x += sinf(b.lifetime * 6.0f) * 100 * GetFrameTime(); // movimiento en onda

        DrawCircleV(b.pos, 8, YELLOW);
    }

    // Puedes desactivarla si se sale de pantalla
    dodgeBullets.erase(
        std::remove_if(dodgeBullets.begin(), dodgeBullets.end(), [](const Bullet_Boss& b) { return !b.active; }),
        dodgeBullets.end()
    );

    // Finalizar patrón después de cierto tiempo
    bulletDodgeTimer -= GetFrameTime();
    if (bulletDodgeTimer <= 0)
    {
        bulletDodgeActive = false;
        patternCooldown = 3.0f;
        currentPattern = ATTACK_NONE;
    }
}

void Boss::BossManager()
{
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