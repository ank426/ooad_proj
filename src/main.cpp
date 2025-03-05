#include <iostream> // IWYU pragma: keep

#include <raylib-cpp.hpp>

// #include "external/raylib-cpp/include/Color.hpp"
// #include "external/raylib-cpp/include/Keyboard.hpp"
// #include "external/raylib-cpp/include/Rectangle.hpp"
// #include "external/raylib-cpp/include/Window.hpp"


#define WIDTH 960
#define HEIGHT 540

#define P_WIDTH 80
#define P_HEIGHT 120
#define MOV_SPEED 3
#define JUMP 30
#define GRAVITY 2

#define FPS 60

raylib::Rectangle PlatList[] = {
  {  0, 520, 960, 20},
  {550, 400, 200, 20},
  {250, 250, 200, 20},
};


class Player {
private:
  raylib::Rectangle rect;
  raylib::Color color;
  int old_x, old_y;
  int vel_y;
  bool on_ground;

public:
  Player() : Player(0, 0) {}
  Player(int x, int y) :
    rect(raylib::Rectangle(x, y, P_WIDTH, P_HEIGHT)),
    old_x(x),
    old_y(y),
    vel_y(0),
    on_ground(false),
    color(WHITE)
  {}

  void Draw() {
    rect.Draw(color);
  }

  void Update() {
    old_x = rect.GetX(), old_y = rect.GetY();

    if (raylib::Keyboard::IsKeyDown(KEY_A) || raylib::Keyboard::IsKeyDown(KEY_H))
      rect.SetX(rect.GetX() - MOV_SPEED);
    if (raylib::Keyboard::IsKeyDown(KEY_D) || raylib::Keyboard::IsKeyDown(KEY_L))
      rect.SetX(rect.GetX() + MOV_SPEED);

    if (raylib::Keyboard::IsKeyDown(KEY_W) || raylib::Keyboard::IsKeyDown(KEY_K))
      if (on_ground) vel_y = -JUMP;

    rect.SetY(rect.GetY() + vel_y);
    vel_y += GRAVITY;
    on_ground = false;
  }

  void HandleCollisions() {
    for (auto plat : PlatList) if (rect.CheckCollision(plat)) HandleCollision(plat);
  }

  void HandleCollision(raylib::Rectangle plat) {
    raylib::Rectangle r = rect;
    if (!raylib::Rectangle(old_x, r.GetY(), r.GetWidth(), r.GetHeight()).CheckCollision(plat))
      rect.SetX(plat.x + (old_x < r.GetX() ? -r.GetWidth() : plat.width));
    if (!raylib::Rectangle(r.GetX(), old_y, r.GetWidth(), r.GetHeight()).CheckCollision(plat)) {
      rect.SetY(plat.y + (old_y < r.GetY() ? -r.GetHeight() : plat.height));
      on_ground = old_y < r.GetY();
      vel_y = 0;
    }
  }
};


int main() {
  raylib::Window window(WIDTH, HEIGHT);
  Player player;

  SetTargetFPS(FPS);

  while (!window.ShouldClose()) {
    player.Update();
    player.HandleCollisions();

    window.BeginDrawing();
    window.ClearBackground();
    player.Draw();
    for (auto plat : PlatList) plat.Draw(WHITE);
    window.EndDrawing();
  }
}
