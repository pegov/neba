#include <cmath>
#include <cstdio>
#include <vector>

#include <rl/raylib.h>
#include <rl/raymath.h>

const int WIDTH = 640;
const int HEIGHT = 480;

struct Bullet {
    Vector2 start;
    Vector2 pos;
    Vector2 direction;
    bool off = false;
};

int main() {
    InitWindow(WIDTH, HEIGHT, "neba example");
    SetTargetFPS(144);

    Vector2 circle = {100.0f, 100.0f};
    float speed = 250.0f;
    float acc_speed = 0.01f;

    float bullet_speed = 100.0f;
    float bullet_limit = 100.0f;
    std::vector<Bullet> bullets;
    bullets.reserve(256);

    bullets.push_back({{circle.x + 1.0f, circle.y + 1.0f}, {circle.x + 1.0f, circle.y + 1.0f}, {1.0f, 1.0f}});

    Vector2 mov = {0.0f, 0.0f};
    Vector2 acc = {0.0f, 0.0f};
    Vector2 slow = {0.0f, 0.0f};
    Vector2 neg;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        // TODO: lerp
        if (IsKeyDown(KEY_W)) acc.y -= acc_speed;
        if (IsKeyDown(KEY_S)) acc.y += acc_speed;
        if (IsKeyDown(KEY_A)) acc.x -= acc_speed;
        if (IsKeyDown(KEY_D)) acc.x += acc_speed;

        // TODO: any helper func?
        if (acc.x > 1.0f) {
            acc.x = 1.0f;
        }
        if (acc.x < -1.0f) {
            acc.x = -1.0f;
        }
        if (acc.y > 1.0f) {
            acc.y = 1.0f;
        }
        if (acc.y < -1.0f) {
            acc.y = -1.0f;
        }

        if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S)) {
            neg = Vector2Normalize(Vector2Negate(acc));
            acc.y += neg.y * acc_speed;
        }

        if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D)) {
            neg = Vector2Normalize(Vector2Negate(acc));
            acc.x += neg.x * acc_speed;
        }


        Vector2 norm = Vector2Normalize(acc);
        float dist = Vector2Length(acc);
        if (dist > 1.0f) {
            dist = 1.0f;
        }

        circle += norm * dist * speed * dt;

        for (auto &bullet : bullets) {
            if (bullet.off) {
                continue;
            }

            bullet.pos += bullet.direction * bullet_speed * dt;
            printf("bullet x=%f y=%f\n", bullet.pos.x, bullet.pos.y);

            if (Vector2Distance(bullet.pos, bullet.start) > bullet_limit) {
                bullet.off = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bullets.push_back({
                circle,
                circle,
                Vector2Normalize(mouse - circle),
            });
        }


        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("neba text", 200, 200, 20, LIGHTGRAY);
            DrawCircleV(circle, 10.0f, RED);
            DrawLineEx(circle, mouse, 1.0f, BLACK);

            for (auto &bullet : bullets) {
                if (!bullet.off) {
                    DrawCircleV(bullet.pos, 3.0f, BLACK);
                }
            }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
