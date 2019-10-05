#include "Tako.hpp"

namespace {
    constexpr float gravity = 16;
}
struct State
{
    tako::Vector2 position;
    tako::Vector2 cameraPosition;
} state;

void tako::Setup(PixelArtDrawer* drawer)
{
    state.position = Vector2(0, 30);
    state.cameraPosition = state.position;
    drawer->SetClearColor({"#87CEEB"});
    drawer->SetTargetSize(240, 135);
    drawer->AutoScale();
    drawer->SetCameraPosition({0, 0});
}

void tako::Update(tako::Input* input, float dt)
{
    const float speed = 16;
    tako::Vector2 movement;
    if (input->GetKey(tako::Key::W))
    {
        movement.y++;
    }
    if (input->GetKey(tako::Key::S))
    {
        movement.y--;
    }
    if (input->GetKey(tako::Key::A))
    {
        movement.x--;
    }
    if (input->GetKey(tako::Key::D))
    {
        movement.x++;
    }
    //movement.normalize();
    state.position += movement * speed * dt;
    //state.position.y -= gravity * dt;
    state.cameraPosition += (state.position + Vector2(8, -8) - state.cameraPosition) * dt;
}

void tako::Draw(PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->SetCameraPosition(state.cameraPosition);
    drawer->DrawRectangle(-1000, 0, 2000, 2000, {"#B5651D"});
    drawer->DrawRectangle(state.position.x, state.position.y, 16, 16, {100, 0, 255, 255});
}

