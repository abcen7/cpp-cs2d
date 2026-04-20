#include "model/tile.h"

Tile::Tile(TileType type, int gridX, int gridY) : m_type(type), m_gridX(gridX), m_gridY(gridY) {}

TileType Tile::getType() const {
    return m_type;
}

int Tile::getGridX() const {
    return m_gridX;
}

int Tile::getGridY() const {
    return m_gridY;
}

bool Tile::isPassable() const {
    return m_type != TileType::Wall;
}

std::optional<TileType> Tile::tryParseMapCode(int code) {
    switch (code) {
        case 0:
            return TileType::Floor;
        case 1:
            return TileType::Wall;
        case 2:
            return TileType::SpawnPlayer;
        case 3:
            return TileType::SpawnBot;
        case 4:
            return TileType::BonusSpot;
        default:
            return std::nullopt;
    }
}
