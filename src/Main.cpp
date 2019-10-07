#include "Tako.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "Player.hpp"
#include "Level.hpp"

namespace
{
    constexpr float jumpHeight = 32;
    constexpr float jumpPeak = 0.5f;
    constexpr float jumpVelocity = 2 * jumpHeight / jumpPeak;
    constexpr float gravity = -2 * jumpHeight / (jumpPeak * jumpPeak);
    //constexpr float worldLeft = -64;
    //constexpr float worldRight = 1024;
    constexpr float acceleration = 0.2f;
}
struct State
{
    Player player;
    tako::Vector2 cameraPosition;
    tako::Vector2 cameraTarget;
    Level level;
} state;

std::array<tako::Sprite*, 12> tileset;
std::vector<Rect> levelRects;

void AddBlock(Rect block)
{
    Physics::AddCollider(block);
    levelRects.push_back(ToRenderRect(block));
}

void tako::Setup(PixelArtDrawer* drawer)
{

    auto tex = drawer->CreateTexture(tako::Bitmap::FromFile("/Tileset.png"));
    for (int i = 0; i < 12; i++)
    {
        int x = i % 3;
        int y = i / 3;
        tileset[i] = drawer->CreateSprite(tex, x * 16, y * 16, 16, 16);
    }
    state.level.LoadLevel("/Level.txt", tileset);
    //state.player.size = {16, 16};
    //state.player.position = state.level.GetSpawn();
    auto spawn = state.level.GetSpawn();
    state.player = {spawn.x, spawn.y, 16, 16};
    //state.player.position = {200, 60};
    LOG("spawn: {} {}", state.player.position.x, state.player.position.y);
    state.cameraPosition = state.cameraTarget = state.player.position;
    drawer->SetClearColor({"#608FCF"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    drawer->SetCameraPosition({0, 0});
    Physics::RegisterEntity(&state.player);
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


    //state.player.position.x = std::max(bounds.x + state.player.size.x / 2, std::min(state.player.position.x, bounds.w - state.player.size.x / 2));
    //state.player.position.y = std::max(bounds.y + state.player.size.y / 2, std::min(state.player.position.y, bounds.h - state.player.size.y / 2));

    if (state.player.position.y < -400)
    {
        state.player.position = state.level.GetSpawn();
        state.player.velocity = {};
    }

    auto bounds = state.level.GetBounds();
    auto extents = tako::Graphics->GetCameraViewSize() / 2;
    if (tako::mathf::abs(state.cameraTarget.x - state.player.position.x) > 32 || tako::mathf::abs(state.cameraTarget.x - state.cameraPosition.x) > 16)
    {
        state.cameraTarget.x = std::max(bounds.x + extents.x, std::min(state.player.position.x, bounds.w - extents.x));
    }

    auto cameraYOffset = tako::Graphics->GetCameraViewSize().y / 10;
    if (state.player.grounded || state.player.position.y + cameraYOffset < state.cameraTarget.y)
    {
        state.cameraTarget.y = std::max(bounds.y + extents.y, std::min(state.player.position.y + cameraYOffset, bounds.h - extents.y));
    }

    state.cameraPosition += (state.cameraTarget - state.cameraPosition) * dt * 2;
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
    state.level.Render(drawer);
    DrawEntity(drawer, state.player);
}

