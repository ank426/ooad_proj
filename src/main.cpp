#include <iostream> // IWYU pragma: keep
#include <vector>
#include <random>
#include <cstdlib>

#include <raylib-cpp.hpp>

std::random_device RANDOM_DEVICE;
std::mt19937 GENERATOR(RANDOM_DEVICE());

#define FPS 60

#define WIDTH 960
#define HEIGHT 540

#define P_WIDTH 80
#define P_HEIGHT 120
#define MOV_SPEED 3
#define JUMP 30
#define GRAVITY 2

#define E_WIDTH 30
#define E_SPEED 3
#define E_SPAWN_RATE 0.5

#define FONT_SIZE 100

raylib::Window window(WIDTH, HEIGHT);

raylib::Rectangle PlatList[] = {
  {  0, 520, 960, 20},
  {550, 400, 200, 20},
  {250, 250, 200, 20},
};

void gameover() {
  for (int i = 0; i < FPS; i++) {
    window.BeginDrawing();
    window.ClearBackground();
    raylib::Text text("Game Over", FONT_SIZE);
    text.Draw((WIDTH-text.Measure())/2, (HEIGHT-FONT_SIZE)/2);
    window.EndDrawing();
  }
  exit(0);
}

class Enemy {
private:
  raylib::Rectangle rect;
  raylib::Color color;
  int vel_x;

public:
  Enemy(int y, int vel_x) :
    rect(raylib::Rectangle(vel_x > 0 ? -E_WIDTH : WIDTH, y, E_WIDTH, E_WIDTH)),
    vel_x(vel_x),
    color(RED)
  {}

  const raylib::Rectangle &GetRect() const {
    return rect;
  }

  void Draw() {
    rect.Draw(color);
  }

  void Update() {
    rect.SetX(rect.GetX() + vel_x);
  }
};

std::vector<Enemy> EnemyVec;

class EnemyManager {
private:
  static void addRandomEnemy() {
    EnemyVec.push_back(
      Enemy(
        std::uniform_int_distribution(0, HEIGHT)(GENERATOR),
        std::uniform_int_distribution(0, 1)(GENERATOR) ? E_SPEED : -E_SPEED
      )
    );
  }

public:
  static void run() {
    for (auto it = EnemyVec.begin(); it != EnemyVec.end();) {
      it->Update();
      int x = it->GetRect().GetX();
      if (x < -E_WIDTH || x > WIDTH) it = EnemyVec.erase(it);
      else it++;
    }
    if (std::uniform_real_distribution()(GENERATOR) < E_SPAWN_RATE / FPS) addRandomEnemy();
  }
};

class Player {
private:
  raylib::Rectangle rect;
  raylib::Color color;
  int old_x, old_y;
  int vel_y;
  bool on_ground;

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
    if (rect.GetX() < -P_WIDTH || rect.GetX() > WIDTH) gameover();

    if (raylib::Keyboard::IsKeyDown(KEY_W) || raylib::Keyboard::IsKeyDown(KEY_K))
      if (on_ground) vel_y = -JUMP;

    rect.SetY(rect.GetY() + vel_y);

    vel_y += GRAVITY;
    on_ground = false;
  }

  void HandleCollisions() {
    for (auto plat : PlatList) if (rect.CheckCollision(plat)) HandleCollision(plat);
    for (auto enemy : EnemyVec) if (rect.CheckCollision(enemy.GetRect())) gameover();
  }
};

int main() {
  Player player;

  SetTargetFPS(FPS);

  while (!window.ShouldClose()) {
    player.Update();
    EnemyManager::run();
    player.HandleCollisions();

    window.BeginDrawing();
    window.ClearBackground();
    player.Draw();
    for (auto plat : PlatList) plat.Draw(WHITE);
    for (auto enemy : EnemyVec) enemy.Draw();
    window.EndDrawing();
  }
}
