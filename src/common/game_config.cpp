#include "common/game_config.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {
namespace fs = std::filesystem;

std::string trim(const std::string& value) {
    const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch) != 0; });
    if (begin == value.end()) {
        return {};
    }
    const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch) != 0; }).base();
    return std::string(begin, end);
}

bool parseBool(const std::string& value, bool fallback) {
    std::string lowered = value;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    if (lowered == "1" || lowered == "true" || lowered == "yes" || lowered == "on") {
        return true;
    }
    if (lowered == "0" || lowered == "false" || lowered == "no" || lowered == "off") {
        return false;
    }
    return fallback;
}

int parseInt(const std::string& value, int fallback) {
    try {
        return std::stoi(value);
    } catch (...) {
        return fallback;
    }
}

float parseFloat(const std::string& value, float fallback) {
    try {
        return std::stof(value);
    } catch (...) {
        return fallback;
    }
}

std::string unquote(const std::string& value) {
    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') || (value.front() == '\'' && value.back() == '\''))) {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

fs::path findExistingConfig(const fs::path& path) {
    if (path.is_absolute()) {
        return path;
    }
    const std::vector<fs::path> bases{
        fs::current_path(),
        fs::current_path() / "..",
        fs::current_path() / "../..",
        fs::current_path() / "../../.."};
    for (const auto& base : bases) {
        std::error_code ec;
        const fs::path candidate = base / path;
        if (fs::exists(candidate, ec)) {
            return fs::canonical(candidate, ec);
        }
    }
    return path;
}

std::string resolvePathRelativeTo(const std::string& value, const fs::path& configDir) {
    if (value.empty()) {
        return value;
    }

    fs::path path(value);
    if (path.is_absolute()) {
        std::error_code ec;
        if (fs::exists(path, ec)) {
            return path.lexically_normal().string();
        }
        return value;
    }

    std::vector<fs::path> bases;
    if (!configDir.empty()) {
        bases.push_back(configDir);
        bases.push_back(configDir / "..");
        bases.push_back(configDir / "../..");
    }
    bases.push_back(fs::current_path());
    bases.push_back(fs::current_path() / "..");
    bases.push_back(fs::current_path() / "../..");
    bases.push_back(fs::current_path() / "../../..");

    for (const auto& base : bases) {
        std::error_code ec;
        const fs::path candidate = base / path;
        if (fs::exists(candidate, ec)) {
            std::error_code canonEc;
            const fs::path canon = fs::canonical(candidate, canonEc);
            return (canonEc ? candidate.lexically_normal() : canon).string();
        }
    }
    return value;
}

}

