#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "pen.h"

void pen_move_forward(Pen *pen, float amount)
{
    SDL_FPoint dest = {.x = amount * cos(pen->rotation_rad) + pen->pos.x,
                       .y = amount * sin(pen->rotation_rad) + pen->pos.y};

    if (pen->down)
    {
        SDL_Color color = pen->color;
        lineRGBA(pen->renderer, pen->pos.x, pen->pos.y, dest.x, dest.y, color.r,
                 color.g, color.b, color.a);
    }

    pen->pos = dest;
}
