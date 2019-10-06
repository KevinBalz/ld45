#include "Tako.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "Player.hpp"

namespace
{
    constexpr float jumpHeight = 32;
    constexpr float jumpPeak = 0.5f;
    constexpr float jumpVelocity = 2 * jumpHeight / jumpPeak;
    constexpr float gravity = -2 * jumpHeight / (jumpPeak * jumpPeak);
    constexpr float worldLeft = -64;
    constexpr float worldRight = 1024;
    constexpr float acceleration = 0.2f;
    constexpr tako::Vector2 playerSpawn(0, 30);
}
struct State
{
    Player player;
    tako::Vector2 cameraPosition;
    tako::Vector2 cameraTarget;
} state;

std::vector<Rect> levelRects;

Rect ToRenderRect(Rect r)
{
    r.x -= r.w/2;
    r.y += r.h/2;
    return r;
}

void AddBlock(Rect block)
{
    Physics::AddCollider(block);
    levelRects.push_back(ToRenderRect(block));
}

void tako::Setup(PixelArtDrawer* drawer)
{
    state.player = {0, 30, 16, 16};
    state.cameraPosition = state.cameraTarget = state.player.position;
    drawer->SetClearColor({"#87CEEB"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    drawer->SetCameraPosition({0, 0});
    Physics::RegisterEntity(&state.player);
    AddBlock({0, -2000, 128, 4000});

    for (int i = 0; i < 4; i++)
    {
        AddBlock(Rect(112 + i * 64.0f, -8, 32, 16));
    }

    for (int i = 0; i < 4; i++)
    {
        AddBlock(Rect(368 + i * 64, 0 + i * 16, 32, 16));
    }

    AddBlock(Rect(624+16+32, -32, 128, 200));
    //AddBlock({60, 8, 32, 16});
    //AddBlock({60 + 64, 8 + 16, 32, 16});
    //AddBlock({60 + 64 + 64, 8 + 16 + 16, 32, 16});
}

void tako::Update(tako::Input* input, float dt)
{
    const float speed = 64;
    float targetVelocity = 0;
    if (input->GetKeyDown(tako::Key::W) && state.player.grounded)
    {
        state.player.velocity.y = jumpVelocity;
    }
    if (input->GetKey(tako::Key::A))
    {
        targetVelocity -= speed;
    }
    if (input->GetKey(tako::Key::D))
    {
        targetVelocity += speed;
    }
    //movement.normalize();
    state.player.velocity.x = acceleration * targetVelocity + (1 - acceleration) * state.player.velocity.x;
    state.player.velocity.y += gravity * dt;
    Physics::Update(dt);


    state.player.position.x = std::max(worldLeft + state.player.size.x / 2, std::min(state.player.position.x, worldRight - state.player.size.x / 2));

    if (state.player.position.y < -400)
    {
        state.player.position = playerSpawn;
        state.player.velocity = {};
    }

    if (tako::mathf::abs(state.cameraTarget.x - state.player.position.x) > 32 || tako::mathf::abs(state.cameraTarget.x - state.cameraPosition.x) > 16)
    {
        auto extents = tako::Graphics->GetCameraViewSize() / 2;
        state.cameraTarget.x = std::max(worldLeft + extents.x, std::min(state.player.position.x, worldRight - extents.x));
    }

    auto cameraYOffset = tako::Graphics->GetCameraViewSize().y / 10;
    if (state.player.grounded || state.player.position.y + cameraYOffset < state.cameraTarget.y)
    {
        state.cameraTarget.y = state.player.position.y + cameraYOffset;
    }

    state.cameraPosition += (state.cameraTarget - state.cameraPosition) * dt * 2;
    //LOG("Player: {} {}", state.player.position.x, state.player.position.y);
}

void DrawEntity(tako::PixelArtDrawer* drawer, const Entity& entity)
{
    auto rect = ToRenderRect(entity.GetRect());
    drawer->DrawRectangle(rect.x, rect.y, rect.w, rect.h, entity.color);
}

void tako::Draw(tako::PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->SetCameraPosition(state.cameraPosition);
    //drawer->DrawRectangle(-1000, 0, 2000, 3000, {"#B5651D"});
    //drawer->DrawRectangle(-32, 8, 8, 8, {"#B5651D"});
    for (auto rect : levelRects)
    {
        drawer->DrawRectangle(rect.x, rect.y, rect.w, rect.h, {"#B5651D"});
    }
    //drawer->DrawRectangle(state.position.x, state.position.y, 16, 16, {100, 0, 255, 255});
    DrawEntity(drawer, state.player);
}

