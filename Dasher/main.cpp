#include "raylib.h"
#include <iostream>
#include <cmath>
#include <vector>

// Window Size
const int windowWidth{900};
const int windowHeight{625};

// Physics
int gravity{1100};
int velocity{-10};
const int jumpVal{650};
int speed{400};
int nebVel{200};

// is the scarf in air
bool isInAir{};

// Functions
float Vector2Distance(Vector2 a, Vector2 b) // Calculate the distance between two Vector2 positions
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

// Classes
class Background
{
    // Background
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    // For Background Scrolling
    float scrollingBack = 0.0f;
    float scrollingMid = 0.0f;
    float scrollingFore = 0.0f;

public:
    void BackgroundScrolling()
    {
        // Background Scrolling
        scrollingBack -= 0.1f;
        scrollingMid -= 0.5f;
        scrollingFore -= 1.0f;

        if (scrollingBack <= -background.width * 2)
            scrollingBack = 0;
        if (scrollingMid <= -midground.width * 2)
            scrollingMid = 0;
        if (scrollingFore <= -foreground.width * 2)
            scrollingFore = 0;
    }

    void DrawBackground()
    {
        // Drawing Background ->
        // Draw background image twice
        DrawTextureEx(background, (Vector2){scrollingBack, 20}, 0.0f, 3.0f, WHITE);
        DrawTextureEx(background, (Vector2){background.width * 3 + scrollingBack, 20}, 0.0f, 3.0f, WHITE);

        // Draw midground image twice
        DrawTextureEx(midground, (Vector2){scrollingMid, 20}, 0.0f, 3.0f, WHITE);
        DrawTextureEx(midground, (Vector2){midground.width * 3 + scrollingMid, 20}, 0.0f, 3.0f, WHITE);

        // Draw foreground image twice
        DrawTextureEx(foreground, (Vector2){scrollingFore, 70}, 0.0f, 3.0f, WHITE);
        DrawTextureEx(foreground, (Vector2){foreground.width * 3 + scrollingFore, 70}, 0.0f, 3.0f, WHITE);
    }

    void UnloadBackground()
    {
        UnloadTexture(background); // Unload background texture
        UnloadTexture(midground);  // Unload midground texture
        UnloadTexture(foreground); // Unload foreground texture
    }
};

class Scarfy
{
    // Scarfy Texture
    Texture2D scarfy = LoadTexture("textures/scarfy.png"); // compund datatype we can get the function by . notation

    // For Player Animation
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8; // Number of spritesheet frames shown by second

public:
    // Scarfy Rectangle
    Rectangle scarfyRec = {0.0f, 0.0f, (float)scarfy.width / 6, (float)scarfy.height};
    Vector2 scarfyPos = {windowWidth / 2 - scarfyRec.width / 2, windowHeight - scarfyRec.height};

    // For Scarfy Animation
    void animation()
    {
        // Player Animation
        framesCounter++;

        if (!isInAir)
        {
            if (framesCounter >= (60 / framesSpeed))
            {
                framesCounter = 0;
                currentFrame++;

                if (currentFrame > 5)
                    currentFrame = 0;

                scarfyRec.x = (float)currentFrame * (float)scarfy.width / 6;
            }
        }
    }

    // To Draw Scarfy
    void DrawScarfy()
    {
        // draw scarfies
        DrawTextureRec(scarfy, scarfyRec, scarfyPos, WHITE);
    }

    // To Unload Textures
    void UnloadScarfy()
    {
        UnloadTexture(scarfy); // Texture unloading
    }

    // Player Movement
    void Movement(float dT)
    {
        // perform ground check
        if (scarfyPos.y >= windowHeight - scarfyRec.height)
        {
            // rectangle on the ground
            velocity = 0;
            isInAir = false;
        }
        else
        {
            // rectangle is on the air
            // apply gravity
            velocity += gravity * dT;
            isInAir = true;
        }
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity -= jumpVal;
        }

        // update  position
        scarfyPos.y += velocity * dT;

        // Left Right Movement
        if (IsKeyDown(KEY_LEFT) && !isInAir)
        {
            if (scarfyPos.x > 0)
            {
                scarfyPos.x -= speed * dT;
            }
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            if (scarfyPos.x > 0)
            {
                scarfyPos.x -= (150) * dT;
            }
        }

