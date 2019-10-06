#pragma once
#include "Rect.hpp"

class Entity
{
public:
    Entity() {}
    Entity(float x, float y, float w, float h, tako::Color c) : position(x, y), size (w, h), color(c) {}

    Rect GetRect() const
    {
        return {position, size};
    }

    Rect GetRenderRect() const
    {
        auto pos = position;
        pos.x -= size.x/2;
        pos.y += size.y/2;
        return {pos, size};
    }

    tako::Vector2 position;
    tako::Vector2 size;
    tako::Color color;
};

class IMoveable : public virtual Entity
{
public:
    virtual tako::Vector2 Velocity() const = 0;
    virtual void Velocity(tako::Vector2 velocity) = 0;
    virtual void SetGrounded(bool grounded) = 0;
};
