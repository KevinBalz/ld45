#include "Tako.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "Player.hpp"

namespace
{
    constexpr float jumpHeight = 32;
    constexpr float jumpPeak = 0.4f;
    constexpr float jumpVelocity = 2 * jumpHeight / jumpPeak;
    constexpr float gravity = -2 * jumpHeight / (jumpPeak * jumpPeak);
    constexpr float worldLeft = -256;
    constexpr float worldRight = 256;
    constexpr float acceleration = 0.2f;
}
struct State
{
    Player player;
    tako::Vector2 cameraPosition;
    tako::Vector2 cameraTarget;
} state;

void tako::Setup(PixelArtDrawer* drawer)
{
    state.player = {0, 30, 16, 16};
    state.cameraPosition = state.cameraTarget = state.player.position;
    drawer->SetClearColor({"#87CEEB"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    drawer->SetCameraPosition({0, 0});
    Physics::RegisterEntity(&state.player);
    Physics::AddCollider({0, -2000, 2000, 4000});
    Physics::AddCollider({-28, 4, 8, 8});
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


    if (tako::mathf::abs(state.cameraTarget.x - state.player.position.x) > 32 || tako::mathf::abs(state.cameraTarget.x - state.cameraPosition.x) > 16)
    {
        auto extents = tako::Graphics->GetCameraViewSize() / 2;
        state.cameraTarget.x = std::max(worldLeft + extents.x, std::min(state.player.position.x, worldRight - extents.x));
    }

    state.cameraPosition += (state.cameraTarget - state.cameraPosition) * dt;
    LOG("Player: {} {}", state.player.position.x, state.player.position.y);
}

void DrawEntity(tako::PixelArtDrawer* drawer, const Entity& entity)
{
    auto rect = entity.GetRenderRect();
    drawer->DrawRectangle(rect.x, rect.y, rect.w, rect.h, entity.color);
}

void tako::Draw(tako::PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->SetCameraPosition(state.cameraPosition);
    drawer->DrawRectangle(-1000, 0, 2000, 3000, {"#B5651D"});
    drawer->DrawRectangle(-32, 8, 8, 8, {"#B5651D"});
    //drawer->DrawRectangle(state.position.x, state.position.y, 16, 16, {100, 0, 255, 255});
    DrawEntity(drawer, state.player);
}

