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
int screen = 1;

int maxEnemies = 5;
int currentEnemies = 0;

std::vector<Bullet> bullets;
std::vector<Bullet_Enemy> enemyBullets;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Space Attacks!");
    InitAudioDevice();

    Rectangle player = { (screenWidth - 74) / 2.0f, screenHeight / 1.5f, 64, 64 }; // PositionX, PositionY, ColliderX, ColliderY

    std::vector<Enemy> enemies;
    std::vector<PowerUp> powerUps; // Vector to manage the generated power ups 

    int totalWaves = 3;  // Number of waves per screen
    float waveTimer = 0.0f;
    float waveDelay = 10.0f; // Seconds between waves
    int currentWave;

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

    Texture2D background = LoadTexture("resources/backgrounds/FONDO_GALAGA.png");

    Music music = LoadMusicStream("resources/music/02 Regular Stage Theme.ogg");

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

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

        // For testing
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
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(background, 0, 0, WHITE); // Draw the background

            DrawTextEx(font, "SCORE", { 50, 15 }, 34, 2, WHITE);
            DrawTextEx(font, "0", { 50, 40 }, 34, 2, WHITE);

            int textWidth = MeasureText("Press any key to start", 20); // Measure the length of the text
            DrawText("Press any key to start", (screenWidth - textWidth) / 2, screenHeight / 2, 20, WHITE); // Center the text
            EndDrawing();

            if (GetKeyPressed() != 0) // Detect any key
            {
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

                bullets.push_back({ { player.x + player.width / 2, player.y, bulletWidth/2-7, bulletHeight }, true });

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
                            if (CheckCollisionRecs(bullet.rect, enemy.rect)) // Si hay colisión con un enemigo
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
                if (currentEnemies == 0 && currentWave == 0)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 100.0f, -100.0f, 1, screenWidth / 1.5f, screenHeight / 2.5f); // Wave 1
                    currentWave++;
                }

                else if (currentEnemies == 0 && currentWave == 1)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 150.0f, screenWidth + 100, -1, screenWidth / 2, screenHeight / 2); // Wave 2
                    currentWave++;
                }

                else if (currentEnemies == 0 && currentWave == 2)
                {
                    currentEnemies = maxEnemies;
                    enemy.SpawnEnemies(enemies, maxEnemies, currentEnemies, 150.0f, screenWidth + 100, -1, screenWidth / 2, screenHeight / 2); // Wave 3
                    currentWave++;
                }
            }

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
                            doubleShot = true;
                        }

                        else if (powerUp.type == Shield)
                        {
                            shield = true;
                        }

                        powerUp.active = false; // Desactivate the power up
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
        DrawTexture(background, 0, 0, WHITE);

        // Draw the bullets
        for (const Bullet& bullet : bullets)
        {
            if (bullet.active)
            {
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
            ClearBackground(BLACK);

            int gameOverWidth = MeasureText("GAME OVER", 40);
            DrawText("GAME OVER", (screenWidth - gameOverWidth) / 2, screenHeight / 2, 40, RED);

            int retryWidth = MeasureText("Press any key to return to the menu", 20);
            DrawText("Press any key to return to the menu", (screenWidth - retryWidth) / 2, screenHeight / 2 + 50, 20, WHITE);

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
            DrawRectangle(0, 0, screenWidth, screenHeight, { 0,0,0,125 });

            int winMessageWidth = MeasureText("CONGRATULATIONS, YOU WON", 20);
            int winMessageHeight = 20;

            int x = (screenWidth - winMessageWidth) / 2;
            int y = (screenHeight - winMessageHeight) / 2;

            // Show victory messages
            DrawText("CONGRATULATIONS, YOU WON", x, y, 20, WHITE);

            int retryWidth = MeasureText("Press any key to return to the menu", 20);
            DrawText("Press any key to return to the menu", (screenWidth - retryWidth) / 2, screenHeight / 2 + 50, 20, WHITE);

            EndDrawing();

            canAct = false;

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Reset the initial states
                hasWon = false;  // Reset the variable
                inMenu = true;   // Return to menu
                score = 0;       // Reset the score
                life = 3;        // Reset the life
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
    UnloadTexture(background);
    UnloadFont(font);

    CloseWindow();
    return 0;
}

//// Function to spawn enemies in a wave
//void SpawnEnemies(std::vector<Enemy>& enemies, float baseHeight, float baseWidth, int direction, float targetx, float targety)
//{
//    enemies.clear();
//    for (int i = 0; i < maxEnemies; i++) {
//        float delay = i * 0.35f; // Space between enemies
//        float startX = baseWidth; // Position X in the wave
//        float startY = baseHeight; // Position Y in the wave
//        float targetX = targetx; // Loop X position
//        float targetY = targety; // Loop Y position
//        float idletargetX = screenWidth / 6.0f * (i + 0.75f) - 200;
//        float finaltargetX = screenWidth / 6.0f * (i + 0.75f); // Final X position
//        float finaltargetY = baseHeight + 20.0f; // Final Y position
//
//        // Enemy data
//        enemies.push_back({ { startX, startY, 112, 84 }, true, false, 0.0f, 0.0f, // Enemys collision
//                            5.0f,{ targetX, targetY }, { finaltargetX, finaltargetY },{idletargetX, finaltargetY},0.0f ,-delay, i, direction, true, false, true, false, false, false, NULL });
//    }
//    currentEnemies = maxEnemies; // Change to sum when more waves will be added
//}
//
//void UpdateEnemy(std::vector<Bullet_Enemy>& enemyBullets, Enemy& enemy, float deltaTime, Rectangle& player)
//{
//    float midX = screenWidth / 2.0f;
//    float midY = enemy.rect.y;
//
//    enemy.entryTime += deltaTime;
//
//    // === NEW ENEMY MOVEMENT ===
//
//    // El retraso se calcula dependiendo del índice del enemigo
//    float delayTime = 0.0f; // Por ejemplo, 0.5 segundos de retraso por cada enemigo
//
//    // Enemy start de movement with a delay 
//
//    if (enemy.entryTime >= delayTime)
//    {
//        // === NEW ENEMY MOVEMENT ===
//
//        // Calcular la posición circular
//        float radius = 150.0f;  // Radio del círculo
//        float centerX = enemy.targetPosition1.x; // Centro de la órbita
//        float centerY = enemy.targetPosition1.y + radius;
//
//        if (enemy.manual)
//        {
//
//            // Incrementar 't' para el movimiento circular
//            float velocity = 500.0f; // Velocidad de movimiento
//            float moveSpeed = velocity * deltaTime;
//            // === 1ST OBJECTIVE ===
//            if (enemy.enemyInitialState)
//            {
//                // Calcular la distancia entre la posición actual y el objetivo
//                float distX = enemy.targetPosition1.x - enemy.rect.x;
//                float distY = enemy.targetPosition1.y - enemy.rect.y;
//
//                float distance = sqrt(distX * distX + distY * distY); // Distancia total al objetivo
//
//                enemy.enemyLoopState = false;
//                if (distance > moveSpeed)
//                {
//                    // Normalizar la dirección
//                    float directionX = distX / distance;
//                    float directionY = distY / distance;
//
//                    // Mover al enemigo hacia el objetivo
//                    enemy.rect.x += directionX * moveSpeed;
//                    enemy.rect.y += directionY * moveSpeed;
//                }
//
//                else
//                {
//                    enemy.enemyInitialState = false;
//                    enemy.enemyLoopState = true;
//                }
//            }
//
//            // CORREGIR PUNTO DE INICIO DEL LOOP
//
//            // === LOOP MOVEMENT ===
//            else if (!enemy.enemyInitialState && enemy.enemyLoopState)
//            {
//                float t = enemy.entryTime;
//                float loopT = t * 0.5f;  // Controlar la velocidad angular (ajusta este valor si es necesario)
//
//                enemy.rect.x -= cos(loopT * PI * 2) * 5; // Movimiento en X
//                enemy.rect.y -= sin(loopT * PI * 2) * 5 * enemy.loopDirection; // Movimiento en Y
//
//                if (enemy.entryTime > 4.5f)
//                {
//                    enemy.enemyLoopState = false;
//                }
//            }
//
//            // === FINAL STATE ==
//            else if (!enemy.enemyInitialState && !enemy.enemyLoopState)
//            {
//                float distX = enemy.targetFinalPosition.x - enemy.rect.x;
//                float distY = enemy.targetFinalPosition.y - enemy.rect.y;
//
//                float distance = sqrt(distX * distX + distY * distY); // Distancia total al objetivo
//
//                if (distance > moveSpeed)
//                {
//                    // Normalizar la dirección
//                    float directionX = distX / distance;
//
//                    float directionY = distY / distance;
//                    // Mover al enemigo hacia el objetivo
//                    enemy.rect.x += directionX * moveSpeed;
//                    enemy.rect.y += directionY * moveSpeed;
//                }
//
//                else
//                {
//                    enemy.rect.x = enemy.targetFinalPosition.x;
//                    enemy.rect.y = enemy.targetFinalPosition.y;
//
//                    enemy.manual = false;
//                    enemy.idle = true;
//                }
//            }
//        }
//
//        // === RANDOM STATE
//
//        else if (enemy.idle && !enemy.random)
//        {
//            cout << "Idle";
//            // Incrementar 't' para el movimiento circular
//            float velocity2 = 200.0f; // Velocidad de movimiento
//            float moveSpeed = velocity2 * deltaTime;
//
//            float distX1 = enemy.targetIdlePosition.x - enemy.rect.x;
//
//            float distance1 = sqrt(distX1 * distX1); // Distancia total al objetivo
//            float directionX1 = distX1 / distance1;
//
//            float distX2 = enemy.targetFinalPosition.x - enemy.rect.x;
//
//            float distance2 = sqrt(distX2 * distX2); // Distancia total al objetivo
//            float directionX2 = distX2 / distance2;
//
//            if (!enemy.right)
//            {
//                if (distance1 > moveSpeed)
//                {
//                    // Mover al enemigo hacia el objetivo
//                    enemy.rect.x += directionX1 * moveSpeed;
//                }
//
//                else
//                {
//                    enemy.right = true;
//                }
//            }
//
//            else
//            {
//                if (distance2 > moveSpeed)
//                {
//                    // Mover al enemigo hacia el objetivo
//                    enemy.rect.x += directionX2 * moveSpeed;
//                }
//
//                else
//                {
//                    enemy.right = false;
//                }
//            }
//
//            if (enemy.attackCooldown >= 1.5f)
//            {
//                enemy.attackTime = GetRandomValue(1, 1000000);
//                if (enemy.attackTime <= 10)
//                {
//                    enemy.random = true;
//                }
//            }
//
//            if (enemy.attackCooldown < 1.5f)
//            {
//                enemy.attackCooldown += deltaTime;
//            }
//        }
//
//        else if (enemy.random)
//        {
//            float velocity2 = 300.0f; // Velocidad de movimiento
//            float moveSpeed = velocity2 * deltaTime;
//            enemy.attackingTimer += deltaTime;
//
//            float t = enemy.attackingTimer;
//            float loopT = t * 0.5f;
//
//            if (enemy.attackingTimer <= 1.2f)
//            {
//                if (enemy.attackingTimer <= 0.7f)
//                {
//                    enemy.rect.x += cos(loopT * PI * 2.5f) * 5; // Movimiento en X
//                    enemy.rect.y -= sin(loopT * PI * 2.5f) * 2.5f; // Movimiento en Y
//                }
//
//                else
//                {
//                    enemy.rect.x += cos(loopT * PI * 2.5f) * 5;
//                    enemy.rect.y -= sin(loopT * PI * 2.5f) * 2.5f;
//                }
//
//                if (enemy.rect.x < player.x)
//                {
//                    enemy.playerOnRight = false;
//                }
//
//                else enemy.playerOnRight = true;
//
//                enemy.attackPlayerPos = player.x;
//                enemy.enemyLoopState = true;
//            }
//
//            else
//            {
//                float distX1;
//                if (enemy.playerOnRight) distX1 = enemy.targetIdlePosition.x - 400 - enemy.rect.x;
//                else distX1 = enemy.targetIdlePosition.x + 400 - enemy.rect.x;
//                float distY1 = player.y - 100 - enemy.rect.y;
//                float distance1 = sqrt(distX1 * distX1 + distY1 * distY1); // Distancia total al objetivo
//
//                float distX2 = enemy.targetIdlePosition.x - enemy.rect.x;
//                float distY2 = enemy.targetFinalPosition.y - enemy.rect.y;
//                float distance2 = sqrt(distX2 * distX2 + distY2 * distY2); // Distancia total al objetivo
//
//                if (distance1 >= moveSpeed && enemy.enemyLoopState)
//                {
//                    // Normalizar la dirección
//                    float directionX = distX1 / distance1;
//                    float directionY = distY1 / distance1;
//
//                    // Mover al enemigo hacia el objetivo
//                    enemy.rect.x += directionX * moveSpeed;
//                    enemy.rect.y += directionY * moveSpeed;
//
//                    if (enemy.rect.y >= player.y - 150)
//                    {
//                        enemy.enemyLoopState = false;
//                    }
//                }
//
//                else if (!enemy.enemyLoopState)
//                {
//                    if (distance2 > moveSpeed)
//                    {
//                        // Normalizar la dirección
//                        float directionX = distX2 / distance2;
//                        float directionY = distY2 / distance2;
//
//                        // Mover al enemigo hacia el objetivo
//                        enemy.rect.x += directionX * moveSpeed;
//                        enemy.rect.y += directionY * moveSpeed;
//                    }
//
//                    else
//                    {
//                        enemy.random = false;
//                        enemy.attackingTimer = 0.0f;
//                    }
//                }
//            }
//        }
//    }
//}
// enemyBullets.push_back({ { enemy.rect.x + enemy.rect.width / 2, enemy.rect.y + enemy.rect.height / 2, 16, 12 }, true });