        if (IsKeyDown(KEY_RIGHT) && !isInAir)
        {
            if (scarfyPos.x + scarfyRec.width < windowWidth)
            {
                scarfyPos.x += speed * dT;
            }
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            if (scarfyPos.x + scarfyRec.width < windowWidth)
            {
                scarfyPos.x += (150) * dT;
            }
        }
    }
};

class Nebula
{
    // For Nebula Animation
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 1.0 / 12.0; // Number of spritesheet frames shown by second

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

public:
    Rectangle nebRec = {0.0f, 0.0f, (float)nebula.width / 8, (float)nebula.height / 8};
    Vector2 nebPos{windowWidth, windowHeight - nebRec.height};

    void NebulaAnimation(float dT)
    {
        framesCounter += dT;

        if (framesCounter >= framesSpeed)
        {
            framesCounter = 0.0;
            nebRec.x = currentFrame * nebRec.width;
            currentFrame++;

            if (currentFrame > 7)
            {
                currentFrame = 0;
            }
        }
    }

    void Movement(float dT)
    {
        // nebula postion
        nebPos.x -= nebVel * dT;
    }

    void DrawNebula()
    {
        DrawTextureRec(nebula, nebRec, nebPos, WHITE);
    }

    void UnloadNebula()
    {
        UnloadTexture(nebula); // Texture unloading
    }
};

enum GameState
{
    Playing,
    GameOver,
    Win
};

int main()
{
    // window dimenstion
    InitWindow(windowWidth, windowHeight, "Dashers");
    SetTargetFPS(60);

    // Player
    Scarfy player;
    // Background
    Background background;
    // Create a vector of Nebula objects
    std::vector<Nebula> nebulae(5);

    // Variables
    const float collisionDistanceThreshold = 50.0f;
    // To get in which state is our game in
    GameState gameState = Playing;

    // Initialize nebula positions
    float xStart = windowWidth;
    float pad = 450.0f;

    for (long long unsigned int i = 0; i < nebulae.size(); ++i)
    {
        nebulae[i].nebPos.x = xStart + pad;
        nebulae[i].nebPos.y = windowHeight - nebulae[i].nebRec.height;
        xStart += nebulae[i].nebRec.width + pad;
    }

    // Set up finish line
    Vector2 finishLinePos = {nebulae.back().nebPos.x, windowHeight};
    finishLinePos.x += nebulae.back().nebRec.width + 450.0f;

    while (!WindowShouldClose())
    {
        // Delta Time
        const float dT{GetFrameTime()};
        // To update finish line pos
        finishLinePos.x -= nebVel * dT;

        if (gameState == Playing)
        {
            // Player Animaton
            player.animation();
            player.Movement(dT);

            // Nebula Animation
            for (Nebula &nebula : nebulae)
            {
                nebula.NebulaAnimation(dT);
                nebula.Movement(dT);
            }

            // Background Scrolling
            background.BackgroundScrolling();
        }

        // Check if player crossed the finish line
        if (gameState == Playing && player.scarfyPos.x >= finishLinePos.x)
        {
            // You win!
            gameState = Win;
        }

        for (const Nebula &nebula : nebulae)
        {
            if (Vector2Distance(player.scarfyPos, nebula.nebPos) < collisionDistanceThreshold)
            {
                gameState = GameOver;
                break;
            }
        }

        // start drawing
        BeginDrawing();
        ClearBackground(GetColor(0x052c46ff));

        // Drawing Background
        background.DrawBackground();

        if (gameState == GameOver)
        {
            // lose the game
            DrawText("Game Over!", windowWidth / 2 - MeasureText("Game Over!", 80) / 2, windowHeight / 2 - 80 / 2, 80, WHITE);
        }
        else if (gameState == Win)
        {
            // You win!
            DrawText("You Win!", windowWidth / 2 - MeasureText("You Win!", 80) / 2, windowHeight / 2 - 80 / 2, 80, WHITE);
        }
        else
        {
            // Drawing Player
            player.DrawScarfy();

            // Drawing Nebulae
            for (Nebula &nebula : nebulae)
            {
                nebula.DrawNebula();
            }
        }

        // Draw Finish Line
        DrawLine(finishLinePos.x, finishLinePos.y, finishLinePos.x, finishLinePos.y - windowHeight, GREEN);

        // Stop Drawing
        EndDrawing();
    }

    // Unload Textures
    background.UnloadBackground();
    player.UnloadScarfy();

    for (Nebula &nebula : nebulae)
    {
        nebula.UnloadNebula();
    }

    // to close the window
    CloseWindow();

    return 0;
}