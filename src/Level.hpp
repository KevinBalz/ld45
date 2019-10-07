#pragma once
#include "Rect.hpp"
#include "FileSystem.hpp"
#include "Sprite.hpp"
#include <array>
#include <string_view>
#include "Utility.hpp"
#include "Physics.hpp"
#include "PixelArtDrawer.hpp"
#include <map>

namespace
{
    std::map<char, int> tileMap =
    {
        {'[', 0},
        {'=', 1},
        {']', 2},
        {'#', 4},
        {'*', 7},
        {'<', 9},
        {'-', 10},
        {'>', 11}
    };
}

class Level
{
public:

    void LoadLevel(const char* fileName, const std::array<tako::Sprite*, 12>& tileset)
    {
        constexpr size_t bufferSize = 1024 * 1024;
        std::array<tako::U8, bufferSize> buffer;
        size_t bytesRead = 0;
        if (!tako::FileSystem::ReadFile(fileName, buffer.data(), bufferSize, bytesRead))
        {
            LOG_ERR("Could not read level {}", fileName);
        }

        auto levelStr = reinterpret_cast<const char*>(buffer.data());

        int maxX = 0;
        int maxY = 0;
        int x = 0;
        int y = 0;

        std::vector<char> tileChars;
        tileChars.reserve(bytesRead);
        for (int i = 0; i < bytesRead; i++)
        {
            if (levelStr[i] != '\n' && levelStr[i] != '\0')
            {
                x++;
                tileChars.push_back(levelStr[i]);
            }
            else
            {
                maxY++;
                maxX = std::max(maxX, x);
                x = 0;
            }
        }
        for (int i = 0; i < tileChars.size(); i++)
        {
            int tileX = i % maxX;
            int tileY = i / maxX;
            x = tileX * 16;
            y = (maxY - tileY + 1) * 16;

            char tileChar = tileChars[i];
            if (tileMap.count(tileChar) > 0)
            {
                Rect rect(x, y, 16, 16);
                renderTiles.push_back({tileset[tileMap[tileChar]],rect});
                Physics::AddCollider(ToLevelRect(rect));
            }
            if (tileChar == 'P')
            {
                playerSpawn = tako::Vector2(x + 8, y - 8);
            }
        }

        boundRight = maxX * 16;
        boundTop = maxY * 16;
    }

    void Render(tako::PixelArtDrawer* drawer)
    {
        for (auto tile: renderTiles)
        {
            drawer->DrawSprite(tile.rect.x, tile.rect.y, tile.rect.w, tile.rect.h, tile.sprite);
        }
    }

    Rect GetBounds()
    {
        return {0, 0, boundRight, boundTop};
    }

    inline tako::Vector2 GetSpawn()
    {
        return playerSpawn;
    }
private:
    struct RenderTile
    {
        tako::Sprite* sprite;
        Rect rect;

        RenderTile(tako::Sprite* sprite, Rect rect) : sprite(sprite), rect(rect) {}
    };

    std::vector<RenderTile> renderTiles;
    tako::Vector2 playerSpawn;
    float boundTop;
    float boundRight;
};