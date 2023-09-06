#pragma once

#include "SDL.h"
#include <stdbool.h>

typedef struct Pen
{
    SDL_Renderer *renderer;
    SDL_FPoint pos;
    float rotation_rad; /* Pen rotation in radians.
                           0 is to the left, + is up. */
    SDL_Color color;
    bool down;
} Pen;

/**
 * @brief Moves the pen forward. If it's down, will draw a line.
 *
 * @param amount The amount by which to move the pen forward.
 */
void pen_move_forward(Pen *pen, float amount);
