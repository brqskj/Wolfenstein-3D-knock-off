
#include "raylib.h"
#include "elements/player.hpp"
#include "elements/map.hpp"


// boot
void newLevel(Player& player, Map& map, std::string name) {
    map.loadFile(name);
    player.spawn(map.spawn);
}

// each frame
void input(Player& player, Map& map) { 
    // rotation
    player.turn(GetMouseDelta().x);
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);

    // movement
    Direction direction = {0.f, 0.f};
    if (IsKeyDown(KEY_W)) direction.medial += 1.0f;
    if (IsKeyDown(KEY_S)) direction.medial -= 1.0f;
    if (IsKeyDown(KEY_D)) direction.lateral += 1.0f;
    if (IsKeyDown(KEY_A)) direction.lateral -= 1.0f;
    
    player.virtualMove(direction);
    Direction collision = map.checkCollision(player.displacement, player.pos);
    player.realMove(collision);
}

void render(Player& player, Map& map) {
    BeginDrawing();
    ClearBackground(BLACK);

    HideCursor();
    map.rayCast(player.pos, player.angle, player.fov);
    map.draw({(float)GetScreenWidth(), (float)GetScreenHeight()});

    EndDrawing();
}

void progression(Player& player, Map& map,unsigned int& currentLvl) {
    bool verdict = false;

    if (IsKeyPressed(KEY_E)) verdict = true;    // conditions

    if (verdict) {
        currentLvl += 1;
        std::string fileName = std::to_string(currentLvl); 
        newLevel(player, map, fileName); 
    }
}


int main() {
    InitWindow(800, 400, "Wolfenstein 3D");
    SetTargetFPS(45);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    Player player;
    Map map;
    unsigned int currentLvl = 1;

    newLevel(player, map, "boot");

    while (!WindowShouldClose()) {
        input(player, map);
        render(player, map);
        progression(player, map, currentLvl);
    }

    CloseWindow();
    return 0;
}