#include <stdio.h>

#include <rl/raylib.h>

const int WIDTH = 640;
const int HEIGHT = 480;

int main() {
    InitWindow(WIDTH, HEIGHT, "neba example");
    SetTargetFPS(144);

    Vector2 circle = {100.0f, 100.0f};
    float speed = 200.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_W)) circle.y -= speed*dt;
        if (IsKeyDown(KEY_S)) circle.y += speed*dt;
        if (IsKeyDown(KEY_A)) circle.x -= speed*dt;
        if (IsKeyDown(KEY_D)) circle.x += speed*dt;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("neba text", 200, 200, 20, LIGHTGRAY);
            DrawCircleV(circle, 10.0f, RED);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
