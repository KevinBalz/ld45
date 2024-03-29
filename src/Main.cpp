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
    float fadeValue = 255;
    float fadeTarget = 0;
    bool gameCompleted = false;
    Level level;
    tako::AudioClip* jumpClip;
    tako::AudioClip* landClip;
    tako::AudioClip* pickupClip;
    tako::AudioClip* ripClip;
} state;

tako::Texture* gameWon;

tako::Vector2 FitCameraTargetIntoBounds(tako::Vector2 target, Rect bounds, tako::Vector2 extents)
{
    tako::Vector2 newTarget;
    newTarget.x = std::max(bounds.x + extents.x, std::min(target.x, bounds.w - extents.x));
    newTarget.y = std::max(bounds.y + extents.y, std::min(target.y, bounds.h - extents.y));

    return newTarget;
}

void tako::Setup(PixelArtDrawer* drawer)
{
    state.jumpClip = new AudioClip("/Jump.wav");
    state.landClip = new AudioClip("/Land.wav");
    state.pickupClip = new AudioClip("/Pickup.wav");
    state.ripClip = new AudioClip("/RIP.wav");
    gameWon = drawer->CreateTexture(tako::Bitmap::FromFile("/gamewon.png"));
    auto playerTex = drawer->CreateTexture(tako::Bitmap::FromFile("/Player.png"));
    auto powerupTex = drawer->CreateTexture(tako::Bitmap::FromFile("/Powerup.png"));
    drawer->SetClearColor({"#608FCF"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    state.level.LoadLevel("/Level.txt", drawer, [&](char c, float x, float y)
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
            tako::Audio::Play(*state.pickupClip);
            return true;
        }

        return false;
    }), state.powerups.end());

    static bool wasGrounded = state.player.grounded;
    if (state.player.grounded)
    {
        state.player.canDoubleJump = true;
        if (!wasGrounded)
        {
            tako::Audio::Play(*state.landClip);
        }
    }
    wasGrounded = state.player.grounded;

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
            tako::Audio::Play(*state.jumpClip);
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

    auto bounds = state.level.GetBounds();
    auto extents = tako::Graphics->GetCameraViewSize() / 2;
    if (state.player.position.y < -5000)
    {
        state.player.position = state.playerSpawn;
        state.cameraPosition = state.cameraTarget = FitCameraTargetIntoBounds(state.player.position, bounds, extents);
        state.player.velocity = {};
        state.player.rip = false;
        
        if (!state.gameCompleted)
        {
            state.fadeTarget = 0;
        }
    }
    else if (state.player.position.y < -32)
    {
        state.fadeTarget = 255;
        if (!state.player.rip)
        {
            tako::Audio::Play(*state.ripClip);
            state.player.rip = true;
        }
        
    }
    if (state.player.position.x > bounds.w)
    {
        state.fadeTarget = 255;
        state.gameCompleted = true;
    }
    
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
    
    if (state.fadeValue != state.fadeTarget)
    {
        state.fadeValue = state.fadeTarget < state.fadeValue ?
                std::max(state.fadeTarget, state.fadeValue - 100 * dt) :
                std::min(state.fadeTarget, state.fadeValue + 100 * dt);
    }
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
    if (state.fadeValue > 0)
    {
        auto view = drawer->GetCameraViewSize();
        drawer->DrawRectangle(state.cameraPosition.x - view.x / 2, state.cameraPosition.y + view.y / 2, view.x, view.y, {0, 0, 0, static_cast<tako::U8>(state.fadeValue)});
    }
    if (state.gameCompleted && state.fadeValue >= 255)
    {
        drawer->DrawImage(state.cameraPosition.x - 53, state.cameraPosition.y + 3, 106, 7, gameWon);
    }
}

