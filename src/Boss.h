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
    bool laserActive;
    float laserTimer;
    bool wideBeamActive;
    float wideBeamTimer;
    Rectangle wideBeamRect;
    bool bulletDodgeActive;
    float bulletDodgeTimer;
    float bulletSpawnCooldown;
    float bulletShootingTime;
    std::vector<Bullet_Boss> dodgeBullets;

    // Constructor
    Boss();

    void BossSpawn();
    void BossManager();
    void SelectNextPattern();
    void LaserDiagonalPattern();
    void WideBeamAttack();
    void BulletDodgePattern();
};

#endif // BOSS_H
