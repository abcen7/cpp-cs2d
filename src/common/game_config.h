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
    std::string playerPath = "assets/tds-assets/PNG/Man Blue/manBlue_gun.png";
    std::string botPath = "assets/tds-assets/PNG/Zombie 1/zoimbie1_gun.png";
    int playerSize = 64;
    int botSize = 64;

    std::string floorPath = "assets/tds-assets/PNG/Tiles/tile_01.png";
    std::string floorAltPath = "assets/tds-assets/PNG/Tiles/tile_02.png";
    std::string wallPath = "assets/tds-assets/PNG/Tiles/tile_13.png";
    std::string spawnPlayerPath = "assets/tds-assets/PNG/Tiles/tile_04.png";
    std::string spawnBotPath = "assets/tds-assets/PNG/Tiles/tile_05.png";
    std::string bonusSpotPath = "assets/tds-assets/PNG/Tiles/tile_03.png";

    std::string bonusHealthPath = "assets/tds-assets/PNG/Tiles/tile_534.png";
    std::string bonusAmmoPath = "assets/tds-assets/PNG/weapon_gun.png";
    std::string bonusArmorPath = "assets/tds-assets/PNG/Tiles/tile_505.png";

    std::string bulletPath = "assets/tds-assets/PNG/Tiles/tile_148.png";
    int bonusPickupSize = 28;
    int bulletSize = 10;
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
