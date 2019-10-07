#pragma once
#include "Entity.hpp"
#include "Player.hpp"
#include <functional>

class Powerup : public Entity
{
public:
    Powerup(float x, float y, float w, float h, tako::Texture* img, std::function<void(Player&)> grantCallback) :
        Entity(x, y, w, h, img),
        grantCallback(grantCallback)
    {
    }

    void PowerUp(Player& player)
    {
        grantCallback(player);
    }
private:
    std::function<void(Player&)> grantCallback;
};
