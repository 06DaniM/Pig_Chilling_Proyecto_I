/*
﻿MIT License


Copyright(c)[year][fullname]


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :


The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Boss.h"
#include "Enemy.h"
#include "Kraken.h"
#include "Bullet.h"
#include "Timer.h"

using namespace std;

#define BULLET_SPEED 7
#define PLAYER_SPEED 5.0f

enum PowerUpType {
    Double_shot,
    Shield
};

struct PowerUp {
    Rectangle rect;
    bool active;
    PowerUpType type; // Tipo de power-up 
};

Boss boss;
Enemy enemy;
KrakenEnemy krakenEnemy;

Timer menuDelayTimer;
bool isMenuTimerStarted = false;

Timer winDelayTimer;
bool isWinTimerStarted = false;

Timer nextScreenTimer;
bool isNextScreenTimerStarted = false;

Timer deathDelayTimer;
bool isDeathTimerStarted = false;
bool isHitTimerStarted = false;

Timer spawnDelayTimer;
bool isSpawnDelayTimerStarted = false;

Timer invencibilityTimer;
bool isInvencibilityDelayTimerStarted = false;

const int screenWidth = 1152;
const int screenHeight = 896;

int level = 1; // Levels of the game

int maxEnemies = 5; 
int currentEnemies = 0;

Rectangle enemyFrameRec = { 0.0f, 0.0f, (float)enemy.rect.width, (float)enemy.rect.height};

int currentEnemyFrame = 0;
int enemyFramesCounter = 0;

Rectangle enemyDeathFrameRec = { 0.0f, 0.0f, (float)enemy.rect.width, (float)enemy.rect.height };

int currentEnemyDeathFrame = 0;
int enemyDeathFramesCounter = 0;

Rectangle backgroundMenuFrameRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };

int currentBackgrounMenuFrameX = 0;
int currentBackgrounMenuFrameY = 0;
int backgrounMenuFramesCounter = 0;

Rectangle backgroundGameFrameReclevel1 = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };

int currentBackgrounGameFrameXlevel1 = 0;
int currentBackgrounGameFrameYlevel1 = 0;
int backgrounGameFramesCounterlevel1 = 0;

Rectangle backgroundGameFrameReclevel2 = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };

int currentBackgrounGameFrameXlevel2 = 0;
int currentBackgrounGameFrameYlevel2 = 0;
int backgrounGameFramesCounterlevel2 = 0;

Sound shotSound[4] = { 0 };
int currentShotSound;

Sound enemyDestroySound[4] = { 0 };
int currentEnemyDestroySound;

std::vector<Bullet> bullets;
std::vector<Bullet_Enemy> enemyBullets;
std::vector<Bullet_Boss> bossBullets;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Omega Space");
    InitAudioDevice();

    const float spriteWidth = 80;
    const float spriteHeight = 80;
    const float colliderWidth = 30;
    const float colliderHeight = 60;

    Vector2 spritePos = { (screenWidth - spriteWidth) / 2.0f, screenHeight / 1.5f };

    Rectangle player = {
        spritePos.x + (spriteWidth - colliderWidth) / 2.0f,
        spritePos.y + (spriteHeight - colliderHeight) / 2.0f,
        colliderWidth,
        colliderHeight
    };

    Rectangle playerDieFrameRec = { 0,0 , 58, 58 };

    int currentPlayerDieFrame = 0;
    int playerDieFramesCounter = 0;

    std::vector<Enemy> enemies; // Vector to manage the generated enemies 
    std::vector<KrakenEnemy> krakens;
    std::vector<PowerUp> powerUps; // Vector to manage the generated power ups 

    int totalWavesLevel1 = 4; // Number of waves in the level 1
    int totalWavesLevel2 = 5; // Number of waves in the level 2
    float waveTimer = 0.0f; // Time to start the next wave
    float waveDelay = 10.0f; // Seconds between waves
    int currentWave = 0; // Last wave played

    // === SHIP SPRITES === //
    Texture2D shipSpriteBase = LoadTexture("resources/ship/Nave Base.png");
    Texture2D shipSpriteDouble = LoadTexture("resources/ship/NAVE 2DS 64X64.png");
    Texture2D shipSpriteBuble = LoadTexture("resources/ship/Nave Bubble 0.png");
    Texture2D shipSpriteDoubleandBuble = LoadTexture("resources/ship/Nave 2S Bubble 0.png");
    Texture2D shipSpriteDeathAnim = LoadTexture("resources/ship/PlayerExplosion.png");

    // === ENEMIES SPRITES === //
    Texture2D draconoida = LoadTexture("resources/enemies/nave draconoida.png");
    Texture2D draconoidaDeathAnim = LoadTexture("resources/enemies/deathEnemyAnim.png");

    Texture2D mantis = LoadTexture("resources/enemies/nave mantis.png");
    Texture2D mantisDeathAnim = LoadTexture("resources/enemies/deathEnemyAnim.png");

    Texture2D squid = LoadTexture("resources/enemies/nave squid.png");
    Texture2D squidDeathAnim = LoadTexture("resources/enemies/deathEnemyAnim.png");

    Texture2D kraken = LoadTexture("resources/enemies/nave kraken.png");
    Texture2D krakenHit = LoadTexture("resources/enemies/nave kraken hitten.png");
    Texture2D krakenDeathAnim = LoadTexture("resources/enemies/deathEnemyAnim.png");

    // === BOSS SPRITES === //
    Texture2D bossSprite = LoadTexture("resources/enemies/boss.png");
    Texture2D bossAttackNormalSprite = LoadTexture("resources/enemies/bossAttackNormal.png");
    Texture2D bossAttackWideBeamSprite = LoadTexture("resources/enemies/bossAttackWideBeam.png");
    Texture2D bossDeathAnim = LoadTexture("resources/enemies/bossDeath.png");

    // === POWER UPS SPRITES === //
    Texture2D doubleShotSprite = LoadTexture("resources/powerUps/DobleShot_PowerUp.png");
    Texture2D shieldSprite = LoadTexture("resources/powerUps/Shield_PowerUp.png");

    // === BULLETS SPRITES === //
    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D bulletEnemySprite = LoadTexture("resources/bullets/Disparo_Regular_Enemy.png");
    Texture2D bulletBossSprite = LoadTexture("resources/bullets/Disparo_Boss.png");

    Texture2D diagonalMidAttackLaser = LoadTexture("resources/bullets/diagonalMidAttackLaser.png");
    Texture2D diagonalLateralAttackLaser = LoadTexture("resources/bullets/diagonalLateralAttackLaser.png");
    Texture2D diagonalBallLateralLaser = LoadTexture("resources/bullets/diagonalBallLateralLaser.png");
    Texture2D diagonalBallMidLaser = LoadTexture("resources/bullets/diagonalBallMidLaser.png");

    Texture2D wideBeamLaser = LoadTexture("resources/bullets/wideBeamLaser.png");
    Texture2D wideBeamBall = LoadTexture("resources/bullets/wideBeamBall.png");

    Texture2D dodgeBulletLaser = LoadTexture("resources/bullets/dodgeBulletLaser.png");
    Texture2D dodgeBulletLaserBall = LoadTexture("resources/bullets/dodgeBulletLaserBall.png");
    Texture2D Shield_Boss = LoadTexture("resources/bullets/Shield_Boss.png");

    // === MENU SPRITES === //
    Texture2D introBackground = LoadTexture("resources/backgrounds/Intro Background.png");
    Texture2D menuBackground = LoadTexture("resources/backgrounds/Menu Background.png");
    Texture2D gamePlayBackgroundLevel1 = LoadTexture("resources/backgrounds/Gameplay Background Level 1.png");
    Texture2D gamePlayBackgroundLevel2 = LoadTexture("resources/backgrounds/Gameplay Background Level 2.png");

    Texture2D logo = LoadTexture("resources/backgrounds/Logo Game.png");

    Music winMusic = LoadMusicStream("resources/music/Win.wav");
    Music gameOverMusic = LoadMusicStream("resources/music/Death.wav");
    Music menuMusic = LoadMusicStream("resources/music/Main_Menu.wav");
    Music musicLevel1 = LoadMusicStream("resources/music/Level1.wav");
    Music musicLevel2 = LoadMusicStream("resources/music/Level2.wav");
    Music musicBoss = LoadMusicStream("resources/music/Boss.wav");

    Font font = LoadFontEx("resources/font/Data 70 Regular.otf", 64, 0, 0);

    Sound shotSound[4] = { 0 };
    int currentShotSound;

    Sound enemyDestroySound[4] = { 0 };
    int currentEnemyDestroySound;

    Sound deathPlayerSound = LoadSound("resources/soundEffects/fighter_destroyed.mp3");
    Sound laserDiagonalSFX = LoadSound("resources/soundEffects/Diagonal_Attack.wav");
    Sound wideBeamSFX = LoadSound("resources/soundEffects/Wide_Beam_Attack.mp3");
    Sound bulletDodgeSFX = LoadSound("resources/soundEffects/Bullet_Dodge_Attack.mp3");

    shotSound[0] = LoadSound("resources/soundEffects/laser_default.wav");

    for (int i = 1; i < 4; i++)
    {
        shotSound[i] = LoadSoundAlias(shotSound[0]);        // Load an alias of the sound into slots 1-9. These do not own the sound data, but can be played
    }
    currentShotSound = 0;

    enemyDestroySound[0] = LoadSound("resources/soundEffects/galaga_destroyed.mp3");

    for (int i = 1; i < 4; i++)
    {
        enemyDestroySound[i] = LoadSoundAlias(enemyDestroySound[0]);        // Load an alias of the sound into slots 1-9. These do not own the sound data, but can be played
    }
    currentEnemyDestroySound = 0;

    bool doubleShot = false, shield = false, canAct = false, isVisible = true, playerGotHit = false;
    bool canStart = false, canPass = false,pause = false, gameOver = false, hasWon = false, canSpawn = false;
    bool inIntro = true, inMenu = false, inboss = false;
    int score = 0;
    int life = 3;
    float scale = 0.75f; // Reduce the scale of the sprites
    int bosshitframe = 0;

    float shotCooldown = 0.3f;  // Time between shots
    float shotTimer = 0.0f;     // Timer for counting seconds
    bool showGameOver = false;

    SetTargetFPS(60);
    PlayMusicStream(menuMusic);
    PlayMusicStream(musicLevel1);
    PlayMusicStream(musicLevel2);
    PlayMusicStream(musicBoss);
    PlayMusicStream(winMusic);
    PlayMusicStream(gameOverMusic);

    bool winMusicStarted = false;
    float winMusicDuration = 3.7f;
    float winMusicTimer = 0.0f;

    bool gameOverMusicStarted = false;
    float gameOverMusicDuration = 1.8f;
    float gameOverMusicTimer = 0.0f;

    while (!WindowShouldClose())
    {

        // === HACKS FOR TESTING ===
        if (IsKeyPressed(KEY_R))
        {
            doubleShot = !doubleShot;
        }

        if (IsKeyPressed(KEY_F))
        {
            shield = !shield;
        }

        if (IsKeyPressed(KEY_BACKSPACE)) // Change to when life is <= 0
        {
            life -= 1;
            if (life <= 0) gameOver = true;
        }

        if (IsKeyPressed(KEY_ENTER)) // Change to when life is <= 0
        {
            hasWon = true;
        }

        if (IsKeyPressed(KEY_B))
        {
            for (Enemy& enemy : enemies)
            {
                if (enemy.active)
                {
                    enemy.gotHit = true;
                }
            }
            currentEnemies = 0;
            currentWave = 4;
            level = 2;
        }

        if (IsKeyPressed(KEY_K) && inboss) // Change to when life is <= 0
        {
            boss.life = 0;
        }

        if (IsKeyPressed(KEY_M))
        {
            for (Enemy& enemy : enemies)
            {
                if (enemy.active)
                {
                    enemy.gotHit = true;
                    currentEnemies--;
                }
            }
        }

        // === BEGINING GAME CODE

        // Pause the game
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed('P') && !inboss) 
        {
            pause = !pause;
            canAct = !canAct;

            if (pause && level == 1) PauseMusicStream(musicLevel1);
            else if (!pause && level == 1)ResumeMusicStream(musicLevel1);

            if (pause && level == 2 && !inboss) PauseMusicStream(musicLevel2);
            else if (!pause && level == 2 && !inboss)ResumeMusicStream(musicLevel2);

            if (inboss) ResumeMusicStream(musicBoss);
            else if (inboss) PauseMusicStream(musicBoss);
        }

        if (isMenuTimerStarted)
        {
            menuDelayTimer.Update(GetFrameTime());

            if (menuDelayTimer.IsFinished())
            {
                isMenuTimerStarted = false;
                canStart = true;
            }
        }

        // Intro manager
        if (inIntro)
        {
            int textWidth;

            if (!isMenuTimerStarted) {
                menuDelayTimer.Start(2);
                isMenuTimerStarted = true;
            }

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(introBackground, 0, 0, WHITE);

            Vector2 pos1;
            const char* text1 = "PIG CHILLING";
            int fontSize1 = 80;

            Vector2 textSize1 = MeasureTextEx(font, text1, fontSize1, 2);
            pos1.x = (screenWidth / 2.0f) - (textSize1.x / 2.0f);
            pos1.y = 150;

            DrawTextEx(font, text1, pos1, fontSize1, 2, GREEN);

            Vector2 pos2;
            const char* text2 = "DANIEL MARTINEZ ORTEGA - HECTOR TARROJA SANCHEZ - ADAM MOLINA BABA";
            int fontSize2 = 20;

            Vector2 textSize2 = MeasureTextEx(font, text2, fontSize2, 2);
            pos2.x = 20; 
            pos2.y = screenHeight - textSize2.y - 20;

            DrawTextEx(font, text2, pos2, fontSize2, 2, WHITE);

            Vector2 pos3;
            const char* text3 = "TEACHER: ALEIX COTS";
            int fontSize3 = 20;

            Vector2 textSize3 = MeasureTextEx(font, text3, fontSize3, 2);
            pos3.x = screenWidth - textSize3.x - 20;
            pos3.y = screenHeight - textSize3.y - 20; 

            DrawTextEx(font, text3, pos3, fontSize3, 2, WHITE);

            const char* textCenter1 = "Shoot 'em up space game based on Galaga 88/90";
            int fontSizeCenter1 = 40; 
            Vector2 textSizeCenter1 = MeasureTextEx(font, textCenter1, fontSizeCenter1, 2);

            Vector2 posCenter1;
            posCenter1.x = (screenWidth / 2.0f) - (textSizeCenter1.x / 2.0f);
            posCenter1.y = (screenHeight / 2.0f) - (textSizeCenter1.y / 2.0f) + 50;

            DrawTextEx(font, textCenter1, posCenter1, fontSizeCenter1, 2, WHITE);

            const char* textCenter2 = "Project I - Disseny i desenvolupament de videojocs (Barcelona) - CITM";
            int fontSizeCenter2 = 30; 
            Vector2 textSizeCenter2 = MeasureTextEx(font, textCenter2, fontSizeCenter2, 2);

            Vector2 posCenter2;
            posCenter2.x = (screenWidth / 2.0f) - (textSizeCenter2.x / 2.0f);
            posCenter2.y = (screenHeight / 2.0f) - (textSizeCenter2.y / 2.0f) + 125;

            DrawTextEx(font, textCenter2, posCenter2, fontSizeCenter2, 2, WHITE);

            EndDrawing();

            if (GetKeyPressed() != 0 && canStart) // Detect any key
            {
                isMenuTimerStarted = false;
                inIntro = false;
                inMenu = true;
                canStart = false;
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Menu manager
        if (inMenu)
        {
            if (!boss.diagonalLaserSFXActive) StopSound(laserDiagonalSFX);
            if (!boss.wideBeamSFXActive) StopSound(wideBeamSFX);
            if (!boss.bulletDodgeSFXActive) StopSound(bulletDodgeSFX);

            boss.EndLaserDiagonalPattern();
            boss.EndWideBeamAttack();
            boss.EndBulletDodgePattern();

            boss.dodgeBullets.clear();

            if (!isMenuTimerStarted) {
                menuDelayTimer.Start(2); 
                isMenuTimerStarted = true;
            }

            int textWidth;

            backgrounMenuFramesCounter++;

            if (backgrounMenuFramesCounter >= 2)
            {
                backgrounMenuFramesCounter = 0;
                currentBackgrounMenuFrameX++;

                if (currentBackgrounMenuFrameX >= 10)
                {
                    currentBackgrounMenuFrameX = 0;
                    currentBackgrounMenuFrameY++;

                    if (currentBackgrounMenuFrameY >= 10)
                    {
                        currentBackgrounMenuFrameY = 0;
                    }
                }

                backgroundMenuFrameRec.x = currentBackgrounMenuFrameX * screenWidth;
                backgroundMenuFrameRec.y = currentBackgrounMenuFrameY * screenHeight;
            }

            UpdateMusicStream(menuMusic);

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureRec(menuBackground, backgroundMenuFrameRec, { 0,0 }, WHITE);

            textWidth = MeasureText("PUSH RUN", 50); // Measure the length of the text
            DrawTextEx(font, "PUSH RUN", { ((float)screenWidth - textWidth) / 2, 650 }, 50, 2, GREEN); // Center the text

            textWidth = MeasureText("BUTTON", 50); // Measure the length of the text
            DrawTextEx(font, "BUTTON", { ((float)screenWidth - textWidth) / 2, 710 }, 50, 2, GREEN); // Center the text

            textWidth = MeasureText("FROM PIG CHILLING", 60); // Measure the length of the text
            DrawTextEx(font, "FROM PIG CHILLING", { (float)(screenWidth - textWidth) / 2 + 55, 790 }, 60, 2, WHITE); // Center the text

            EndDrawing();

            if (GetKeyPressed() != 0 && canStart) // Detect any key
            {
                enemies.clear();
                // Reset the values

                boss.dead = false;
                life = 3;
                currentWave = 0;
                currentEnemies = 0;

                isMenuTimerStarted = false;

                doubleShot = false;
                shield = false;
                spritePos = { (screenWidth - spriteWidth) / 2.0f, screenHeight / 1.5f };

                canStart = false;
                inMenu = false;
                canAct = true;
                isVisible = true;
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Game manager 
        if (!pause)
        {
            if (level == 1)
            {
                backgrounGameFramesCounterlevel1++;

                if (backgrounGameFramesCounterlevel1 >= 4)
                {
                    backgrounGameFramesCounterlevel1 = 0;
                    currentBackgrounGameFrameXlevel1++;

                    if (currentBackgrounGameFrameXlevel1 >= 10)
                    {
                        currentBackgrounGameFrameXlevel1 = 0;
                        currentBackgrounGameFrameYlevel1++;

                        if (currentBackgrounGameFrameYlevel1 >= 10)
                        {
                            currentBackgrounGameFrameYlevel1 = 0;
                        }
                    }

                    backgroundGameFrameReclevel1.x = currentBackgrounGameFrameXlevel1 * screenWidth;
                    backgroundGameFrameReclevel1.y = currentBackgrounGameFrameYlevel1 * screenHeight;
                }
            }

            else
            {
                backgrounGameFramesCounterlevel2++;

                if (backgrounGameFramesCounterlevel2 >= 4)
                {
                    backgrounGameFramesCounterlevel2 = 0;
                    currentBackgrounGameFrameXlevel2++;

                    if (currentBackgrounGameFrameXlevel2 >= 10)
                    {
                        currentBackgrounGameFrameXlevel2 = 0;
                        currentBackgrounGameFrameYlevel2++;

                        if (currentBackgrounGameFrameYlevel2 >= 10)
                        {
                            currentBackgrounGameFrameYlevel2 = 0;
                        }
                    }

                    backgroundGameFrameReclevel2.x = currentBackgrounGameFrameXlevel2 * screenWidth;
                    backgroundGameFrameReclevel2.y = currentBackgrounGameFrameYlevel2 * screenHeight;
                }
            }

            if (level == 1 && !gameOver && !hasWon) UpdateMusicStream(musicLevel1);
            else if (level == 2 && !inboss && !gameOver && !hasWon) UpdateMusicStream(musicLevel2);
            else if (level == 2 && inboss && !gameOver && !hasWon) UpdateMusicStream(musicBoss);

            // Movimiento del sprite
            if (IsKeyDown(KEY_D) && canAct) spritePos.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_A) && canAct) spritePos.x -= PLAYER_SPEED;

            // Limit the movemente inside the window
            if (spritePos.x < 0) spritePos.x = 0;
            else if (spritePos.x > screenWidth - spriteWidth) spritePos.x = screenWidth - spriteWidth;

            // Actualizar collider centrado en el sprite
            player.x = spritePos.x + (spriteWidth - colliderWidth) / 2.0f;
            player.y = spritePos.y + (spriteHeight - colliderHeight) / 2.0f;


            // Shots with cooldown
            shotTimer += GetFrameTime();
            if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && shotTimer >= shotCooldown && canAct)
            {
                shotTimer = 0.0f;

                // Define the size of the bullets collision
                float bulletWidth = 16;  // Width of the collision
                float bulletHeight = 12; // Height of the collision

                // === FOR THE 2nd ASSIGNMENT

                PlaySound(shotSound[currentShotSound]);            // play the next open sound slot
                currentShotSound++;                                 // increment the sound slot

                if (currentShotSound >= 4) currentShotSound = 0;

                // === FOR THE FINAL GAME
                if (doubleShot)
                {
                    bullets.push_back({
                    {
                        spritePos.x + spriteWidth / 2 - 15 - bulletWidth / 2,
                        spritePos.y + 20,
                        bulletWidth,
                        bulletHeight
                    },
                    true

                    });
                    bullets.push_back({
                    {
                        spritePos.x + spriteWidth / 2 + 15 - bulletWidth / 2,
                        spritePos.y + 20,
                        bulletWidth,
                        bulletHeight
                    },
                    true
                    });
                }

                else
                {
                    bullets.push_back({
                    {
                    spritePos.x + spriteWidth / 2 - bulletWidth / 2,
                    spritePos.y,
                    bulletWidth,
                    bulletHeight
                    },
                    true
                    });
                }
            }

            if (isNextScreenTimerStarted)
            {
                nextScreenTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (nextScreenTimer.IsFinished())
                {
                    isNextScreenTimerStarted = false;
                    canPass = true;  // Solo se marca como ganado al terminar el temporizador
                }
            }

            if (isWinTimerStarted)
            {
                winDelayTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (winDelayTimer.IsFinished())
                {
                    isWinTimerStarted = false;
                    hasWon = true;  // Solo se marca como ganado al terminar el temporizador
                }
            }

            if (isSpawnDelayTimerStarted)
            {
                spawnDelayTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (spawnDelayTimer.IsFinished())
                {
                    canSpawn = true;  // Solo se marca como ganado al terminar el temporizador
                    isSpawnDelayTimerStarted = false;
                }
            }

            if (bosshitframe > 0) bosshitframe++;

            // Update the bullets
            for (Bullet& bullet : bullets)
            {
                if (bullet.active)
                {
                    bullet.rect.y -= BULLET_SPEED;

                    for (Enemy& enemy : enemies)
                    {
                        if (enemy.active)
                        {
                            if (CheckCollisionRecs(bullet.rect, enemy.rect) && !enemy.gotHit) // If collision with an enemy
                            {
                                PlaySound(enemyDestroySound[currentEnemyDestroySound]);            // play the next open sound slot
                                currentEnemyDestroySound++;                                 // increment the sound slot

                                if (currentEnemyDestroySound >= 4) currentEnemyDestroySound = 0;

                                bullet.active = false;

                                if (enemy.enemyClass == Kraken && !enemy.krakenHit) enemy.krakenHit = true;
                                
                                else
                                {
                                    enemy.gotHit = true;

                                    if (enemy.enemyClass == Draconoida) score += 100;
                                    else if (enemy.enemyClass == Mantis) score += 200;
                                    else if (enemy.enemyClass == Squid) score += 800;
                                    else if (enemy.enemyClass == Kraken) score += 500;

                                    currentEnemies--;

                                    // 20% to generate the item/object
                                    if (GetRandomValue(1, 100) < 5) // 20% de probabilidad de generar un power-up
                                    {
                                        // Lista de power-ups disponibles según los estados actuales y los que ya están en pantalla
                                        std::vector<PowerUpType> availablePowerUps;

                                        PowerUp newPowerUp;

                                        bool doubleShotOnScreen = false;
                                        bool shieldOnScreen = false;

                                        // Verificar si ya hay un power-up de cada tipo en pantalla
                                        for (const auto& powerUp : powerUps)
                                        {
                                            if (powerUp.type == Double_shot) doubleShotOnScreen = true;
                                            if (powerUp.type == Shield) shieldOnScreen = true;
                                        }

                                        // Solo añadir si el power-up no está activo ni en pantalla
                                        if (!doubleShot && !doubleShotOnScreen) availablePowerUps.push_back(Double_shot);
                                        if (!shield && !shieldOnScreen) availablePowerUps.push_back(Shield);

                                        // Solo generamos un power-up si hay disponibles
                                        if (!availablePowerUps.empty())
                                        {
                                            newPowerUp.rect = { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 20, 20 };

                                            // Convertir size_t a int de forma segura
                                            int maxIndex = static_cast<int>(availablePowerUps.size()) - 1;
                                            newPowerUp.type = availablePowerUps[GetRandomValue(0, maxIndex)];

                                            powerUps.push_back(newPowerUp);
                                        }
                                        newPowerUp.active = true;
                                    }
                                }

                                break;
                            }
                        }
                    }

                    if (boss.active)
                    {
                        if (boss.bulletDodgeSFXActive && CheckCollisionRecs(boss.shieldRect, bullet.rect))
                        {
                            bullet.active = false;
                        }

                        else if (CheckCollisionRecs(boss.rectDamage, bullet.rect))
                        {
                            bullet.active = false;
                            boss.gotHit = true;
                            bosshitframe++;
                            boss.life--;
                            if (boss.life <= 0) score += 5000;
                            cout << boss.life;
                        }
                    }
                }
            }

            // Delete bullets
            if (life <= 0) bullets.clear();
            else bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                    [](const Bullet& b) { return !b.active || b.rect.y < 0; }), bullets.end());

            if (playerGotHit)
            {
                if (isVisible)
                {
                    // Actualizar frame rect
                    playerDieFrameRec.x = currentPlayerDieFrame * 58;

                    // Solo avanzar si no terminamos la animación
                    if (currentPlayerDieFrame < 4)
                    {
                        playerDieFramesCounter++;

                        if (playerDieFramesCounter >= 5)
                        {
                            playerDieFramesCounter = 0;
                            currentPlayerDieFrame++;

                            // Si la animación termina, iniciar delay y ocultar sprite
                            if (currentPlayerDieFrame >= 4 && !gameOver && !isHitTimerStarted)
                            {
                                currentPlayerDieFrame = 0;
                                deathDelayTimer.Start(1.0f);
                                isDeathTimerStarted = true;
                                isVisible = false;
                            }
                        }
                    }
                }
            }

            // Delete the inactive bullets
            enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(),
                [](const Bullet_Enemy& b) { return !b.active || b.rect.y < 0; }), enemyBullets.end());

            if (level == 1)
            {
                // === WAVES DELAY ===
                if (currentWave < totalWavesLevel1 && currentEnemies == 0 && currentWave == 0 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel1 && currentEnemies <= 7 && currentWave == 1 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel1 && currentEnemies == 0 && currentWave == 2 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(0.5f);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel1 && currentEnemies <= 3 && currentWave == 3 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }
            }

            else if (level == 2)
            {
                // === WAVES DELAY ===
                if (currentWave < totalWavesLevel2 && currentEnemies == 0 && currentWave == 0 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel2 && currentEnemies <= 7 && currentWave == 1 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel2 && currentEnemies == 0 && currentWave == 2 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(0.5f);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel2 && currentEnemies <= 3 && currentWave == 3 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }

                else if (currentWave < totalWavesLevel2 && currentEnemies == 0 && currentWave == 4 && !isSpawnDelayTimerStarted)
                {
                    spawnDelayTimer.Start(1);
                    isSpawnDelayTimerStarted = true;
                }
            }

            // === ENEMY PRESETS ===
            // 1: Draconoide, Mantis, Draconoide, Mantis, Draconoide
            // 2: Draconoide, Mantis, Mantis, Mantis, Draconoide
            // 3: Squid, Draconoide, Mantis, Draconoide, Squid
            // 4: Kraken, Kraken, Squid, Kraken, Kraken
            // 5: Kraken, Draconoide, Mantis, Draconoide, Mantis
            // 6: Squid, Mantis, Mantis, Mantis, Squid

            // === WAVES MANAGER ===
            if (canSpawn)
            {
                if (level == 1)
                {
                    // === Wave 1 ===
                    if (currentEnemies == 0 && currentWave == 0)
                    {
                        currentEnemies += 5;
                        enemy.SpawnEnemies(enemies, maxEnemies, 60.0f, -100.0f, 1, -1, 3.5f, screenWidth / 1.5f, screenHeight / 2.5f, currentEnemies, 1); // Left
                        currentWave++;
                    }

                    // === Wave 2 ===
                    else if (currentEnemies <= 2 && currentWave == 1)
                    {
                        currentEnemies += 10;
                        enemy.SpawnEnemies(enemies, maxEnemies, 120.0f, -100.0f, -1, -1, 3.5f, screenWidth / 2.0f - 80, screenHeight / 2.0f + 50, currentEnemies, 2); // Left
                        enemy.SpawnEnemies(enemies, maxEnemies, 180.0f, screenWidth + 100.0f, 1, -1, 3.5f, screenWidth / 2.0f + 80, screenHeight / 2.0f + 50, currentEnemies, 2); // Right
                        currentWave++;
                    }

                    // === Wave 3 ===
                    else if (currentEnemies == 0 && currentWave == 2)
                    {
                        currentEnemies += 5;
                        enemy.SpawnEnemies(enemies, maxEnemies, 120.0f, screenWidth + 100, 1, 1, 2.5f, screenWidth / 2.0f, screenHeight / 1.8f, currentEnemies, 1); // Right
                        currentWave++;
                    }

                    // === Wave 4 ===
                    else if (currentEnemies <= 3 && currentWave == 3)
                    {
                        currentEnemies += 15;
                        enemy.SpawnEnemies(enemies, maxEnemies, 180.0f, screenWidth + 100, -1, 1, 2.5f, screenWidth / 2 + 40, screenHeight / 3.5f, currentEnemies, 3); // Right
                        enemy.SpawnEnemies(enemies, maxEnemies, 240.0f, -100.0f, -1, -1, 2.5f, screenWidth / 2 - 100, screenHeight / 2.3f, currentEnemies, 1); // Left
                        enemy.SpawnEnemies(enemies, maxEnemies, 300.0f, screenWidth + 100, 1, 1, 2.5f, screenWidth / 2 + 100, screenHeight / 2.3f, currentEnemies, 2); // Right
                        currentWave++;
                    }
                }
                
                else if (level == 2)
                {
                    // === Wave 1 ===
                    if (currentEnemies == 0 && currentWave == 0)
                    {
                        currentEnemies += 5;
                        enemy.SpawnEnemies(enemies, maxEnemies, 140.0f, -100.0f, 1, -1, 3.5f, screenWidth / 1.5f, screenHeight / 2.5f, currentEnemies, 4); // Left
                        currentWave++;
                    }

                    // === Wave 2 ===
                    else if (currentEnemies == 0 && currentWave == 1)
                    {
                        currentEnemies += 10;
                        enemy.SpawnEnemies(enemies, maxEnemies, 200.0, -100.0f, -1, -1, 3.5f, screenWidth / 2.0f - 80, screenHeight / 2.0f + 50, currentEnemies, 5); // Left
                        enemy.SpawnEnemies(enemies, maxEnemies, 260.0f, screenWidth + 100.0f, 1, -1, 3.5f, screenWidth / 2.0f + 80, screenHeight / 2.0f + 50, currentEnemies, 3); // Right
                        currentWave++;
                    }

                    // === Wave 3 ===
                    else if (currentEnemies == 0 && currentWave == 2)
                    {
                        currentEnemies += 10;
                        enemy.SpawnEnemies(enemies, maxEnemies, 140.0f, screenWidth + 100, 1, 1, 2.5f, screenWidth / 2.0f, screenHeight / 1.8f, currentEnemies, 6); // Left
                        enemy.SpawnEnemies(enemies, maxEnemies, 200.0, -100.0f, -1, -1, 3.5f, screenWidth / 2.0f - 80, screenHeight / 2.0f + 50, currentEnemies, 5); // Left
                        currentWave++;
                    }

                    // === Wave 4 ===
                    else if (currentEnemies == 0 && currentWave == 3)
                    {
                        currentEnemies += 15;
                        enemy.SpawnEnemies(enemies, maxEnemies, 140.0f, -100.0f, -1, -1, 2.5f, screenWidth / 2 - 100, screenHeight / 2.3f, currentEnemies, 5); // Left
                        enemy.SpawnEnemies(enemies, maxEnemies, 200.0f, screenWidth + 100, 1, 1, 2.5f, screenWidth / 2 + 100, screenHeight / 2.3f, currentEnemies, 3); // Right
                        enemy.SpawnEnemies(enemies, maxEnemies, 260.0f, screenWidth + 100, -1, 1, 2.5f, screenWidth / 2 + 40, screenHeight / 3.5f, currentEnemies, 5); // Right
                        currentWave++;
                    }

                    // === BOSS ===
                    else if (currentEnemies == 0 && currentWave == 4)
                    {
                        inboss = true;
                        boss.BossSpawn();
                        currentWave++;
                    }
                }
                canSpawn = false;
            }

            // === Ends the first screen after destroying all enemies ===
            else if (currentEnemies == 0)
            {
                if (currentWave >= totalWavesLevel1 && level == 1)
                {
                    currentWave = 0;
                    level++;
                }

                if (!hasWon && !isWinTimerStarted && currentWave >= totalWavesLevel2 && level == 2 && boss.dead)
                {
                    winDelayTimer.Start(1.5f); // Wait 1 second for the winning screen
                    isWinTimerStarted = true;
                }
            }

            enemyFramesCounter++;

            if (enemyFramesCounter >= (30))
            {
                enemyFramesCounter = 0;
                currentEnemyFrame++;

                if (currentEnemyFrame > 1) currentEnemyFrame = 0;

                enemyFrameRec.x = (float)currentEnemyFrame * (float)enemy.rect.width;
            }
            enemy.UpdateEnemyOffsetNormal(GetFrameTime());
            enemy.UpdateEnemyOffset(GetFrameTime());

            // Update enemies
            for (Enemy& enemy : enemies)
            {
                if (CheckCollisionRecs(player, enemy.rect) && !shield && !playerGotHit && !enemy.picked && isVisible && !isInvencibilityDelayTimerStarted && enemy.active && !enemy.gotHit)
                {
                    PlaySound(deathPlayerSound); 
                    PlaySound(enemyDestroySound[currentEnemyDestroySound]);
                    currentEnemyDestroySound++;

                    if (currentEnemyDestroySound >= 4) currentEnemyDestroySound = 0;

                    playerGotHit = true;
                    enemy.gotHit = true;
                    canAct = false;
                    life--;
                    currentEnemies--;

                    break;
                }
                
                else if (CheckCollisionRecs(player, enemy.rect) && shield && !enemy.gotHit && isVisible)
                {
                    PlaySound(enemyDestroySound[currentEnemyDestroySound]);
                    currentEnemyDestroySound++;

                    if (currentEnemyDestroySound >= 4) currentEnemyDestroySound = 0;

                    shield = false;
                    enemy.gotHit = true;
                    currentEnemies--;
                    break;
                }

                if (enemy.picked && enemy.active)
                {
                    canAct = false;
                    shield = false;
                }
                else if (!canAct) krakenEnemy.playerPicked = true;

                enemy.UpdateEnemy(enemyBullets, enemies, enemy, GetFrameTime(), player, spritePos, gameOver, isInvencibilityDelayTimerStarted);
            }

            // Update enemies bullets
            for (Bullet_Enemy& bullet : enemyBullets)
            {
                if (bullet.active)
                {
                    bullet.rect.y += BULLET_SPEED;
                    if (CheckCollisionRecs(player, bullet.rect) && !shield && canAct && isVisible && !isInvencibilityDelayTimerStarted)
                    {
                        PlaySound(deathPlayerSound);
                        playerGotHit = true;
                        canAct = false;
                        bullet.active = false;
                        life--;

                        break;
                    }

                    else if (CheckCollisionRecs(player, bullet.rect) && shield && isVisible)
                    {
                        shield = false;
                        bullet.active = false;
                        break;
                    }
                }
            }

            if (boss.wideBeamDamageActive && CheckCollisionRecs(player, boss.wideBeamRect) && canAct && isVisible && !isInvencibilityDelayTimerStarted)
            {
                PlaySound(deathPlayerSound);
                playerGotHit = true;
                canAct = false;
                shield = false;
                life--;
            }

            else if ((CheckCollisionRecs(player, boss.leftRayRect) || CheckCollisionRecs(player, boss.rightRayRect)) && boss.bulletLaserDamageActive && canAct && isVisible && !isInvencibilityDelayTimerStarted)
            {
                PlaySound(deathPlayerSound);
                playerGotHit = true;
                canAct = false;
                shield = false;
                life--;
            }

            else if ((CheckCollisionRecs(player, boss.centerLaserRect) || CheckCollisionRecs(player, boss.leftDiagonalRect) || CheckCollisionRecs(player, boss.rightDiagonalRect)) && boss.laserDamageActive && canAct && isVisible && !isInvencibilityDelayTimerStarted)
            {
                PlaySound(deathPlayerSound);
                playerGotHit = true;
                canAct = false;
                shield = false;
                life--;
            }

            for (Bullet_Boss& bullet : boss.dodgeBullets)
            {
                if (CheckCollisionRecs(player, bullet.rect) && canAct && isVisible && !shield && !isInvencibilityDelayTimerStarted)
                {
                    PlaySound(deathPlayerSound);
                    playerGotHit = true;
                    bullet.active = false;
                    canAct = false;
                    life--;
                }

                else if (CheckCollisionRecs(player, bullet.rect) && isVisible && shield)
                {
                    shield = false;
                    bullet.active = false;
                }
            }

            // Update Boss
            if (boss.active && !boss.dying && !gameOver)
            {
                boss.BossManager();
                if (boss.life <= 0)
                {
                    boss.dying = true;
                    bosshitframe = 0;
                }

                if (!boss.diagonalLaserSFXActive) StopSound(laserDiagonalSFX);
                if (!boss.wideBeamSFXActive) StopSound(wideBeamSFX);
                if (!boss.bulletDodgeSFXActive) StopSound(bulletDodgeSFX);
            }

            if (boss.dying)
            {
                if (!boss.diagonalLaserSFXActive) StopSound(laserDiagonalSFX);
                if (!boss.wideBeamSFXActive) StopSound(wideBeamSFX);
                if (!boss.bulletDodgeSFXActive) StopSound(bulletDodgeSFX);

                boss.laserDamageActive = false;

                boss.EndLaserDiagonalPattern();
                boss.EndWideBeamAttack();
                boss.EndBulletDodgePattern();

                boss.dodgeBullets.clear();
            }

            if (boss.dead) canAct = false;

            if (boss.diagonalLaserSFXActive && !boss.diagonalLaserSFXPlayed)
            {
                PlaySound(laserDiagonalSFX);
                boss.diagonalLaserSFXPlayed = true;
            }

            else if (boss.wideBeamSFXActive && !boss.wideBeamSFXPlayed)
            {
                PlaySound(wideBeamSFX);
                boss.wideBeamSFXPlayed = true;
            }

            else if (boss.bulletDodgeSFXActive && !boss.bulletDodgeSFXPlayed)
            {
                PlaySound(bulletDodgeSFX);
                boss.bulletDodgeSFXPlayed = true;
            }

            if (isInvencibilityDelayTimerStarted)
            {
                invencibilityTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (invencibilityTimer.IsFinished())
                {
                    isInvencibilityDelayTimerStarted = false;
                }
            }

            if (isDeathTimerStarted)
            {
                deathDelayTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (deathDelayTimer.IsFinished())
                {
                    if (life <= 0) gameOver = true;  // Solo se marca como ganado al terminar el temporizador
                    else
                    {
                        spritePos.x = (screenWidth - spriteWidth) / 2.0f;
                        spritePos.y = screenHeight / 1.5f;
                        isVisible = true;
                        canAct = true;
                    }

                    invencibilityTimer.Start(2);
                    isInvencibilityDelayTimerStarted = true;

                    isDeathTimerStarted = false;
                    playerGotHit = false;

                    playerDieFramesCounter = 0;
                    currentPlayerDieFrame = 0;
                }
            }

            for (PowerUp& powerUp : powerUps)
            {
                if (powerUp.active)
                {
                    powerUp.rect.y += 2; // Velocity to down

                    // Activate the double shot if collsion between the ship and the item
                    if (CheckCollisionRecs(player, powerUp.rect))
                    {
                        if (powerUp.type == Double_shot)
                        {
                            doubleShot = true; // Activate double shoot power-up 
                        }

                        else if (powerUp.type == Shield)
                        {
                            shield = true; // Activate shield power-up
                        }

                        powerUp.active = false; // Desactivate the power up // Innecesario, creo
                    }
                }
            }

            // Eliminar power-ups inactivos
            powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
                [](const PowerUp& p) { return !p.active || p.rect.y > screenHeight; }), powerUps.end());
        }

        // Draw all the scene
        BeginDrawing();
        ClearBackground(BLACK);
        if (level == 1) DrawTextureRec(gamePlayBackgroundLevel1, backgroundGameFrameReclevel1, { 0, 0 }, WHITE);
        else DrawTextureRec(gamePlayBackgroundLevel2, backgroundGameFrameReclevel2, { 0, 0 }, WHITE);

        // Draw the bullets
        for (const Bullet& bullet : bullets)
        {
            if (bullet.active)
            {
                // Draw the bullet
                DrawTextureEx(bulletSprite,
                    { (bullet.rect.x + bullet.rect.width / 2 - bulletSprite.width / 2),
                    (bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2) }, 0, 1,
                    WHITE);
            }
        }

        // Draw the enemy bullets
        for (const Bullet_Enemy& bullet : enemyBullets)
        {
            if (bullet.active)
            {
                DrawTextureEx(bulletEnemySprite,
                    { (bullet.rect.x + bullet.rect.width / 2 - bulletSprite.width / 2),
                    (bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2) }, 0, 1,
                    WHITE);
            }
        }

        // Draw the ship
        if (isVisible)
        {
            if (playerGotHit)
            {
                Rectangle dest = {
                spritePos.x + spriteWidth / 2.0f,
                spritePos.y + spriteHeight / 2.0f,
                spriteWidth * 2,  // escalar al doble si quieres
                spriteHeight * 2
                };

                Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                DrawTexturePro(shipSpriteDeathAnim, playerDieFrameRec, dest, origin, 0, WHITE);
            }

            else if (doubleShot && !shield) DrawTextureEx(shipSpriteDouble, spritePos, 0, 1, isInvencibilityDelayTimerStarted ? Color{ 255, 255, 255, 120 } : WHITE);

            else if (shield && !doubleShot) DrawTexture(shipSpriteBuble, (int)spritePos.x, (int)spritePos.y, WHITE);

            else if (doubleShot && shield) DrawTexture(shipSpriteDoubleandBuble, (int)spritePos.x, (int)spritePos.y, WHITE);

            else DrawTextureEx(shipSpriteBase, spritePos, 0, 1, isInvencibilityDelayTimerStarted ? Color{ 255, 255, 255, 120 } : WHITE);

            DrawRectangleLinesEx(player, 2, GREEN);
        }

        // Draw the enemies
        for (const Enemy& enemy : enemies)
        {
            if (enemy.active)
            {
                float scale = 0.9f;

                Rectangle source = enemyFrameRec;

                Rectangle dest = {
                    enemy.rect.x + enemy.rect.width / 2.0f,
                    enemy.rect.y + enemy.rect.height / 2.0f,
                    enemy.rect.width * scale,
                    enemy.rect.height * scale
                };

                Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                if (!enemy.gotHit)
                {
                    if (enemy.enemyClass == Draconoida) DrawTexturePro(draconoida, source, dest, origin, enemy.rotation, WHITE);
                    else if (enemy.enemyClass == Mantis) DrawTexturePro(mantis, source, dest, origin, enemy.rotation, WHITE);
                    else if (enemy.enemyClass == Squid) DrawTexturePro(squid, source, dest, origin, enemy.rotation, WHITE);
                    else if (enemy.enemyClass == Kraken && !enemy.krakenHit) DrawTexturePro(kraken, source, dest, origin, enemy.rotation, WHITE);
                    else DrawTexturePro(krakenHit, source, dest, origin, enemy.rotation, WHITE);
                }

                else
                {
                    float scale = 1.8f;

                    Rectangle source = enemy.enemyDeathFrameRec;

                    Rectangle dest = {
                        enemy.rect.x + 54 / 2.0f,
                        enemy.rect.y + 54 / 2.0f,
                        54 * scale,
                        54 * scale
                    };

                    Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                    if(enemy.enemyClass == Draconoida) DrawTexturePro(draconoidaDeathAnim, source, dest, origin, 0, WHITE);
                    else if (enemy.enemyClass == Mantis) DrawTexturePro(mantisDeathAnim, source, dest, origin, 0, WHITE);
                    else if (enemy.enemyClass == Squid) DrawTexturePro(squidDeathAnim, source, dest, origin, 0, WHITE);
                    else DrawTexturePro(krakenDeathAnim, source, dest, origin, 0, WHITE);
                }
            }
        }

        // Draw the boss
        if (!boss.dead && boss.active)
        {
            if (!boss.dying && boss.life > 0)
            {
                // Si está mostrando la animación de advertencia, dibujarla
                if (boss.currentPattern != ATTACK_WIDE_BEAM && (boss.warningAnimPlaying || boss.warningAnimFinished) && !boss.bossAttackFromWideBeam)
                {
                    if (bosshitframe == 0) DrawTextureRec(bossAttackNormalSprite, boss.warningAnimFrame, { boss.rect.x, boss.rect.y }, WHITE);
                    else if (bosshitframe > 0) DrawTextureRec(bossAttackNormalSprite, boss.warningAnimFrame, { boss.rect.x, boss.rect.y }, RED);
                }

                else if (boss.currentPattern == ATTACK_WIDE_BEAM && (boss.warningAnimPlaying || boss.warningAnimFinished) && boss.bossAttackFromWideBeam)
                {
                    if (bosshitframe == 0) DrawTextureRec(bossAttackWideBeamSprite, boss.warningAnimFrame, { boss.rect.x, boss.rect.y }, WHITE);
                    else if (bosshitframe > 0) DrawTextureRec(bossAttackWideBeamSprite, boss.warningAnimFrame, { boss.rect.x, boss.rect.y }, RED);
                }

                else if (bosshitframe > 0 && boss.currentPattern == ATTACK_NONE)
                {
                    DrawTextureRec(bossSprite, boss.bossIdleFrameRect, { boss.rect.x, boss.rect.y }, RED);
                    boss.gotHit = false;
                }

                else
                {
                    DrawTextureRec(bossSprite, boss.bossIdleFrameRect, { boss.rect.x, boss.rect.y }, WHITE);
                }

                if (boss.shieldActive)
                {
                    DrawTexture(Shield_Boss, boss.shieldRect.x, boss.shieldRect.y, WHITE); // borde azul
                }

                if (bosshitframe > 5) bosshitframe = 0;

                // Dibujar ataques del jefe DESPUÉS del jefe (así se ven encima)
                if (boss.active)
                {
                    switch (boss.currentPattern)
                    {
                    case ATTACK_LASER_DIAGONAL:
                        boss.LaserDiagonalPattern();
                        break;
                    case ATTACK_WIDE_BEAM:
                        boss.WideBeamAttack();
                        break;
                    case ATTACK_BULLET_DODGE:
                        boss.BulletDodgePattern();
                        break;
                    }
                }

                if (boss.laserDamageActive)
                {
                    Vector2 origin = { boss.rect.x + boss.rect.width / 2, boss.rect.y + boss.rect.height };

                    // Láser central 
                    DrawTextureEx(diagonalMidAttackLaser, { origin.x - 50, origin.y - 50 }, 0, 1, WHITE);

                    // --------------------
                    // LÁSER IZQUIERDO
                    // --------------------

                    Vector2 startLeft = { origin.x - 190, origin.y };
                    Vector2 endLeft = { origin.x + 950, origin.y + 800 };

                    Vector2 diffLeft = { endLeft.x - startLeft.x, endLeft.y - startLeft.y };
                    float lengthLeft = sqrtf(diffLeft.x * diffLeft.x + diffLeft.y * diffLeft.y);
                    float angleLeft = atan2f(diffLeft.y, diffLeft.x) * RAD2DEG;

                    Vector2 centerLeft = {
                        (startLeft.x + endLeft.x) / 2.0f,
                        (startLeft.y + endLeft.y) / 2.0f
                    };

                    Rectangle sourceRec = {
                        0, 0,
                        (float)diagonalLateralAttackLaser.width,
                        (float)diagonalLateralAttackLaser.height
                    };

                    Rectangle destLeft = {
                        centerLeft.x, centerLeft.y,
                        1100, 50
                    };

                    Vector2 originLeft = { lengthLeft / 2, 25 };

                    DrawTexturePro(diagonalLateralAttackLaser, sourceRec, destLeft, originLeft, angleLeft, WHITE);

                    // --------------------
                    // LÁSER DERECHO
                    // --------------------

                    Vector2 startRight = { origin.x + 190, origin.y };
                    Vector2 endRight = { origin.x - 950, origin.y + 800 };

                    Vector2 diffRight = { endRight.x - startRight.x, endRight.y - startRight.y };
                    float lengthRight = sqrtf(diffRight.x * diffRight.x + diffRight.y * diffRight.y);
                    float angleRight = atan2f(diffRight.y, diffRight.x) * RAD2DEG;

                    Vector2 centerRight = {
                        (startRight.x + endRight.x) / 2.0f,
                        (startRight.y + endRight.y) / 2.0f
                    };

                    Rectangle destRight = {
                        centerRight.x, centerRight.y,
                        1100, 50
                    };

                    Vector2 originRight = { lengthRight / 2, 50 / 2 };

                    DrawTexturePro(diagonalLateralAttackLaser, sourceRec, destRight, originRight, angleRight, WHITE);
                }

                if (boss.warningAnimFinished && boss.currentPattern == ATTACK_LASER_DIAGONAL)
                {
                    boss.LaserDiagonalBallWarningAnimation(); 

                    Vector2 origin = { boss.rect.x + boss.rect.width / 2, boss.rect.y + boss.rect.height };
                    Vector2 positions[2] = {
                        { origin.x - 190, origin.y },
                        { origin.x + 190, origin.y }
                    };
                    Vector2 center = { origin.x, origin.y };

                    for (int i = 0; i < 2; i++)
                    {
                        Vector2 drawPos = {
                            positions[i].x - boss.diagonalBallFrameRect.width / 2,
                            positions[i].y - boss.diagonalBallFrameRect.height / 2
                        };

                        DrawTextureRec(diagonalBallLateralLaser, boss.diagonalBallFrameRect, drawPos, WHITE);
                    }

                    Vector2 drawPos = {
                        center.x - boss.diagonalBallMidFrameRect.width / 2,
                        center.y - boss.diagonalBallMidFrameRect.height / 2
                    };

                    DrawTextureRec(diagonalBallMidLaser, boss.diagonalBallMidFrameRect, drawPos, WHITE);
                }

                if (boss.wideBeamActive && boss.currentPattern == ATTACK_WIDE_BEAM) // Cambiar wideBeamActive por la condición que sea cuando la animación esté hecha
                {
                    boss.WideBeamBallWarningAnimation();

                    Vector2 wideBeamPos = {
                        boss.rect.x + boss.rect.width / 2 - boss.wideBeamBallFrameRect.width / 2,
                        boss.rect.y + boss.rect.height - boss.wideBeamBallFrameRect.height / 2
                    };

                    DrawTextureRec(wideBeamBall, boss.wideBeamBallFrameRect, wideBeamPos, WHITE);

                    if (boss.wideBeamDamageActive) DrawTexture(wideBeamLaser, boss.wideBeamRect.x, boss.wideBeamRect.y, WHITE);
                }

                if (boss.warningStarted && boss.currentPattern == ATTACK_BULLET_DODGE)
                {
                    boss.DodgeBulletLaserBallWarningAnimation(); // <-- actualiza los frames

                    Vector2 origin = { boss.rect.x + boss.rect.width / 2, boss.rect.y + boss.rect.height };
                    Vector2 positions[2] = {
                        { boss.rect.x - 200.0f, boss.rect.y + boss.rect.height },
                        { boss.rect.x + boss.rect.width + 200.0f, boss.rect.y + boss.rect.height }
                    };

                    Vector2 center = { origin.x, origin.y };

                    for (int i = 0; i < 2; i++)
                    {
                        Vector2 drawPos = {
                            positions[i].x - boss.dodgeBulletLaserBallFrameRect.width / 2,
                            positions[i].y - boss.dodgeBulletLaserBallFrameRect.height / 2
                        };

                        DrawTextureRec(dodgeBulletLaserBall, boss.dodgeBulletLaserBallFrameRect, drawPos, WHITE);
                    }

                    if (boss.bulletLaserDamageActive)
                    {
                        DrawTexture(dodgeBulletLaser, boss.leftRayRect.x, boss.leftRayRect.y, WHITE);
                        DrawTexture(dodgeBulletLaser, boss.rightRayRect.x, boss.rightRayRect.y, WHITE);
                    }
                }
            }

            else
            {
                if (boss.bossDieFrameIndex < 8)
                {
                    boss.DieAnimation();
                    DrawTextureRec(bossDeathAnim, boss.bossDieFrameRect, { boss.rect.x, boss.rect.y }, WHITE);
                }
                else
                {
                    boss.active = false;
                    boss.dead = true;
                }
            }
        }

        // Draw the boss bullets
        for (const Bullet_Boss& bullet : boss.dodgeBullets)
        {
            if (bullet.active)
            {
                DrawTextureEx(bulletBossSprite,
                    { bullet.pos.x - bulletBossSprite.width / 2,
                      bullet.pos.y - bulletBossSprite.height / 2 },
                    0, 0.8f, WHITE);
            }
        }

        // Draw the lifes of the ship
        for (int i = 0; i < life; i++) {
            Vector2 position = { 25 + i * (shipSpriteBase.width * scale + 10), screenHeight - shipSpriteBase.height * scale - 25 };
            DrawTextureEx(shipSpriteBase, position, 0.0f, scale, WHITE);
        }

        // Display the score
        DrawTextEx(font, TextFormat("SCORE: %i", score), { 10, 10 }, 34, 2, WHITE);

        // Draw pause
        if (pause)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0,0,0,125 });
            DrawTextEx(font, "PAUSE", { (screenWidth - 100) / 2, (screenHeight - 50) / 2 }, 50, 2, WHITE);
        }

        // Game Over manager
        if (gameOver)
        {
            if (!isNextScreenTimerStarted) {
                nextScreenTimer.Start(2);        // Inicia el temporizador solo una vez
                isNextScreenTimerStarted = true;
            }

            if (boss.diagonalLaserSFXActive) StopSound(laserDiagonalSFX);
            if (boss.wideBeamSFXActive) StopSound(wideBeamSFX);
            if (boss.bulletDodgeSFXActive) StopSound(bulletDodgeSFX);

            if (!gameOverMusicStarted)
            {
                PlayMusicStream(gameOverMusic);
                gameOverMusicStarted = true;
                gameOverMusicTimer = 0.0f;
            }

            gameOverMusicTimer += GetFrameTime();
            if (gameOverMusicTimer < gameOverMusicDuration)
            {
                UpdateMusicStream(gameOverMusic);
            }

            inboss = false;

            // Show the GAME OVER screen
            BeginDrawing();
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 120 });

            int gameOverWidth = MeasureText("GAME OVER", 40);
            DrawTextEx(font, "GAME OVER", { (float)(screenWidth - gameOverWidth) / 2, screenHeight / 2 }, 40, 2, RED);

            int retryWidth = MeasureText("Press any key to return to the menu", 30);
            DrawTextEx(font, "Press any key to return to the menu", { (float)(screenWidth - retryWidth) / 2 + 50, screenHeight / 2 + 50 }, 30, 2, WHITE);

            EndDrawing();

            canAct = false;

            isNextScreenTimerStarted = true;

            if (GetKeyPressed() != 0 && canPass) // Detect any key
            {
                // Restart the game
                isNextScreenTimerStarted = false;
                isInvencibilityDelayTimerStarted = false;
                inMenu = true;
                gameOver = false;

                boss.EndLaserDiagonalPattern();
                boss.EndWideBeamAttack();
                boss.EndBulletDodgePattern();

                boss.active = false;

                // Restore to its initial state
                player.x = (screenWidth - player.width) / 2.0f;
                player.y = screenHeight / 1.5f;
                bullets.clear();
                score = 0;
                level = 1;
                currentWave = 0;
                currentEnemies = 0;
            }
            continue; // Avoid the code is still executing in the menu
        }

        else gameOverMusicStarted = false;

        if (hasWon)
        {
            if (!isNextScreenTimerStarted) {
                nextScreenTimer.Start(4);        // Inicia el temporizador solo una vez
                isNextScreenTimerStarted = true;
            }

            if (!winMusicStarted)
            {
                PlayMusicStream(gameOverMusic);
                winMusicStarted = true;
                winMusicTimer = 0.0f;
            }

            inboss = false;

            winMusicTimer += GetFrameTime();
            if (winMusicTimer < winMusicDuration)
            {
                UpdateMusicStream(winMusic);
            }

            // Show victory screen
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0 ,0 ,0 ,125 });

            int winMessageWidth = MeasureText("CONGRATULATIONS YOU WON!!", 60);
            int winMessageHeight = 60;

            float x = (screenWidth - winMessageWidth) / 2;
            float y = (screenHeight - winMessageHeight) / 2;

            // Show victory messages
            DrawTextEx(font, "CONGRATULATIONS, YOU WON", { x + 80, y }, 60, 2, WHITE);

            int retryWidth = MeasureText("Press any key to return to the menu", 30);
            DrawTextEx(font, "Press any key to return to the menu", { (float)(screenWidth - retryWidth) / 2 + 70, screenHeight / 2 + 80 }, 30, 2, WHITE);

            EndDrawing();

            canAct = false;

            isNextScreenTimerStarted = true;

            if (GetKeyPressed() != 0 && canPass) // Detect any key
            {
                // Reset the initial states
                isNextScreenTimerStarted = false;
                isMenuTimerStarted = true;
                hasWon = false; // Reset the variable
                inMenu = true; // Return to menu
                score = 0; // Reset the score
                life = 3; // Reset the life
                level = 1;
                currentWave = 0; // Reset the waves
                currentEnemies = 0; // Reset the enemies

                // Reset the position of the player
                player.x = (screenWidth - player.width) / 2.0f;

                // Clear all the enemies and bullets, just in case
                bullets.clear();
                enemies.clear();
            }
            continue; // Avoid the code is still executing in the victory menu
        }

        else winMusicStarted = false;

        if (!gameOver || !hasWon) canPass = false;

        for (const PowerUp& powerUp : powerUps)
        {
            if (powerUp.active && powerUp.type == Double_shot)
            {
                DrawTexture(doubleShotSprite, (int)powerUp.rect.x, (int)powerUp.rect.y, WHITE); // Draw the double shot item
            }

            else if (powerUp.active && powerUp.type == Shield)
            {
                DrawTexture(shieldSprite, (int)powerUp.rect.x, (int)powerUp.rect.y, WHITE); // Cambiar al sprite del escudo cuando esté hecho
            }
        }

        EndDrawing();
    }

    // Unload resources
    UnloadTexture(shipSpriteBase);
    UnloadTexture(shipSpriteDouble);
    UnloadTexture(draconoida);
    UnloadTexture(bulletSprite);
    UnloadTexture(gamePlayBackgroundLevel1);
    UnloadTexture(menuBackground);
    UnloadFont(font);

    CloseWindow();
    return 0;
}