#ifndef BOSS_H
#define BOSS_H
#include <vector>
#include "raylib.h"

class Bullet_Boss {
public:
    Rectangle rect;
    Vector2 pos;
    float lifetime;
    bool active;

    Bullet_Boss();
};

enum BossAttackPattern {
    ATTACK_LASER_DIAGONAL,
    ATTACK_WIDE_BEAM,
    ATTACK_BULLET_DODGE,
    ATTACK_NONE
};

class Boss {
public:
    Rectangle rect;
    Rectangle enemyDeathFrameRec;
    int life;
    int initialLife;
    bool active;
    bool dying;
    bool dead;
    int attackRutine;
    bool laserAttackNormal;
    bool laserAttackHeavy;
    bool shooting;
    float attackTime;
    float attackingTimer;
    float attackCooldown;
    Vector2 playerPos;
    float rotation;
    bool appearance;
    bool idle;
    bool random;
    bool canAttack;
    bool hasArribed;
    int enemyDeathFramesCounter;
    int currentEnemyDeathFrame;
    Vector2 start;
    Vector2 target;
    BossAttackPattern currentPattern;
    float patternTimer;        // Duración del patrón actual
    float patternCooldown;     // Tiempo hasta el próximo patrón
    Rectangle centerLaserRect;
    Rectangle leftDiagonalRect;
    Rectangle rightDiagonalRect;
    bool laserActive;
    bool laserDamageActive;
    float laserTimer;
    bool wideBeamActive;
    bool wideBeamDamageActive;
    float wideBeamTimer;
    Rectangle wideBeamRect;
    bool bulletDodgeActive;
    bool bulletLaserDamageActive;
    Rectangle leftRayRect;
    Rectangle rightRayRect;
    float bulletDodgeTimer;
    float bulletSpawnCooldown;
    float bulletShootingTime;
    std::vector<Bullet_Boss> dodgeBullets;
    bool warningStarted;
    float warningTimer;
    const float warningDuration;


    // Constructor
    Boss();

    void BossSpawn();
    void BossManager(bool pause);
    void SelectNextPattern();
    void LaserDiagonalPattern(bool pause);
    void WideBeamAttack(bool pause);
    void BulletDodgePattern(bool pause);
};

#endif // BOSS_H
