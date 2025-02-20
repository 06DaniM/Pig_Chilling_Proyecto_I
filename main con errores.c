#include "raylib.h"
#include <queue>

#define BULLET_SPEED 7

typedef struct Bullet {
    Rectangle rect;
    bool active;
} Bullet;

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
    Texture2D bulletSprite = LoadTexture("resources/Disparo_Spaceship.png");

    std::queue<Bullet> bullets;
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

            // Replace old bullets queue with updated one
            bullets = tempBullets;
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

                // Draw the bullet using the same position as the rectangle
                // This makes sure the bullet is drawn in the same place as its rect
                DrawTexture(bulletSprite, (int)bullet.rect.x-13, (int)bullet.rect.y, WHITE);

                bullets.push(bullet);  // Put the bullet back in the queue
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
