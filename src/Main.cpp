#include "Tako.hpp"

void tako::Setup(PixelArtDrawer* drawer)
{
}

void tako::Update()
{
}

void tako::Draw(PixelArtDrawer* drawer)
{
    drawer->Clear();
    drawer->DrawRectangle(0, 0, 100, 100, {100, 0, 255, 255});
}

