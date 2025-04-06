#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Enemy.h"

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

//typedef struct Enemy {
//    Rectangle rect;
//    bool active;
//    bool isAttacking;
//    float attackTime;
//    float attackingTimer;
//    float attackCooldown;
//    Vector2 targetPosition1; // Start Loop position
//    Vector2 targetFinalPosition; // Final position of the Loop
//    Vector2 targetIdlePosition; // Idle position after loop
//    float attackPlayerPos; // Position of the player when attack
//    float entryTime; // Tiempo de entrada
//    int index; // Índice del enemigo en la fila
//    int loopDirection; // 1 = Right | -1 = Left
//    bool enemyInitialState;
//    bool enemyLoopState;
//    bool manual;
//    bool idle;
//    bool random;
//    bool right;
//    bool playerOnRight;
//} Enemy;
//
//typedef struct Bullet_Enemy {
//    Rectangle rect;
//    bool active;
//} Bullet_Enemy;

Enemy enemy;

const int screenWidth = 1152;
const int screenHeight = 896;

int screen = 1; // Levels of the game

int maxEnemies = 5; 
int currentEnemies = 0;

std::vector<Bullet> bullets;
std::vector<Bullet_Enemy> enemyBullets;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Space Attacks!");
    InitAudioDevice();

    Rectangle player = { (screenWidth - 74) / 2.0f, screenHeight / 1.5f, 64, 64 }; // PositionX, PositionY, ColliderX, ColliderY

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

    Texture2D enemySprite = LoadTexture("resources/enemies/nave draconoida.png");

    Texture2D doubleShotSprite = LoadTexture("resources/powerUps/DobleShot_PowerUp.png");
    Texture2D shieldSprite = LoadTexture("resources/powerUps/Shield_PowerUp.png");

    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D bulletEnemySprite = LoadTexture("resources/bullets/Disparo_Regular_Enemy.png");
    Texture2D bulletBossSprite = LoadTexture("resources/bullets/Disparo_Boss.png");

    Texture2D menuBackground = LoadTexture("resources/backgrounds/Menu.png");
    Texture2D shipMenuBackground = LoadTexture("resources/backgrounds/Nave Back.png");
    Texture2D gamePlayBackground = LoadTexture("resources/backgrounds/FONDO_GALAGA.png");

    Texture2D logo = LoadTexture("resources/backgrounds/Logo Game.png");

    Music music = LoadMusicStream("resources/music/02 Regular Stage Theme.ogg");

    Font font = LoadFontEx("Font/Data 70 Regular.otf", 64, 0, 0);

    bool doubleShot = false, shield = false, canAct = false;
    bool pause = false, gameOver = false, hasWon = false;
    bool inMenu = true;
    int score = 0;
    int life = 3;
    float scale = 1.4f; // Reduce a 50% the scale of the sprites

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

            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(menuBackground, 0, 0, WHITE); // Draw the background
            DrawTexture(shipMenuBackground, screenWidth / 2, 450, WHITE); // Draw the background
            DrawTexture(logo, screenWidth/2 - 590/2, 100, WHITE); // Draw the logo

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
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Game manager 
        if (!pause)
        {
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

                bullets.push_back({ { player.x + player.width / 2, player.y, bulletWidth/2-7, bulletHeight }, true }); // Shooting

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
                                bullet.active = false;
                                enemy.active = false;
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

            // Update the enemeis
            for (Enemy& enemy : enemies)
            {
                if (enemy.active)
                {
                    enemy.UpdateEnemy(enemyBullets, enemy, GetFrameTime(), player);
                }
            }

            // Update enemies bullets
            for (Bullet_Enemy& bullet : enemyBullets)
            {
                if (bullet.active)
                {
                    bullet.rect.y += BULLET_SPEED;
                    if (CheckCollisionRecs(player, bullet.rect) && !shield)
                    {
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
                hasWon = true;
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
        if (life <= 0)
        {
            gameOver = true;
        }

        // Draw all the scene
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(gamePlayBackground, 0, 0, WHITE);

        // Draw the bullets
        for (const Bullet& bullet : bullets)
        {
            if (bullet.active)
            {
                // Draw the bullet
                DrawTexture(bulletSprite,
                    (int)(bullet.rect.x + bullet.rect.width / 2 - bulletSprite.width / 2),
                    (int)(bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2),
                    WHITE);
            }
        }

        // Draw the enemy bullets
        for (const Bullet_Enemy& bullet : enemyBullets)
        {
            if (bullet.active)
            {
                DrawTexture(bulletEnemySprite,
                    (int)(bullet.rect.x + bullet.rect.width / 2 - bulletSprite.width / 2),
                    (int)(bullet.rect.y + bullet.rect.height / 2 - bulletSprite.height / 2),
                    WHITE);
            }
        }

        // Draw the ship
        if (doubleShot && !shield)
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
                Vector2 position = { (enemy.rect.x + enemy.rect.width / 2 - enemySprite.width / 2),
                    (enemy.rect.y + enemy.rect.height / 2 - enemySprite.height / 2) };

                DrawTextureEx(enemySprite, position, 0, 1.5f, WHITE);
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