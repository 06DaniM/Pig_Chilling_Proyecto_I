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
    bool gotHit;
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
    float patternTimer;   
    float patternCooldown;
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

    Texture2D warningAnimTexture;    
    Rectangle warningAnimFrame;     
    int warningAnimFrameIndex;
    float warningAnimTimer;
    float warningAnimSpeed;     
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

    int bossIdleFrameIndex;
    float bossIdleAnimTimer;
    float bossIdleAnimSpeed;
    Rectangle bossIdleFrameRect;

    int bossDieFrameIndex;
    float bossDieAnimTimer;
    float bossDieAnimSpeed;
    Rectangle bossDieFrameRect;

    int diagonalBallFrameIndex;
    float diagonalBallAnimTimer;
    float diagonalBallAnimSpeed;
    Rectangle diagonalBallFrameRect;
    Rectangle diagonalBallMidFrameRect;

    int wideBeamBallFrameIndex;
    float wideBeamBallAnimTimer;
    float wideBeamBallAnimSpeed;
    Rectangle wideBeamBallFrameRect;

    int dodgeBulletLaserBallFrameIndex;
    float dodgeBulletLaserBallAnimTimer;
    float dodgeBulletLaserBallAnimSpeed;
    Rectangle dodgeBulletLaserBallFrameRect;

    bool bossAttackFromWideBeam;

    Rectangle rectDamage;

    // Constructor
    Boss();

    void BossSpawn();
    void IdleAnimation();
    void DieAnimation();
    void PlayWarningAnimation();
    void LaserDiagonalBallWarningAnimation();
    void WideBeamBallWarningAnimation();
    void DodgeBulletLaserBallWarningAnimation();
    void BossManager();
    void SelectNextPattern();
    void LaserDiagonalPattern();
    void EndLaserDiagonalPattern();
    void WideBeamAttack();
    void EndWideBeamAttack();
    void BulletDodgePattern();
    void EndBulletDodgePattern();
};

#endif // BOSS_H
