#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "model/tile.h"

class GameMap {
public:
    static constexpr int TILE_SIZE = 32;

    GameMap() = default;

    [[nodiscard]] bool loadFromFile(const std::string& path);

    [[nodiscard]] int getWidthInTiles() const;
    [[nodiscard]] int getHeightInTiles() const;
    [[nodiscard]] float getWidthPixels() const;
    [[nodiscard]] float getHeightPixels() const;

    [[nodiscard]] const Tile& getTile(int gridX, int gridY) const;
    [[nodiscard]] bool isInsideMap(int gridX, int gridY) const;

    /// World coordinates of tile center (for spawns and rendering).
    [[nodiscard]] static std::pair<float, float> tileCenterToWorld(int gridX, int gridY);

    [[nodiscard]] std::optional<std::pair<float, float>> getPlayerSpawnWorldPosition() const;
    [[nodiscard]] const std::vector<std::pair<float, float>>& getBotSpawnWorldPositions() const;

    [[nodiscard]] bool isWorldPositionBlockingBullets(float worldX, float worldY) const;
    [[nodiscard]] bool isSegmentBlockedByWall(float startX, float startY, float endX, float endY) const;

private:
    int m_widthInTiles = 0;
    int m_heightInTiles = 0;
    std::vector<Tile> m_tiles;
    bool m_hasPlayerSpawn = false;
    float m_playerSpawnWorldX = 0.0f;
    float m_playerSpawnWorldY = 0.0f;
    std::vector<std::pair<float, float>> m_botSpawnWorldPositions;

    [[nodiscard]] size_t tileIndex(int gridX, int gridY) const;
};
