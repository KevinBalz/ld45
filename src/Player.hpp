#pragma once
#include "Entity.hpp"

class Player : public IMoveable
{
public:
    Player() {}
    Player(float x, float y, float w, float h) : Entity(x, y, w, h, {100, 0, 255, 255}) {}
    tako::Vector2 velocity;
    bool grounded = false;

    tako::Vector2 Velocity() const override
    {
        return velocity;
    };

    void Velocity(tako::Vector2 velocity) override
    {
        this->velocity = velocity;
    };

    virtual void SetGrounded(bool grounded) override
    {
        this->grounded = grounded;
    }
};
