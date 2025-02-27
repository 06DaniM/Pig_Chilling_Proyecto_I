#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

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

typedef struct Enemy {
    Rectangle rect;
    bool active;
    bool isAttacking;
    float attackTime;
    float attackCooldown;
    Vector2 targetPosition; // Posición final
    float entryTime; // Tiempo de entrada
    int index; // Índice del enemigo en la fila
    int loopDirection;
} Enemy;

const int screenWidth = 800;
const int screenHeight = 450;
int screen = 1;

int maxEnemies = 5;        // Declarada como global
int currentEnemies = 0;    // Declarada como global

// Declaración de la función antes de main()
void UpdateEnemy(Enemy& enemy, float deltaTime);
void SpawnEnemies(std::vector<Enemy>& enemies, float baseHeight, float baseWidth, int direction);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Space Attacks!");

    Rectangle player = { (screenWidth - 74) / 2.0f, screenHeight / 1.5f, 64, 64 };

    std::vector<Enemy> enemies;
    std::vector<PowerUp> powerUps; // Vector to manage the generated power ups 


    int currentWave = 1;
    int totalWaves = 3;  // Número de oleadas por pantalla
    float waveTimer = 0.0f;
    float waveDelay = 10.0f; // Segundos entre oleadas

    SpawnEnemies(enemies, 100.0f, -50.0f, 1); // Generar la primera oleada

    Texture2D shipSpriteBase = LoadTexture("resources/ship/Nave Base.png");
    Texture2D shipSpriteDouble = LoadTexture("resources/ship/NAVE 2DS 64X64.png");
    Texture2D shipSpriteBuble = LoadTexture("resources/ship/Nave Bubble 0.png");
    Texture2D shipSpriteDoubleandBuble = LoadTexture("resources/ship/Nave 2S Bubble 0.png");

    Texture2D enemySprite = LoadTexture("resources/enemies/nave draconoida.png");

    Texture2D doubleShotSprite = LoadTexture("resources/powerUps/DobleShot_PowerUp.png");

    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D bulletEnemySprite = LoadTexture("resources/bullets/Disparo_Regular_Enemy.png");
    Texture2D bulletBossSprite = LoadTexture("resources/bullets/Disparo_Boss.png");

    Texture2D background = LoadTexture("resources/backgrounds/background.jpg");

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    std::vector<Bullet> bullets;
    bool doubleShot = false, shield = false, canShot = false;
    bool pause = false, gameOver = false, hasWon = false;
    bool inMenu = true;
    int score = 0;
    int life = 3;
    float scale = 0.5f; // Reduce a 50% the scale of the sprites

    float shotCooldown = 0.3f;  // Time between shots
    float shotTimer = 0.0f;     // Timer for counting seconds
    bool showGameOver = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
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
            canShot = !canShot;
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
                inMenu = false;
                canShot = true;
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Game manager 
        if (!pause)
        {
            // Movement of the ship
            if (IsKeyDown(KEY_D)) player.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_A)) player.x -= PLAYER_SPEED;

            // Limit the movemente inside the window
            if (player.x < 0) player.x = 0;
            if (player.x > screenWidth - player.width) player.x = screenWidth - player.width;

            // Shots with cooldown
            shotTimer += GetFrameTime();
            if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && shotTimer >= shotCooldown && canShot)
            {
                shotTimer = 0.0f;

                // Define the size of the bullets collision
                float bulletWidth = 16;  // Width of the collision
                float bulletHeight = 12; // Height of the collision

                if (doubleShot)
                {
                    bullets.push_back({ { player.x + player.width / 2 - 15, player.y + player.height / 2, bulletWidth, bulletHeight }, true });
                    bullets.push_back({ { player.x + player.width / 2 + 15, player.y + player.height / 2, bulletWidth, bulletHeight }, true });
                }

                else
                {
                    bullets.push_back({ { player.x + player.width / 2, player.y, bulletWidth, bulletHeight }, true });
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
                            if (CheckCollisionRecs(bullet.rect, enemy.rect)) // Si hay colisión con un enemigo
                            {
                                bullet.active = false;
                                enemy.active = false;
                                score += 100;
                                currentEnemies--;

                                // 20% to generate the item/object
                                if (GetRandomValue(1, 100) < 20) // 20% de probabilidad de generar un power-up
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
                    UpdateEnemy(enemy, GetFrameTime());
                }
            }

            if (currentEnemies == 0 && currentWave < 3)
            {
                currentWave++;
                SpawnEnemies(enemies, 150.0f, screenWidth, -1);
            }
            
            else if(currentEnemies == 0 && currentWave >= 3)
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
            DrawTexture(shipSpriteBase, (int)player.x, (int)player.y, WHITE);
        }

        // Draw the lifes of the ship
        for (int i = 0; i < life; i++) {
            Vector2 position = { 20 + i * (shipSpriteBase.width * scale + 10), screenHeight - shipSpriteBase.height * scale - 20 };
            DrawTextureEx(shipSpriteBase, position, 0.0f, scale, WHITE);
        }

        // Draw the enemies
        for (const Enemy& enemy : enemies)
        {
            if (enemy.active)
            {
                DrawTexture(enemySprite,
                    (int)(enemy.rect.x + enemy.rect.width / 2 - enemySprite.width / 2),
                    (int)(enemy.rect.y + enemy.rect.height / 2 - enemySprite.height / 2),
                    WHITE);
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

            canShot = false;

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Restart the game
                inMenu = true;
                gameOver = false;

                // Restore to its initial state
                player.x = (screenWidth - player.width) / 2.0f;
                bullets.clear();
                score = 0;
                currentWave = 1;
                SpawnEnemies(enemies, 100.0f, -50.0f, 1);
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

            canShot = false;

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Reset the initial states
                hasWon = false;  // Reset the variable
                inMenu = true;   // Return to menu
                score = 0;       // Reset the score
                life = 3;        // Reset the life
                currentWave = 1; // Reset the waves

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
                DrawTexture(bulletBossSprite, (int)powerUp.rect.x, (int)powerUp.rect.y, WHITE); // Cambiar al sprite del escudo cuando esté
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

// Function to spawn enemies in a wave
void SpawnEnemies(std::vector<Enemy>& enemies, float baseHeight, float baseWidth, int direction)
{
    enemies.clear();
    for (int i = 0; i < maxEnemies; i++) {
        float delay = i * 0.5f;
        float startX = baseWidth; // Ahora la posición en X varía según la oleada
        float startY = baseHeight; // La altura inicial varía según la oleada
        float targetX = screenWidth / 6.0f * (i + 1);
        float targetY = baseHeight + 20.0f; // Ajusta el destino en función de la base

        // Asignar la dirección de "looping" directamente según el parámetro
        enemies.push_back({ { startX, startY, 32, 32 }, true, false, 3.0f,
                            (float)GetRandomValue(2, 5), { targetX, targetY }, -delay, i, direction });
    }
    currentEnemies = maxEnemies; // Se reasigna en lugar de sumarlo
}

float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

void UpdateEnemy(Enemy& enemy, float deltaTime)
{
    enemy.entryTime += deltaTime;
    float t = enemy.entryTime;

    float startX = enemy.rect.x;  // Igual a la posición X del enemigo
    float startY = enemy.rect.y;  // Igual a la posición Y del enemigo

    // === Fase de Entrada ===
    if (t < 1.5f)
    {
        float midX = screenWidth / 2.0f;
        float midY = startY;

        if (t < 0.5f)
        {
            enemy.rect.x = startX;
            enemy.rect.y = startY;
        }
        else
        {
            float duration = 10.0f; // Duración para mover el enemigo
            float tProgress = (t - 0.5f) / duration;

            // Para evitar temblores
            if (tProgress > 1.0f)
            {
                tProgress = 1.0f;
            }

            enemy.rect.x = Lerp(startX, midX, tProgress);
            enemy.rect.y = Lerp(startY, midY, tProgress);
        }
    }

    // === Fase de Looping ===
    else if (t < 3.0f)
    {
        float loopT = (t - 1.5f) / 1.5f;
        float radius = 4.0f;

        float centerX = enemy.rect.x;
        float centerY = enemy.rect.y;

        // Usa loopDirection para aplicar el movimiento en la dirección correcta
        enemy.rect.x = centerX + radius * enemy.loopDirection * cos(loopT * PI * 2);
        enemy.rect.y = centerY + radius * sin(loopT * PI * 2);
    }

    // === Fase Final ===
    else
    {
        float finalT = (t - 3.0f) / 1.5f;

        float startX = enemy.rect.x;
        float startY = enemy.rect.y;

        enemy.rect.x = Lerp(startX, enemy.targetPosition.x, finalT);
        enemy.rect.y = Lerp(startY, enemy.targetPosition.y, finalT);

        if (finalT >= 1.0f)
        {
            enemy.rect.x = enemy.targetPosition.x;
            enemy.rect.y = enemy.targetPosition.y;
        }
    }
}
