#include "raylib.h"
#include <queue>

#define BULLET_SPEED 7
#define ENEMY_SPEED 3

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

    Rectangle player = { (float)screenWidth / 2, (float)screenHeight / 1.5, 50, 50 }; // Set player size

    std::queue<Bullet> bullets;
    std::queue<Enemy> enemies;
    bool gameOver = false;
    int hard = 220;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (!gameOver)
        {
            // Player movement
            if (IsKeyDown(KEY_D)) player.x += 5.0f;
            if (IsKeyDown(KEY_A)) player.x -= 5.0f;

            // Randomly generate enemies
            if (rand() % hard == 1)
            {
                Enemy enemy;
                enemy.rect.x = GetRandomValue(0, screenWidth - 50);
                enemy.rect.y = -20;
                enemy.rect.width = 50;
                enemy.rect.height = 20;
                enemies.push(enemy);
                if (hard > 10) hard--;
            }

            int enemyCount = enemies.size();
            for (int i = 0; i < enemyCount; i++)
            {
                Enemy enemy = enemies.front();
                enemies.pop();
                enemy.rect.y += ENEMY_SPEED;
                if (enemy.rect.y + enemy.rect.height > screenHeight)
                {
                    gameOver = true;
                }

                else enemies.push(enemy);
            }

            // Bullet firing
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Bullet bu;
                bu.rect.x = player.x + player.width / 2 - 2;
                bu.rect.y = player.y - 10; // Start bullet just above the player
                bu.rect.width = 4;
                bu.rect.height = 10;
                bu.active = true;
                bullets.push(bu);
            }

            // Update bullets
            int bulletCount = bullets.size();
            std::queue<Bullet> tempBullets;

            for (int i = 0; i < bulletCount; i++)
            {
                Bullet bu = bullets.front();
                bullets.pop();

                if (bu.rect.y > 0 && bu.active)
                {
                    bu.rect.y -= BULLET_SPEED;  // Move the bullet upwards
                    tempBullets.push(bu);       // Add to temp queue if active
                }
            }

            bulletCount = bullets.size();
            enemyCount = enemies.size();
            std::queue<Bullet> tempBullets;
            std::queue<Enemy> tempenemies;

            for (int i = 0; i < bulletCount; i++)
            {
                Bullet bullet = bullets.front();
                bullets.pop();
                bool bulletHit = false;
                for (int j = 0; j < enemyCount; j++)
                {
                    Enemy enemy = enemies.front();
                    enemies.pop();
                    if (CheckCollisionRecs(bullet.rect, enemy.rect))
                    {
                        bulletHit = true;
                    }

                    else tempenemies.push(enemy);
                }

                if (!bulletHit) tempBullets.push(bullet);
                enemies = tempenemies;
            }

            // Replace old bullets queue with updated one
            bullets = tempBullets;

            // Collision detection logic can go here
        }

        // Drawing
        BeginDrawing();

        ClearBackground(BLACK);

        if (!gameOver)
        {
            // Draw bullets
            int bulletCount = bullets.size();
            for (int i = 0; i < bulletCount; i++)
            {
                Bullet bullet = bullets.front();
                bullets.pop();
                DrawRectangleRec(bullet.rect, RED);
                bullets.push(bullet);  // Put the bullet back in the queue
            }

            int enemyCount = enemies.size();
            for (int i = 0; i < enemyCount; i++)
            {
                Enemy enemy = enemies.front();
                enemies.pop();
                DrawRectangleRec(enemy.rect, WHITE);
            }

            // Draw player
            DrawRectangleRec(player, BLUE);
        }

        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
