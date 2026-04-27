#pragma once

#include <string>

struct WindowConfig {
    int width = 1024;
    int height = 768;
    int hudHeight = 56;
    std::string title = "CS2DS";
};

struct LoopConfig {
    float logicHz = 60.0f;
    float renderHz = 60.0f;
};

struct MatchConfig {
    float durationSeconds = 180.0f;
    int scoreLimit = 15;
    float playerRespawnSeconds = 3.0f;
    float botRespawnSeconds = 5.0f;
};

struct BonusConfig {
    float spawnMinSeconds = 6.0f;
    float spawnMaxSeconds = 11.0f;
    float minDistance = 22.0f;
    float pickupRadius = 40.0f;
};

struct TextureConfig {
    bool enabled = false;
    std::string playerPath = "assets/textures/persons/soldier_light.png";
    std::string botPath = "assets/textures/persons/soldier_dark.png";
    int playerSize = 32;
    int botSize = 32;
};

struct MenuConfig {
    std::string simpleMapPath = "assets/maps/de_simple.map";
    std::string crossfireMapPath = "assets/maps/de_crossfire.map";
    std::string mazeRunMapPath = "assets/maps/de_maze_run.map";
    std::string arenaPlusMapPath = "assets/maps/de_arena_plus.map";
};

struct GameConfig {
    WindowConfig window;
    LoopConfig loop;
    MatchConfig match;
    BonusConfig bonus;
    TextureConfig textures;
    MenuConfig menu;

    static GameConfig loadFromFile(const std::string& path);
};
