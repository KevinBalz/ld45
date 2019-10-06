#pragma once
#include "Rect.hpp"
#include "Entity.hpp"
#include <vector>

namespace
{
    struct MovementData
    {
        IMoveable* entity;
        tako::Vector2 movement;
    };
}

class Physics
{
public:
    static void RegisterEntity(IMoveable *entity)
    {
        entities.push_back(entity);
    }

    static void AddCollider(Rect col)
    {
        colliders.push_back(col);
    }

    static void Update(float dt)
    {
        static std::vector<MovementData> movements;
        movements.clear();
        for (auto entity : entities)
        {
            MovementData mov;
            mov.entity = entity;
            mov.movement = entity->Velocity() * dt;
            movements.push_back(mov);
        }

        for (auto movement : movements)
        {
            auto newXRect = movement.entity->GetRect();
            newXRect.x += movement.movement.x;
            if (!CheckCollision(newXRect, movement.entity))
            {
                movement.entity->position.x = newXRect.x;
            }
            else
            {
                auto prevRound = static_cast<int>(movement.entity->position.x);
                auto nextRound = static_cast<int>(newXRect.x);

                if (prevRound != nextRound)
                {
                    newXRect.x = std::round(newXRect.x);
                    if (!CheckCollision(newXRect, movement.entity))
                    {
                        movement.entity->position.x = newXRect.x;
                    }
                    else
                    {
                        auto vel = movement.entity->Velocity();
                        vel.x = 0;
                        movement.entity->Velocity(vel);
                    }
                }
                else
                {
                    auto vel = movement.entity->Velocity();
                    vel.x = 0;
                    movement.entity->Velocity(vel);
                }
            }

            auto newYRect = movement.entity->GetRect();
            newYRect.y += movement.movement.y;
            if (!CheckCollision(newYRect, movement.entity))
            {
                movement.entity->SetGrounded(false);
                movement.entity->position.y = newYRect.y;
            }
            else
            {
                auto prevRound = static_cast<int>(movement.entity->position.y);
                auto nextRound = static_cast<int>(newYRect.y);

                if (prevRound != nextRound)
                {
                    movement.entity->SetGrounded(nextRound < prevRound);
                    newYRect.y = std::round(newYRect.y);
                    if (!CheckCollision(newYRect, movement.entity))
                    {
                        movement.entity->position.y = newYRect.y;
                    }
                    else
                    {
                        auto vel = movement.entity->Velocity();
                        vel.y = 0;
                        movement.entity->Velocity(vel);
                    }
                }
                else
                {
                    auto vel = movement.entity->Velocity();
                    vel.y = 0;
                    movement.entity->Velocity(vel);
                }
            }
        }
    }
private:
    static std::vector<IMoveable*> entities;
    static std::vector<Rect> colliders;

    static bool CheckCollision(Rect newRect, Entity* self)
    {
        for (auto entity : entities)
        {
            if (entity == self)
            {
                continue;
            }

            if (Rect::Overlap(newRect, entity->GetRect()))
            {
                return true;
            }
        }
        for (auto collider : colliders)
        {
            if (Rect::Overlap(newRect, collider))
            {
                return true;
            }
        }
        return false;
    }
};

std::vector<IMoveable*> Physics::entities;
std::vector<Rect> Physics::colliders;



