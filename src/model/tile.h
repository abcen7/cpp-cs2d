#pragma once

#include <optional>

enum class TileType {
    Floor = 0,
    Wall = 1,
    SpawnPlayer = 2,
    SpawnBot = 3,
    BonusSpot = 4
};

class Tile {
public:
    Tile() = default;
    Tile(TileType type, int gridX, int gridY);

    [[nodiscard]] TileType getType() const;
    [[nodiscard]] int getGridX() const;
    [[nodiscard]] int getGridY() const;
    [[nodiscard]] bool isPassable() const;

    static std::optional<TileType> tryParseMapCode(int code);

private:
    TileType m_type = TileType::Floor;
    int m_gridX = 0;
    int m_gridY = 0;
};
