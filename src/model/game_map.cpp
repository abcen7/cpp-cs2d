#include "model/game_map.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

namespace {

std::string trim(std::string s) {
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) {
        s.erase(s.begin());
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) {
        s.pop_back();
    }
    return s;
}

} // namespace

bool GameMap::loadFromFile(const std::string& path) {
    m_tiles.clear();
    m_widthInTiles = 0;
    m_heightInTiles = 0;
    m_hasPlayerSpawn = false;
    m_botSpawnWorldPositions.clear();

    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::vector<std::vector<int>> rows;

    std::string line;
    while (std::getline(file, line)) {
        if (trim(line).empty()) {
            continue;
        }

        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            cell = trim(cell);
            if (cell.empty()) {
                continue;
            }
            try {
                row.push_back(std::stoi(cell));
            } catch (const std::exception&) {
                return false;
            }
        }

        if (m_widthInTiles == 0) {
            m_widthInTiles = static_cast<int>(row.size());
        } else if (static_cast<int>(row.size()) != m_widthInTiles) {
            return false;
        }

        rows.push_back(std::move(row));
    }

    if (rows.empty() || m_widthInTiles == 0) {
        return false;
    }

    m_heightInTiles = static_cast<int>(rows.size());
    m_tiles.reserve(static_cast<size_t>(m_widthInTiles * m_heightInTiles));

    for (int gy = 0; gy < m_heightInTiles; ++gy) {
        for (int gx = 0; gx < m_widthInTiles; ++gx) {
            const int code = rows[static_cast<size_t>(gy)][static_cast<size_t>(gx)];
            const auto typeOpt = Tile::tryParseMapCode(code);
            if (!typeOpt.has_value()) {
                m_tiles.clear();
                m_widthInTiles = 0;
                m_heightInTiles = 0;
                return false;
            }
            const TileType type = *typeOpt;
            m_tiles.emplace_back(type, gx, gy);

            if (type == TileType::SpawnPlayer && !m_hasPlayerSpawn) {
                const auto center = tileCenterToWorld(gx, gy);
                m_playerSpawnWorldX = center.first;
                m_playerSpawnWorldY = center.second;
                m_hasPlayerSpawn = true;
            } else if (type == TileType::SpawnBot) {
                m_botSpawnWorldPositions.push_back(tileCenterToWorld(gx, gy));
            }
        }
    }

    return true;
}

int GameMap::getWidthInTiles() const {
    return m_widthInTiles;
}

int GameMap::getHeightInTiles() const {
    return m_heightInTiles;
}

float GameMap::getWidthPixels() const {
    return static_cast<float>(m_widthInTiles * TILE_SIZE);
}

float GameMap::getHeightPixels() const {
    return static_cast<float>(m_heightInTiles * TILE_SIZE);
}

const Tile& GameMap::getTile(int gridX, int gridY) const {
    return m_tiles[tileIndex(gridX, gridY)];
}

bool GameMap::isInsideMap(int gridX, int gridY) const {
    return gridX >= 0 && gridY >= 0 && gridX < m_widthInTiles && gridY < m_heightInTiles;
}

std::pair<float, float> GameMap::tileCenterToWorld(int gridX, int gridY) {
    const float worldX = (static_cast<float>(gridX) + 0.5f) * static_cast<float>(TILE_SIZE);
    const float worldY = (static_cast<float>(gridY) + 0.5f) * static_cast<float>(TILE_SIZE);
    return {worldX, worldY};
}

std::optional<std::pair<float, float>> GameMap::getPlayerSpawnWorldPosition() const {
    if (!m_hasPlayerSpawn) {
        return std::nullopt;
    }
    return std::make_pair(m_playerSpawnWorldX, m_playerSpawnWorldY);
}

const std::vector<std::pair<float, float>>& GameMap::getBotSpawnWorldPositions() const {
    return m_botSpawnWorldPositions;
}

size_t GameMap::tileIndex(int gridX, int gridY) const {
    return static_cast<size_t>(gridY) * static_cast<size_t>(m_widthInTiles) + static_cast<size_t>(gridX);
}

bool GameMap::isWorldPositionBlockingBullets(float worldX, float worldY) const {
    const int gridX = static_cast<int>(std::floor(worldX / static_cast<float>(TILE_SIZE)));
    const int gridY = static_cast<int>(std::floor(worldY / static_cast<float>(TILE_SIZE)));
    if (!isInsideMap(gridX, gridY)) {
        return true;
    }
    return !getTile(gridX, gridY).isPassable();
}

bool GameMap::isSegmentBlockedByWall(float startX, float startY, float endX, float endY) const {
    const float dx = endX - startX;
    const float dy = endY - startY;
    const float distance = std::sqrt(dx * dx + dy * dy);
    if (distance <= 1.0e-4f) {
        return isWorldPositionBlockingBullets(endX, endY);
    }

    const float step = static_cast<float>(TILE_SIZE) * 0.25f;
    const int steps = 1 + static_cast<int>(std::ceil(distance / step));

    for (int i = 0; i <= steps; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(steps);
        const float x = startX + dx * t;
        const float y = startY + dy * t;
        if (isWorldPositionBlockingBullets(x, y)) {
            return true;
        }
    }

    return false;
}
