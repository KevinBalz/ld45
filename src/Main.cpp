#include "Tako.hpp"

struct State
{
    tako::Vector2 position;
} state;

void tako::Setup(PixelArtDrawer* drawer)
{
}

void tako::Update(tako::Input* input, float dt)
{
    const float speed = 10;
    tako::Vector2 movement;
    if (input->GetKey(tako::Key::W))
    {
        movement.y--;
    }
    if (input->GetKey(tako::Key::A))
    {
        movement.x--;
    }
    if (input->GetKey(tako::Key::S))
    {
        movement.y++;
    }
    if (input->GetKey(tako::Key::D))
    {
        movement.x++;
    }
    //movement.normalize();
    state.position += movement * speed * dt;
}

void tako::Draw(PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->DrawRectangle(state.position.x, state.position.y, 100, 100, {100, 0, 255, 255});
}

