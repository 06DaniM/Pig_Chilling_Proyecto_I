#include "raylib.h"
#include <queue>
#include <string>

#define BULLET_SPEED 7

typedef struct Bullet {
    Rectangle rect;
    bool active;
} Bullet;

typedef struct Enemy {
    Rectangle rect;
    bool active;
} Enemy;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - keyboard input");

    // Define the player position and size for collision, set it to match your ship's texture
    Rectangle player = { (float)screenWidth / 2, (float)screenHeight / 1.5, 64, 64 }; // Player size (for collision)
    Enemy enemy = { (float)screenWidth / 2, (float)screenHeight / 3, 32, 32 }; // Enemy position

    Texture2D shipSpriteBase = LoadTexture("resources/ship/Nave Base.png");
    Texture2D shipSpriteDouble = LoadTexture("resources/ship/NAVE 2DS 64X64.png");
    Texture2D enemySprite = LoadTexture("resources/enemies/nave draconoida.png");

    Texture2D bulletSprite = LoadTexture("resources/bullets/Disparo_Spaceship.png");
    Texture2D bulletNormalEnemySprite = LoadTexture("resources/bullets/Disparo_Regular_Enemy.png");
    Texture2D bulletBossSprite = LoadTexture("resources/bullets/Disparo_Boss.png");

    Texture2D background = LoadTexture("resources/background.jpg");

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);

    std::queue<Bullet> bullets;
    bool doubleShoot = true;

    enemy.active = true;

    bool pause = false;
    bool gameOver = false;
    int score = 0;
    int counterBullets = 0;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed('P')) pause = !pause;

            if (!pause) {
                // Player movement
                if (IsKeyDown(KEY_D)) player.x += 5.0f;
                if (IsKeyDown(KEY_A)) player.x -= 5.0f;

                // Bullet firing
                if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    if (!doubleShoot) {
                        Bullet bu;
                        bu.rect.x = player.x + player.width / 2 - 16;
                        bu.rect.y = player.y;
                        bu.rect.width = 4;
                        bu.rect.height = 10;
                        bu.active = true;
                        bullets.push(bu);
                    }
                    else {
                        Bullet bu1, bu2;
                        bu1.rect.x = player.x + player.width / 2 - 30;
                        bu1.rect.y = player.y;
                        bu1.rect.width = 4;
                        bu1.rect.height = 10;
                        bu1.active = true;

                        bu2.rect.x = player.x + player.width / 2 - 3;
                        bu2.rect.y = player.y;
                        bu2.rect.width = 4;
                        bu2.rect.height = 10;
                        bu2.active = true;

                        bullets.push(bu1);
                        bullets.push(bu2);
                    }
                }

                // Update bullets
                std::queue<Bullet> tempBullets; // Temp queue to hold active bullets
                while (!bullets.empty()) {
                    Bullet bu = bullets.front();
                    bullets.pop();

                    if (bu.rect.y > 0 && bu.active) {
                        bu.rect.y -= BULLET_SPEED; // Move the bullet upwards

                        // Check for collision with enemy
                        if (CheckCollisionRecs(bu.rect, enemy.rect) && enemy.active) {
                            // Bullet hits enemy
                            bu.active = false;
                            enemy.active = false;
                            score += 100;
                        }

                        if (bu.active) {
                            tempBullets.push(bu); // Only add active bullets to temp queue
                        }
                    }
                }

                // Replace old bullets queue with updated one
                bullets = tempBullets;
            }
        }

        // Drawing
        BeginDrawing();

        DrawTexture(background, 0, 0, WHITE);

        if (!gameOver) {
            // Draw bullets
            std::queue<Bullet> tempBullets = bullets; // Create a temporary queue to avoid modifying the original queue
            while (!tempBullets.empty()) {
                Bullet bullet = tempBullets.front();
                tempBullets.pop();

                if (bullet.active) {
                    DrawTexture(bulletSprite, (int)bullet.rect.x, (int)bullet.rect.y, WHITE);
                }
            }

            // Draw player (ship texture)
            if (!doubleShoot) DrawTexture(shipSpriteBase, (int)player.x, (int)player.y, WHITE);
            else if (doubleShoot) DrawTexture(shipSpriteDouble, (int)player.x, (int)player.y, WHITE);

            // Draw enemy if active
            if (enemy.active) DrawTexture(enemySprite, (int)enemy.rect.x, (int)enemy.rect.y, WHITE);
        }

        // Display score
        DrawTextEx(font, "SCORE", { 50, 15 }, 34, 2, WHITE);
        std::string scoreText = std::to_string(score);
        DrawTextEx(font, scoreText.c_str(), { 50, 40 }, 34, 2, WHITE);

        EndDrawing();
    }


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
