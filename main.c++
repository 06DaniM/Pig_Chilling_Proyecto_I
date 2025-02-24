#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>

#define BULLET_SPEED 7
#define PLAYER_SPEED 5.0f

typedef struct Bullet {
    Rectangle rect;
    bool active;
} Bullet;

typedef struct Enemy {
    Rectangle rect;
    bool active;
} Enemy;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Space Attacks!");

    Rectangle player = { screenWidth / 2.0f, screenHeight / 1.5f, 64, 64 };
    Enemy enemy = { screenWidth / 2.0f, screenHeight / 3.0f, 32, 32, true };

    Texture2D shipSpriteBase = LoadTexture("resources/ship/Nave Base.png");
    Texture2D shipSpriteDouble = LoadTexture("resources/ship/NAVE 2DS 64X64.png");
    Texture2D enemySprite = LoadTexture("resources/enemies/nave draconoida.png");
    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D background = LoadTexture("resources/background.jpg");

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    std::vector<Bullet> bullets;
    bool doubleShoot = false;
    bool pause = false, gameOver = false;
    bool inMenu = true;
    int score = 0;
    int life = 3;
    float scale = 0.5f; // Reduce a 50% the scale of the sprites

    float shootCooldown = 0.3f;  // Time between shots
    float shootTimer = 0.0f;     // Timer for counting seconds
    bool showGameOver = false;


    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // For testing
        if (IsKeyPressed(KEY_R)) 
        {
            doubleShoot = !doubleShoot;
        }

        if (IsKeyPressed(KEY_ENTER)) // Change to when life is <= 0
        {
            life -= 1;
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
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Game Over manager
        if (gameOver)
        {
            // Show the GAME OVER screen
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(background, 0, 0, WHITE);

            int gameOverWidth = MeasureText("GAME OVER", 40);
            DrawText("GAME OVER", (screenWidth - gameOverWidth) / 2, screenHeight / 2, 40, RED);

            int retryWidth = MeasureText("Press any key to return to the menu", 20);
            DrawText("Press any key to return to the menu", (screenWidth - retryWidth) / 2, screenHeight / 2 + 50, 20, WHITE);

            EndDrawing();

            if (GetKeyPressed() != 0) // Detect any key
            {
                // Restart the game
                inMenu = true;
                gameOver = false;

                // Restore to its initial state
                player.x = (screenWidth - player.width) / 2.0f;
                enemy.rect = { screenWidth / 2.0f, screenHeight / 3.0f, 32, 32 };
                enemy.active = true;
                bullets.clear();
                score = 0;
            }
            continue; // Avoid the code is still executing in the menu
        }

        // Pause the game
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed('P')) {
            pause = !pause;
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
            shootTimer += GetFrameTime();
            if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && shootTimer >= shootCooldown)
            {
                shootTimer = 0.0f;
                if (doubleShoot)
                {
                    bullets.push_back({ { player.x + player.width / 2 - 30, player.y, 4, 10 }, true });
                    bullets.push_back({ { player.x + player.width / 2 - 3, player.y, 4, 10 }, true });
                }
                else
                {
                    bullets.push_back({ { player.x + player.width / 2 - 16, player.y, 4, 10 }, true });
                }
            }

            // Update the bullets
            for (Bullet& bullet : bullets)
            {
                if (bullet.active)
                {
                    bullet.rect.y -= BULLET_SPEED;
                    if (CheckCollisionRecs(bullet.rect, enemy.rect) && enemy.active)
                    {
                        bullet.active = false;
                        enemy.active = false;
                        score += 100;
                    }
                }
            }

            // Delete the inactive bullets
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
                [](const Bullet& b) { return !b.active || b.rect.y < 0; }), bullets.end());
        }

        if (life <= 0) // Change to when life is <= 0
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
            if (bullet.active) {
                DrawTexture(bulletSprite, (int)bullet.rect.x, (int)bullet.rect.y, WHITE);
            }
        }

        // Draw the ship
        DrawTexture(doubleShoot ? shipSpriteDouble : shipSpriteBase, (int)player.x, (int)player.y, WHITE);

        for (int i = 0; i < life; i++) {
            Vector2 position = { 20 + i * (shipSpriteBase.width * scale + 10), screenHeight - shipSpriteBase.height * scale - 20 };

            DrawTextureEx(doubleShoot ? shipSpriteDouble : shipSpriteBase, position, 0.0f, scale, WHITE);
        }

        // Draw the enemies
        if (enemy.active) DrawTexture(enemySprite, (int)enemy.rect.x, (int)enemy.rect.y, WHITE);


        // Draw the score
        DrawTextEx(font, "SCORE", { 50, 15 }, 34, 2, WHITE);
        DrawTextEx(font, std::to_string(score).c_str(), { 50, 40 }, 34, 2, WHITE);

        // Draw the pause layer
        if (pause)
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Color{ 0, 0, 0, 128 });

            int pauseWidth = MeasureText("Paused", 20);
            DrawText("Paused", (screenWidth - pauseWidth) / 2, screenHeight / 2 + 50, 20, WHITE);

            int textWidth = MeasureText("Press 'P' again to continue", 20);
            DrawText("Press 'P' again to continue", (screenWidth - textWidth) / 2, screenHeight / 2, 20, WHITE);
        }


        EndDrawing();
    }

    // Release the memory
    UnloadTexture(shipSpriteBase);
    UnloadTexture(shipSpriteDouble);
    UnloadTexture(enemySprite);
    UnloadTexture(bulletSprite);
    UnloadTexture(background);
    UnloadFont(font);

    CloseWindow();
    return 0;
}
