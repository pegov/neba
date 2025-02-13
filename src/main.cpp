#include <cmath>
#include <cstdio>
#include <vector>

#include <rl/raylib.h>
#include <rl/raymath.h>

#include <fmt/core.h>

const int WIDTH = 1280;
const int HEIGHT = 720;

struct Bullet {
    Vector2 start;
    Vector2 pos;
    Vector2 direction;
    bool off = false;
};

enum Look {
    Up,
    Down,
    Left,
    Right,
};

struct PlayerSprite {
    Texture2D tex;
    float src_width, src_height;
    float dst_width, dst_height;
    Rectangle look_up_src;
    Rectangle look_down_src;
    Rectangle look_left_src;
    Rectangle look_right_src;

    auto init_look_srcs() -> void {
        Vector2 up_offset = {18.0f, 211.0f};
        Vector2 down_offset = {18.0f, 17.0f};
        Vector2 left_offset = {18.0f, 83.0f};
        Vector2 right_offset = {18.0f, 339.0f};
        look_up_src = Rectangle{up_offset.x, up_offset.y, src_width, src_height};
        look_down_src = Rectangle{down_offset.x, down_offset.y, src_width, src_height};
        look_left_src = Rectangle{left_offset.x, left_offset.y, src_width, src_height};
        look_right_src = Rectangle{right_offset.x, right_offset.y, src_width, src_height};
    }

    auto draw(Vector2 pos, Look look) -> void {
        Rectangle dst = Rectangle{
            pos.x-dst_width/2,
            pos.y-dst_height/2,
            dst_width,
            dst_height
        };

        Rectangle src;
        switch (look) {
            case Look::Up:
                src = look_up_src;
                break;
            case Look::Down:
                src = look_down_src;
                break;
            case Look::Left:
                src = look_left_src;
                break;
            case Look::Right:
                src = look_right_src;
                break;
        }

        DrawTexturePro(tex, src, dst, {}, 0.0f, WHITE);
    }
};

bool circle_rectangle_collide(
    Vector2 c,
    float cr,
    Rectangle r
) {
    float test_x = c.x;
    float test_y = c.y;
    if (c.x < r.x) {
        test_x = r.x;
    } else if (c.x > r.x + r.width) {
        test_x = r.x + r.width;
    }
    if (c.y < r.y) {
        test_y = r.y;
    } else if (c.y > r.y + r.height) {
        test_y = r.y + r.height;
    }

    float dist_x = c.x - test_x;
    float dist_y = c.y - test_y;
    float dist = sqrt((dist_x * dist_x) + (dist_y * dist_y));

    if (dist <= cr) {
        return true;
    }

    return false;
}

std::string format_as(Vector2 v) {
    return fmt::format("Vector2: {{x={} y={}}}", v.x, v.y);
}

Vector2 circle_rectangle_tn(
    Vector2 c,
    float cr,
    Rectangle r,
    Vector2 curr_tn
) {
    // top left
    if (c.x < r.x && c.y < r.y) {
        return Vector2Normalize(c - Vector2{r.x, r.y});
    }

    // top right
    if (c.x > r.x && c.x > r.x+r.width && c.y < r.y) {
        return Vector2Normalize(c - Vector2{r.x+r.width, r.y});
    }

    // bottom left
    if (c.x < r.x && c.y > r.y+r.height) {
        return Vector2Normalize(c - Vector2{r.x, r.y+r.height});
    }

    // bottom right
    if (c.x > r.x+r.width && c.y > r.y+r.height) {
        return Vector2Normalize(c - Vector2{r.x+r.width, r.y+r.height});
    }

    // top
    if (c.x > r.x && c.y < r.y) {
        return Vector2{0.0f, -1.0f};
    }

    // bottom
    if (c.x > r.x && c.y > r.y+r.height) {
        return Vector2{0.0f, 1.0f};
    }

    // left
    if (c.x < r.x && c.y > r.y) {
        return Vector2{-1.0f, 0.0f};
    }

    // right
    if (c.x > r.x+r.width && c.y > r.y) {
        return Vector2{1.0f, 0.0f};
    }

    return curr_tn;
}

#define MAX_SOUNDS 32

int main() {
    InitWindow(WIDTH, HEIGHT, "neba example");
    SetTargetFPS(144);

    InitAudioDevice();
    Sound theme = LoadSound("resources/sounds/ambient/theme.mp3");

    Sound sound_array[MAX_SOUNDS] = {};
    int current_sound = 0;
    sound_array[0] = LoadSound("resources/sounds/effects/shot.wav");
    for (int i = 1; i < MAX_SOUNDS; i++) {
        sound_array[i] = LoadSoundAlias(sound_array[0]);
    }

    PlaySound(theme);

    Image player_img = LoadImage("resources/textures/characters/main-walk.png");
    Texture2D player_tex = LoadTextureFromImage(player_img);
    UnloadImage(player_img);

    const float src_width = 12.0f;
    const float src_height = 26.0f;
    PlayerSprite ps = {
        .tex = player_tex,
        .src_width = src_width,
        .src_height = src_height,
        .dst_width = src_width*2,
        .dst_height = src_height*2,
    };
    ps.init_look_srcs();

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

        if (circle_rectangle_collide(player, player_radius, wall)) {
            Vector2 tn = circle_rectangle_tn(player, player_radius, wall, acc);
            fmt::print("{}\n", tn);
            float beta = M_PI - 2*Vector2Angle(tn, acc);
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
            PlaySound(sound_array[current_sound++]);
            if (current_sound >= MAX_SOUNDS) {
                current_sound = 0;
            }
        }


        Look look;
        float eps = 0.1f;
        if (acc.x < -eps) {
            look = Look::Left;
        } else if (acc.x > eps) {
            look = Look::Right;
        } else {
            if (acc.y < -eps) {
                look = Look::Up;
            } else {
                look = Look::Down;
            }
        }


        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                DrawRectanglePro(wall, Vector2{0.0f, 0.0f}, 0.0f, MAGENTA);

                ps.draw(player, look);

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

    UnloadSound(theme);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}