GameConfig GameConfig::loadFromFile(const std::string& path) {
    GameConfig config;
    const fs::path resolvedConfigPath = findExistingConfig(fs::path(path));

    std::ifstream stream(resolvedConfigPath);
    if (!stream.is_open()) {
        std::fprintf(stderr, "[config] cannot open '%s' (cwd='%s'); using defaults\n",
            resolvedConfigPath.string().c_str(),
            fs::current_path().string().c_str());
        return config;
    }

    fs::path configDir;
    {
        std::error_code ec;
        configDir = fs::absolute(resolvedConfigPath, ec).parent_path();
    }

    auto resolvePath = [&](const std::string& value) {
        return resolvePathRelativeTo(value, configDir);
    };

    std::string line;
    while (std::getline(stream, line)) {
        const auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        const auto eqPos = line.find('=');
        if (eqPos == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, eqPos));
        const std::string rawValue = trim(line.substr(eqPos + 1));
        const std::string value = unquote(rawValue);

        if (key == "window.width") {
            config.window.width = std::max(320, parseInt(value, config.window.width));
        } else if (key == "window.height") {
            config.window.height = std::max(240, parseInt(value, config.window.height));
        } else if (key == "window.hud_height") {
            config.window.hudHeight = std::max(0, parseInt(value, config.window.hudHeight));
        } else if (key == "window.title") {
            config.window.title = value;
        } else if (key == "loop.logic_hz") {
            config.loop.logicHz = std::max(1.0f, parseFloat(value, config.loop.logicHz));
        } else if (key == "loop.render_hz") {
            config.loop.renderHz = std::max(1.0f, parseFloat(value, config.loop.renderHz));
        } else if (key == "match.duration_seconds") {
            config.match.durationSeconds = std::max(1.0f, parseFloat(value, config.match.durationSeconds));
        } else if (key == "match.score_limit") {
            config.match.scoreLimit = std::max(1, parseInt(value, config.match.scoreLimit));
        } else if (key == "match.player_respawn_seconds") {
            config.match.playerRespawnSeconds = std::max(0.1f, parseFloat(value, config.match.playerRespawnSeconds));
        } else if (key == "match.bot_respawn_seconds") {
            config.match.botRespawnSeconds = std::max(0.1f, parseFloat(value, config.match.botRespawnSeconds));
        } else if (key == "bonus.spawn_min_seconds") {
            config.bonus.spawnMinSeconds = std::max(0.1f, parseFloat(value, config.bonus.spawnMinSeconds));
        } else if (key == "bonus.spawn_max_seconds") {
            config.bonus.spawnMaxSeconds = std::max(0.1f, parseFloat(value, config.bonus.spawnMaxSeconds));
        } else if (key == "bonus.min_distance") {
            config.bonus.minDistance = std::max(0.0f, parseFloat(value, config.bonus.minDistance));
        } else if (key == "bonus.pickup_radius") {
            config.bonus.pickupRadius = std::max(1.0f, parseFloat(value, config.bonus.pickupRadius));
        } else if (key == "textures.enabled") {
            config.textures.enabled = parseBool(value, config.textures.enabled);
        } else if (key == "textures.player_path") {
            config.textures.playerPath = resolvePath(value);
        } else if (key == "textures.bot_path") {
            config.textures.botPath = resolvePath(value);
        } else if (key == "textures.player_size") {
            config.textures.playerSize = std::max(4, parseInt(value, config.textures.playerSize));
        } else if (key == "textures.bot_size") {
            config.textures.botSize = std::max(4, parseInt(value, config.textures.botSize));
        } else if (key == "textures.floor_path") {
            config.textures.floorPath = resolvePath(value);
        } else if (key == "textures.floor_alt_path") {
            config.textures.floorAltPath = resolvePath(value);
        } else if (key == "textures.wall_path") {
            config.textures.wallPath = resolvePath(value);
        } else if (key == "textures.spawn_player_path") {
            config.textures.spawnPlayerPath = resolvePath(value);
        } else if (key == "textures.spawn_bot_path") {
            config.textures.spawnBotPath = resolvePath(value);
        } else if (key == "textures.bonus_spot_path") {
            config.textures.bonusSpotPath = resolvePath(value);
        } else if (key == "textures.bonus_health_path") {
            config.textures.bonusHealthPath = resolvePath(value);
        } else if (key == "textures.bonus_ammo_path") {
            config.textures.bonusAmmoPath = resolvePath(value);
        } else if (key == "textures.bonus_armor_path") {
            config.textures.bonusArmorPath = resolvePath(value);
        } else if (key == "textures.bullet_path") {
            config.textures.bulletPath = resolvePath(value);
        } else if (key == "textures.bonus_pickup_size") {
            config.textures.bonusPickupSize = std::max(4, parseInt(value, config.textures.bonusPickupSize));
        } else if (key == "textures.bullet_size") {
            config.textures.bulletSize = std::max(2, parseInt(value, config.textures.bulletSize));
        } else if (key == "menu.map.simple") {
            config.menu.simpleMapPath = resolvePath(value);
        } else if (key == "menu.map.crossfire") {
            config.menu.crossfireMapPath = resolvePath(value);
        } else if (key == "menu.map.maze_run") {
            config.menu.mazeRunMapPath = resolvePath(value);
        } else if (key == "menu.map.arena_plus") {
            config.menu.arenaPlusMapPath = resolvePath(value);
        }
    }

    if (config.bonus.spawnMaxSeconds < config.bonus.spawnMinSeconds) {
        config.bonus.spawnMaxSeconds = config.bonus.spawnMinSeconds;
    }

    return config;
}
