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
#include "Enemy.h"
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

typedef struct Bullet {
    Rectangle rect;
    bool active;
} Bullet;

Enemy enemy;

Timer winDelayTimer;
bool isWinTimerStarted = false;

Timer deathDelayTimer;
bool isDeathTimerStarted = false;
bool isHitTimerStarted = false;

const int screenWidth = 1152;
const int screenHeight = 896;

int screen = 1; // Levels of the game

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

Rectangle backgroundGameFrameRec = { 0.0f, 0.0f, (float)screenWidth, (float)screenHeight };

int currentBackgrounGameFrameX = 0;
int currentBackgrounGameFrameY = 0;
int backgrounGameFramesCounter = 0;

Sound shotSound[4] = { 0 };
int currentShotSound;

Sound enemyDestroySound[4] = { 0 };
int currentEnemyDestroySound;

std::vector<Bullet> bullets;
std::vector<Bullet_Enemy> enemyBullets;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Space Attacks!");
    InitAudioDevice();

    Rectangle player = { (screenWidth - 74) / 2.0f, screenHeight / 1.5f, 64, 64 }; // PositionX, PositionY, ColliderX, ColliderY

    Rectangle playerDieFrameRec = { 0,0 , 58, 58};

    int currentPlayerDieFrame = 0;
    int playerDieFramesCounter = 0;

    std::vector<Enemy> enemies; // Vector to manage the generated enemies 
    std::vector<PowerUp> powerUps; // Vector to manage the generated power ups 

    int totalWaves = 3; // Number of waves per screen
    float waveTimer = 0.0f; // Time to start the next wave
    float waveDelay = 10.0f; // Seconds between waves
    int currentWave; // Last wave played

    Texture2D shipSpriteBase = LoadTexture("resources/ship/Nave Base.png");
    Texture2D shipSpriteDouble = LoadTexture("resources/ship/NAVE 2DS 64X64.png");
    Texture2D shipSpriteBuble = LoadTexture("resources/ship/Nave Bubble 0.png");
    Texture2D shipSpriteDoubleandBuble = LoadTexture("resources/ship/Nave 2S Bubble 0.png");
    Texture2D shipSpriteDeathAnim = LoadTexture("resources/ship/PlayerExplosion.png");

    Texture2D enemySprite = LoadTexture("resources/enemies/nave draconoida.png");
    Texture2D enemySpriteDeathAnim = LoadTexture("resources/enemies/deathEnemyAnim.png");

    Texture2D doubleShotSprite = LoadTexture("resources/powerUps/DobleShot_PowerUp.png");
    Texture2D shieldSprite = LoadTexture("resources/powerUps/Shield_PowerUp.png");

    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D bulletEnemySprite = LoadTexture("resources/bullets/Disparo_Regular_Enemy.png");
    Texture2D bulletBossSprite = LoadTexture("resources/bullets/Disparo_Boss.png");

    Texture2D menuBackground = LoadTexture("resources/backgrounds/Menu Background.png");
    Texture2D gamePlayBackground = LoadTexture("resources/backgrounds/Gameplay Background.png");

    Texture2D logo = LoadTexture("resources/backgrounds/Logo Game.png");

    Music music = LoadMusicStream("resources/music/02 Regular Stage Theme.ogg");

    Font font = LoadFontEx("font/Data 70 Regular.otf", 64, 0, 0);

    Sound deathPlayerSound = LoadSound("resources/soundEffects/fighter_destroyed.mp3");

    shotSound[0] = LoadSound("resources/soundEffects/laser_default.mp3");

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

    bool doubleShot = false, shield = false, canAct = false, isVisible = true, beenhitten = false;
    bool pause = false, gameOver = false, hasWon = false;
    bool inMenu = true;
    int score = 0;
    int life = 3;
    float scale = 1.4f; // Reduce the scale of the sprites

    float shotCooldown = 0.3f;  // Time between shots
    float shotTimer = 0.0f;     // Timer for counting seconds
    bool showGameOver = false;

    SetTargetFPS(60);
    PlayMusicStream(music);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        // === HACKS FOR TESTING ===
        if (IsKeyPressed(KEY_R))
        {
            doubleShot = !doubleShot;
        }

        if (IsKeyPressed(KEY_F))
        {
            shield = !shield;
        }

        if (IsKeyPressed(KEY_ENTER)) // Change to when life is <= 0
        {
            life -= 1;
        }

        if (IsKeyPressed(KEY_BACKSPACE)) // Change to when life is <= 0
        {
            hasWon = true;
        }

        // === BEGINING GAME CODE

        // Pause the game
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed('P')) {
            pause = !pause;
            canAct = !canAct;

            if (pause) PauseMusicStream(music);
            else ResumeMusicStream(music);
        }

        // Menu manager
        if (inMenu)
        {
            int textWidth;

            backgrounMenuFramesCounter++;

            if (backgrounMenuFramesCounter >= 2)
            {
                backgrounMenuFramesCounter = 0;
                currentBackgrounMenuFrameX++;

                if (currentBackgrounMenuFrameX >= 6) // máximo índice = 5
                {
                    currentBackgrounMenuFrameX = 0;
                    currentBackgrounMenuFrameY++;

                    if (currentBackgrounMenuFrameY >= 4) // máximo índice = 4
                    {
                        currentBackgrounMenuFrameY = 0;
                    }
                }

                backgroundMenuFrameRec.x = currentBackgrounMenuFrameX * screenWidth;
                backgroundMenuFrameRec.y = currentBackgrounMenuFrameY * screenHeight;
            }


            BeginDrawing();
            ClearBackground(BLACK);
            DrawTextureRec(menuBackground, backgroundMenuFrameRec, { 0,0 }, WHITE);
            //DrawTexture(menuBackground, 0, 0, WHITE); // Draw the background

            DrawTextEx(font, "SCORE", { 100, 30 }, 34, 2, WHITE);
            DrawTextEx(font, "0", { 100, 60 }, 34, 2, WHITE);

            textWidth = MeasureText("PUSH RUN", 50); // Measure the length of the text
            DrawTextEx(font, "PUSH RUN", { ((float)screenWidth - textWidth) / 2, 490 }, 50, 2, GREEN); // Center the text

            textWidth = MeasureText("BUTTON", 50); // Measure the length of the text
            DrawTextEx(font, "BUTTON", { ((float)screenWidth - textWidth) / 2, 560 }, 50, 2, GREEN); // Center the text

            textWidth = MeasureText("FROM PIG CHILLING", 60); // Measure the length of the text
            DrawTextEx(font, "FROM PIG CHILLING", { (float)(screenWidth - textWidth) / 2 + 55, 700 }, 60, 2, WHITE); // Center the text

            EndDrawing();

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Reset the values
                life = 3;
                currentWave = 0;
                inMenu = false;
                canAct = true;
                isVisible = true;
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Game manager 
        if (!pause)
        {
            backgrounGameFramesCounter++;

            if (backgrounGameFramesCounter >= 2)
            {
                backgrounGameFramesCounter = 0;
                currentBackgrounGameFrameX++;

                if (currentBackgrounGameFrameX >= 6)
                {
                    currentBackgrounGameFrameX = 0;
                    currentBackgrounGameFrameY++;

                    if (currentBackgrounGameFrameY >= 4)
                    {
                        currentBackgrounGameFrameY = 0;
                    }
                }

                backgroundGameFrameRec.x = currentBackgrounGameFrameX * screenWidth;
                backgroundGameFrameRec.y = currentBackgrounGameFrameY * screenHeight;
            }

            // Movement of the ship
            if (IsKeyDown(KEY_D) && canAct) player.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_A) && canAct) player.x -= PLAYER_SPEED;

            // Limit the movemente inside the window
            if (player.x < 0) player.x = 0;
            if (player.x > screenWidth - player.width) player.x = screenWidth - player.width;

            // Shots with cooldown
            shotTimer += GetFrameTime();
            if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && shotTimer >= shotCooldown && canAct)
            {
                shotTimer = 0.0f;

                // Define the size of the bullets collision
                float bulletWidth = 16;  // Width of the collision
                float bulletHeight = 12; // Height of the collision

                // === FOR THE 2nd ASSIGNMENT

                bullets.push_back({ { player.x + player.width / 2, player.y, bulletWidth/2-16, bulletHeight }, true }); // Shooting

                PlaySound(shotSound[currentShotSound]);            // play the next open sound slot
                currentShotSound++;                                 // increment the sound slot

                if (currentShotSound >= 4) currentShotSound = 0;

                // === FOR THE FINAL GAME
                /*if (doubleShot)
                {
                    bullets.push_back({ { player.x + player.width / 2 - 15, player.y + player.height / 2, bulletWidth, bulletHeight }, true });
                    bullets.push_back({ { player.x + player.width / 2 + 15, player.y + player.height / 2, bulletWidth, bulletHeight }, true });
                }

                else
                {
                    bullets.push_back({ { player.x + player.width / 2, player.y, bulletWidth, bulletHeight }, true });
                }*/
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

            if (isDeathTimerStarted)
            {
                deathDelayTimer.Update(GetFrameTime());  // Actualiza el temporizador

                if (deathDelayTimer.IsFinished())
                {
                    isDeathTimerStarted = false;
                    beenhitten = false;
                    playerDieFramesCounter = 0;
                    currentPlayerDieFrame = 0;
                    if (life <= 0) gameOver = true;  // Solo se marca como ganado al terminar el temporizador
                    else
                    {
                        player.x = (screenWidth - 74) / 2.0f;
                        player.y = screenHeight / 1.5f;
                        isVisible = true;
                        canAct = true;
                    }
                }
            }

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
                            if (CheckCollisionRecs(bullet.rect, enemy.rect)) // If collision with an enemy
                            {
                                PlaySound(enemyDestroySound[currentEnemyDestroySound]);            // play the next open sound slot
                                currentEnemyDestroySound++;                                 // increment the sound slot

                                if (currentEnemyDestroySound >= 4) currentEnemyDestroySound = 0;

                                bullet.active = false;
                                enemy.gotHit = true;
                                score += 100;
                                currentEnemies--;

                                // === FOR THE FINAL GAME
                                // 20% to generate the item/object
                                //if (GetRandomValue(1, 100) < 20) // 20% de probabilidad de generar un power-up
                                //{
                                //    // Lista de power-ups disponibles según los estados actuales y los que ya están en pantalla
                                //    std::vector<PowerUpType> availablePowerUps;

                                //    PowerUp newPowerUp;

                                //    bool doubleShotOnScreen = false;
                                //    bool shieldOnScreen = false;

                                //    // Verificar si ya hay un power-up de cada tipo en pantalla
                                //    for (const auto& powerUp : powerUps)
                                //    {
                                //        if (powerUp.type == Double_shot) doubleShotOnScreen = true;
                                //        if (powerUp.type == Shield) shieldOnScreen = true;
                                //    }

                                //    // Solo añadir si el power-up no está activo ni en pantalla
                                //    if (!doubleShot && !doubleShotOnScreen) availablePowerUps.push_back(Double_shot);
                                //    if (!shield && !shieldOnScreen) availablePowerUps.push_back(Shield);

                                //    // Solo generamos un power-up si hay disponibles
                                //    if (!availablePowerUps.empty())
                                //    {
                                //        newPowerUp.rect = { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 20, 20 };

                                //        // Convertir size_t a int de forma segura
                                //        int maxIndex = static_cast<int>(availablePowerUps.size()) - 1;
                                //        newPowerUp.type = availablePowerUps[GetRandomValue(0, maxIndex)];

                                //        powerUps.push_back(newPowerUp);
                                //    }
                                //    newPowerUp.active = true;
                                //}

                                break;
                            }
                        }
                    }
                }
            }

            // Delete the inactive bullets
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.active || b.rect.y < 0; }), bullets.end());

            if (beenhitten)
            {
                if (isVisible)
                {
                    // Actualizar frame rect
                    playerDieFrameRec.x = currentPlayerDieFrame * 58;

                    // Solo avanzar si no terminamos la animación
                    if (currentPlayerDieFrame < 4)
                    {
                        playerDieFramesCounter++;

                        if (playerDieFramesCounter >= 15)
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

            // === WAVES ===
            if (currentWave < totalWaves)
            {
                // === Wave 1 ===
                if (currentEnemies == 0 && currentWave == 0)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 100.0f, -100.0f, 1, screenWidth / 1.5f, screenHeight / 2.5f); // Wave 1
                    currentWave++;
                }

                // === Wave 2 ===
                else if (currentEnemies == 0 && currentWave == 1)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 150.0f, screenWidth + 100, -1, screenWidth / 2, screenHeight / 2); // Wave 2
                    currentWave++;
                }

                // === Wave 3 ===
                else if (currentEnemies == 0 && currentWave == 2)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 150.0f, screenWidth + 100, -1, screenWidth / 2, screenHeight / 2); // Wave 3
                    currentWave++;
                }
            }

            // === Ends the first screen after destroying all enemies ===
            else if (currentEnemies == 0 && currentWave >= 3)
            {
                if (!hasWon && !isWinTimerStarted)
                {
                    winDelayTimer.Start(1.0f); // Wait 1 second for the winning screen
                    isWinTimerStarted = true;
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

                        // powerUp.active = false; // Desactivate the power up // Innecesario, creo
                    }
                }
            }

            // Eliminar power-ups inactivos
            powerUps.erase(std::remove_if(powerUps.begin(), powerUps.end(),
                [](const PowerUp& p) { return !p.active || p.rect.y > screenHeight; }), powerUps.end());
        }

        // Game Over when is dead
        //if (life <= 0)
        //{
        //    canAct = false;

        //    if (isVisible)
        //    {
        //        // Actualizar frame rect
        //        playerDieFrameRec.x = currentPlayerDieFrameX * 56;
        //        playerDieFrameRec.y = currentPlayerDieFrameY * 56;

        //        // Solo avanzar si no terminamos la animación
        //        if (currentPlayerDieFrameY < 2)
        //        {
        //            playerDieFramesCounter++;

        //            if (playerDieFramesCounter >= 15)
        //            {
        //                playerDieFramesCounter = 0;
        //                currentPlayerDieFrameX++;

        //                if (currentPlayerDieFrameX >= 2)
        //                {
        //                    currentPlayerDieFrameX = 0;
        //                    currentPlayerDieFrameY++;
        //                }

        //                // Si la animación termina, iniciar delay y ocultar sprite
        //                if (currentPlayerDieFrameY >= 2 && !gameOver && !isDeathTimerStarted)
        //                {
        //                    isVisible = false; // ← Ocultar sprite
        //                    deathDelayTimer.Start(1.0f);
        //                    isDeathTimerStarted = true;
        //                }
        //            }
        //        }
        //    }
        //}

        enemyFramesCounter++;

        if (enemyFramesCounter >= (30))
        {
            enemyFramesCounter = 0;
            currentEnemyFrame++;

            if (currentEnemyFrame > 1) currentEnemyFrame = 0;

            enemyFrameRec.x = (float)currentEnemyFrame * (float)enemy.rect.width;
        }

        // Update the enemeis
        for (Enemy& enemy : enemies)
        {
            enemy.UpdateEnemy(enemyBullets, enemy, GetFrameTime(), player, gameOver);
        }

        // Update enemies bullets
        for (Bullet_Enemy& bullet : enemyBullets)
        {
            if (bullet.active)
            {
                bullet.rect.y += BULLET_SPEED;
                if (CheckCollisionRecs(player, bullet.rect) && !shield && canAct && isVisible)
                {
                    PlaySound(deathPlayerSound);
                    beenhitten = true;
                    canAct = false;
                    bullet.active = false;
                    life--;

                    break;
                }

                else if (CheckCollisionRecs(player, bullet.rect) && shield)
                {
                    shield = false;
                    bullet.active = false;
                    break;
                }
            }
        }        

        // Draw all the scene
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureRec(gamePlayBackground, backgroundGameFrameRec,{0, 0}, WHITE);

        // Draw the bullets
        for (const Bullet& bullet : bullets)
        {
            if (bullet.active)
            {
                // Draw the bullet
                DrawTextureEx(bulletSprite,
                    { (bullet.rect.x + bullet.rect.width / 2 - bulletSprite.width / 2),
                    (bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2) }, 0, 2,
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
                    (bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2) },0 , 2,
                    WHITE);
            }
        }

        // Draw the ship
        if (isVisible)
        {
            if (beenhitten)
            {
                Rectangle dest = {
                        player.x + player.width / 2.0f,
                        player.y + player.height / 2.0f,
                        player.width * 2,
                        player.height * 2
                };

                Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                DrawTexturePro(shipSpriteDeathAnim, playerDieFrameRec, dest, origin, 0, WHITE);
            }

            else if (doubleShot && !shield)
            {
                DrawTexture(shipSpriteDouble, (int)player.x, (int)player.y, WHITE);
            }

            else if (shield && !doubleShot)
            {
                DrawTexture(shipSpriteBuble, (int)player.x, (int)player.y, WHITE);
            }

            else if (doubleShot && shield)
            {
                DrawTexture(shipSpriteDoubleandBuble, (int)player.x, (int)player.y, WHITE);
            }

            else
            {
                DrawTextureEx(shipSpriteBase, { player.x, player.y }, 0, 2, WHITE);
            }
        }

        // Draw the lifes of the ship
        for (int i = 0; i < life; i++) {
            Vector2 position = { 25 + i * (shipSpriteBase.width * scale + 10), screenHeight - shipSpriteBase.height * scale - 25 };
            DrawTextureEx(shipSpriteBase, position, 0.0f, scale, WHITE);
        }

        // Draw the enemies
        for (const Enemy& enemy : enemies)
        {
            if (enemy.active)
            {
                float scale = 1;

                Rectangle source = enemyFrameRec;

                Rectangle dest = {
                    enemy.rect.x + enemy.rect.width / 2.0f,
                    enemy.rect.y + enemy.rect.height / 2.0f,
                    enemy.rect.width * scale,
                    enemy.rect.height * scale
                };

                Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                if(!enemy.gotHit) DrawTexturePro(enemySprite, source, dest, origin, enemy.rotation, WHITE);
                else
                {
                    float scale = 1;

                    Rectangle source = enemy.enemyDeathFrameRec;
                    cout << "in";
                    Rectangle dest = {
                        enemy.rect.x + 54 / 2.0f,
                        enemy.rect.y + 54 / 2.0f,
                        54 * scale,
                        54 * scale
                    };

                    Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

                    DrawTexturePro(enemySpriteDeathAnim, source, dest, origin, 0, WHITE);
                }
            }
        }

        // Display the score
        DrawTextEx(font, TextFormat("SCORE: %i", score), { 10, 10 }, 34, 2, WHITE);

        // Display wave and enemy count
        DrawTextEx(font, TextFormat("Wave %i / %i", currentWave, totalWaves), { screenWidth - 200, 10 }, 34, 2, WHITE); // Quitar cuando esté acabado el manager

        // Draw pause
        if (pause)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0,0,0,125 });
            DrawTextEx(font, "PAUSE", { (screenWidth - 100) / 2, (screenHeight - 50) / 2 }, 50, 2, WHITE);
        }

        // Game Over manager
        if (gameOver)
        {
            // Show the GAME OVER screen
            BeginDrawing();
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0, 0, 0, 120 });

            int gameOverWidth = MeasureText("GAME OVER", 40);
            DrawTextEx(font, "GAME OVER", { (float)(screenWidth - gameOverWidth) / 2, screenHeight / 2 }, 40, 2, RED);

            int retryWidth = MeasureText("Press any key to return to the menu", 30);
            DrawTextEx(font, "Press any key to return to the menu", { (float)(screenWidth - retryWidth) / 2 + 50, screenHeight / 2 + 50 }, 30, 2, WHITE);

            EndDrawing();

            canAct = false;

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Restart the game
                inMenu = true;
                gameOver = false;

                // Restore to its initial state
                player.x = (screenWidth - player.width) / 2.0f;
                bullets.clear();
                score = 0;
                currentWave = 0;
                enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 100.0f, -100.0f, 1, screenWidth / 1.5f, screenHeight / 2.5f);
            }
            continue; // Avoid the code is still executing in the menu
        }

        if (hasWon)
        {
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

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Reset the initial states
                hasWon = false; // Reset the variable
                inMenu = true; // Return to menu
                score = 0; // Reset the score
                life = 3; // Reset the life
                currentWave = 0; // Reset the waves

                // Reset the position of the player
                player.x = (screenWidth - player.width) / 2.0f;

                // Clear all the enemies and bullets, just in case
                bullets.clear();
                enemies.clear();
                currentEnemies = 0;
            }
            continue; // Avoid the code is still executing in the victory menu
        }

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
    UnloadTexture(enemySprite);
    UnloadTexture(bulletSprite);
    UnloadTexture(gamePlayBackground);
    UnloadTexture(menuBackground);
    UnloadFont(font);

    CloseWindow();
    return 0;
}