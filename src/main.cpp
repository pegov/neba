#include <rl/raylib.h>
#include <rl/raymath.h>

const int WIDTH = 640;
const int HEIGHT = 480;

int main() {
    InitWindow(WIDTH, HEIGHT, "neba example");
    SetTargetFPS(144);

    Vector2 circle = {100.0f, 100.0f};
    float speed = 200.0f;

    Vector2 mov = {0.0f, 0.0f};

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        mov = {0.0f, 0.0f};
        if (IsKeyDown(KEY_W)) mov.y -= 1.0f;
        if (IsKeyDown(KEY_S)) mov.y += 1.0f;
        if (IsKeyDown(KEY_A)) mov.x -= 1.0f;
        if (IsKeyDown(KEY_D)) mov.x += 1.0f;

        Vector2 norm = Vector2Normalize(mov);
        circle += norm * speed * dt;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("neba text", 200, 200, 20, LIGHTGRAY);
            DrawCircleV(circle, 10.0f, RED);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
