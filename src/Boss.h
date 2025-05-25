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
    const float warningDurationWideBeam;
    const float warningDurationLaserDiagonal;
    const float warningDurationBulletDodge;

    Texture2D warningAnimTexture;      // Sprite sheet
    Rectangle warningAnimFrame;        // Frame actual
    int warningAnimFrameIndex;
    float warningAnimTimer;
    float warningAnimSpeed;     // Velocidad entre frames
    bool warningAnimPlaying;
    bool warningAnimFinished;
    bool warningAnimReversing;

    bool diagonalLaserSFXActive;
    bool diagonalLaserSFXPlayed;
    bool wideBeamSFXActive;
    bool wideBeamSFXPlayed;
    bool bulletDodgeSFXActive;
    bool bulletDodgeSFXPlayed;

    bool shieldActive;
    float shieldTimer;
    float shieldDuration;
    Rectangle shieldRect;

    // Constructor
    Boss();

    void BossSpawn();
    void PlayWarningAnimation();
    void BossManager();
    void SelectNextPattern();
    void LaserDiagonalPattern();
    void WideBeamAttack();
    void BulletDodgePattern();
};

#endif // BOSS_H
