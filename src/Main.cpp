#include "Tako.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "Player.hpp"
#include "Level.hpp"
#include "Powerup.hpp"
#include <algorithm>
#include <emscripten/html5.h>

namespace
{
    constexpr float jumpHeight = 32;
    constexpr float jumpPeak = 0.5f;
    constexpr float jumpVelocity = 2 * jumpHeight / jumpPeak;
    constexpr float gravity = -2 * jumpHeight / (jumpPeak * jumpPeak);
    constexpr float acceleration = 0.2f;
}
struct State
{
    Player player;
    tako::Vector2 cameraPosition;
    tako::Vector2 cameraTarget;
    tako::Vector2 playerSpawn;
    std::vector<Powerup> powerups;
    Level level;
} state;

tako::Vector2 FitCameraTargetIntoBounds(tako::Vector2 target, Rect bounds, tako::Vector2 extents)
{
    tako::Vector2 newTarget;
    newTarget.x = std::max(bounds.x + extents.x, std::min(target.x, bounds.w - extents.x));
    newTarget.y = std::max(bounds.y + extents.y, std::min(target.y, bounds.h - extents.y));

    return newTarget;
}

void tako::Setup(PixelArtDrawer* drawer)
{
    auto playerTex = drawer->CreateTexture(tako::Bitmap::FromFile("/Player.png"));
    auto powerupTex = drawer->CreateTexture(tako::Bitmap::FromFile("/Powerup.png"));
    drawer->SetClearColor({"#608FCF"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    auto tex = drawer->CreateTexture(tako::Bitmap::FromFile("/Tileset.png"));
    std::array<tako::Sprite*, 12> tileset;
    for (int i = 0; i < 12; i++)
    {
        int x = i % 3;
        int y = i / 3;
        tileset[i] = drawer->CreateSprite(tex, x * 16, y * 16, 16, 16);
    }
    state.level.LoadLevel("/Level.txt", tileset, [&](char c, float x, float y)
    {
        if (c == 'P')
        {
            state.playerSpawn = {x, y};
        }
        if (c == 'W')
        {
            state.powerups.emplace_back(x, y, 16, 16, powerupTex, [](Player& player)
            {
                player.gainedWalk = true;
            });
        }
        if (c == 'J')
        {
            state.powerups.emplace_back(x, y, 16, 16, powerupTex, [](Player& player)
            {
                player.gainedJump = true;
            });
        }
        if (c == 'D')
        {
            state.powerups.emplace_back(x, y, 16, 16, powerupTex, [](Player& player)
            {
                player.gainedDoubleJump = true;
            });
        }
    });
    auto spawn = state.playerSpawn;
    state.player = {spawn.x, spawn.y, 12, 12, playerTex};
    state.cameraPosition = state.cameraTarget = FitCameraTargetIntoBounds(state.player.position, state.level.GetBounds(), tako::Graphics->GetCameraViewSize() / 2);
    Physics::RegisterEntity(&state.player);
}

void tako::Update(tako::Input* input, float dt)
{
    const float speed = 64;
    float targetVelocity = 0;

    if (input->GetKeyDown(tako::Key::Enter))
    {
        emscripten_request_fullscreen(0, true);
    }

    state.powerups.erase(std::remove_if(state.powerups.begin(), state.powerups.end(), [&](Powerup& power)
    {
        if (Rect::Overlap(state.player.GetRect(), power.GetRect()))
        {
            power.PowerUp(state.player);
            return true;
        }

        return false;
    }), state.powerups.end());

    if (state.player.grounded)
    {
        state.player.canDoubleJump = true;
    }

    if ((input->GetKeyDown(tako::Key::W) || input->GetKeyDown(tako::Key::Up) || input->GetKeyDown(tako::Key::Space)) && state.player.gainedJump)
    {
        bool canJump = false;
        if (state.player.grounded)
        {
            canJump = true;
        }
        else if (state.player.gainedDoubleJump && state.player.canDoubleJump)
        {
            state.player.canDoubleJump = false;
            canJump = true;
        }

        if (canJump)
        {
            state.player.velocity.y = jumpVelocity;
        }

    }
    if ((input->GetKey(tako::Key::A) || input->GetKey(tako::Key::Left))&& state.player.gainedWalk)
    {
        targetVelocity -= speed;
    }
    if ((input->GetKey(tako::Key::D) || input->GetKey(tako::Key::Right)) && state.player.gainedWalk)
    {
        targetVelocity += speed;
    }

    state.player.velocity.x = acceleration * targetVelocity + (1 - acceleration) * state.player.velocity.x;
    state.player.velocity.y += gravity * dt;
    Physics::Update(dt);

    if (state.player.position.y < -400)
    {
        state.player.position = state.playerSpawn;
        state.player.velocity = {};
    }

    auto bounds = state.level.GetBounds();
    auto extents = tako::Graphics->GetCameraViewSize() / 2;
    if (tako::mathf::abs(state.cameraTarget.x - state.player.position.x) > 32 || tako::mathf::abs(state.cameraTarget.x - state.cameraPosition.x) > 16)
    {
        state.cameraTarget.x = state.player.position.x;
    }

    auto cameraYOffset = tako::Graphics->GetCameraViewSize().y / 10;
    if (state.player.grounded || state.player.position.y + cameraYOffset < state.cameraTarget.y)
    {
        state.cameraTarget.y = state.player.position.y + cameraYOffset;
    }

    state.cameraTarget = FitCameraTargetIntoBounds(state.cameraTarget, bounds, extents);
    state.cameraPosition += (state.cameraTarget - state.cameraPosition) * dt * 2;
}

void DrawEntity(tako::PixelArtDrawer* drawer, const Entity& entity)
{
    auto rect = ToRenderRect(entity.GetRect());
    drawer->DrawImage(rect.x, rect.y, rect.w, rect.h, entity.image);
}

void tako::Draw(tako::PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->SetCameraPosition(state.cameraPosition);
    state.level.Render(drawer);
    for (auto powerup: state.powerups)
    {
        DrawEntity(drawer, powerup);
    }
    DrawEntity(drawer, state.player);
}

