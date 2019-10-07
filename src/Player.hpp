#pragma once
#include "Entity.hpp"

class Player : public IMoveable
{
public:
    Player() {}
    Player(float x, float y, float w, float h, tako::Texture* img) : Entity(x, y, w, h, img) {}
    tako::Vector2 velocity;
    bool grounded = false;
    bool gainedWalk = false;
    bool gainedJump = false;
    bool gainedDoubleJump = false;
    bool canDoubleJump = false;

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
