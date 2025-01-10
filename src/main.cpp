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

bool circle_rectangle_collide(Vector2 circle_pos, float circle_radius, Rectangle rectangle) {
    float test_x = circle_pos.x;
    float test_y = circle_pos.y;
    if (circle_pos.x < rectangle.x) test_x = rectangle.x;
    else if (circle_pos.x > rectangle.x + rectangle.width) test_x = rectangle.x + rectangle.width;
    if (circle_pos.y < rectangle.y) test_y = rectangle.y;
    else if (circle_pos.y > rectangle.y + rectangle.height) test_y = rectangle.y + rectangle.height;

    float distance = Vector2Distance(Vector2{circle_pos.x, circle_pos.y},Vector2{test_x, test_y});

    if (distance <= circle_radius) {
        return true;
    }

    return false;
}

int main() {
    InitWindow(WIDTH, HEIGHT, "neba example");
    SetTargetFPS(144);

    Vector2 player = {100.0f, 100.0f};
    float player_radius = 10.0f;
    float speed = 250.0f;
    float acc_speed = 0.01f;

    Rectangle wall = {
        .x = 250.0f,
        .y = 250.0f,
        .width = 80.0f,
        .height = 40.0f,
    };

    Camera2D camera = {};
    camera.target = player;
    camera.offset = {WIDTH/2.0f, HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    float bullet_speed = 800.0f;
    float bullet_limit = 1500.0f;
    float bullet_radius = 5.0f;
    std::vector<Bullet> bullets;
    bullets.reserve(256);

    bullets.push_back({
        {player.x + 1.0f, player.y + 1.0f},
        {player.x + 1.0f, player.y + 1.0f},
        {1.0f, 1.0f}
    });

    Vector2 mov = {0.0f, 0.0f};
    Vector2 acc = {0.0f, 0.0f};
    Vector2 slow = {0.0f, 0.0f};
    Vector2 neg;

    Vector2 target = {150.0f, 150.0f};
    float target_radius = 30.0f;

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

        float circle_distance = Vector2Distance(player, target);
        if (circle_distance <= player_radius+target_radius) {
            Vector2 tn = Vector2Normalize(player - target);
            float alpha = Vector2Angle(tn, acc);
            float dalpha = 2*alpha;
            float beta = M_PI - dalpha;
            acc = Vector2Rotate(acc, beta);
        }

        Vector2 norm = Vector2Normalize(acc);
        float dist = Vector2Length(acc);
        if (dist > 1.0f) {
            dist = 1.0f;
        }

        player += norm * dist * speed * dt;

        camera.target = player;

        for (auto &bullet : bullets) {
            if (bullet.off) {
                continue;
            }

            bullet.pos += bullet.direction * bullet_speed * dt;

            float distance = Vector2Distance(bullet.pos, target);
            if (distance <= bullet_radius+target_radius) {
                bullet.off = true;
                continue;
            }

            if (Vector2Distance(bullet.pos, bullet.start) > bullet_limit) {
                bullet.off = true;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 o = {camera.target.x - WIDTH/2.0f, camera.target.y - HEIGHT/2.0f};
            Vector2 mouse_abs = o + mouse;
            bullets.push_back({
                camera.target,
                camera.target,
                Vector2Normalize(mouse_abs - camera.target),
            });
        }


        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                DrawRectanglePro(wall, Vector2{0.0f, 0.0f}, 0.0f, MAGENTA);
                DrawCircleV(player, player_radius, RED);
                DrawCircleV(target, target_radius, BLUE);
                for (auto &bullet : bullets) {
                    if (!bullet.off) {
                        DrawCircleV(bullet.pos, bullet_radius, BLACK);
                    }
                }
            EndMode2D();

            DrawLineEx({WIDTH/2.0f, HEIGHT/2.0f}, mouse, 1.0f, BLACK);

            DrawText("neba text", 200, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

