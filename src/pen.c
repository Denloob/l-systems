#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "pen.h"
#include "utils.h"
#include <string.h>

#define ASCII_CHAR_AMOUNT ('~' + 1)

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

PenCommandRegistry pen_command_registry_create()
{
    PenCommandRegistry registry =
        xmalloc(sizeof(*registry) * ASCII_CHAR_AMOUNT);
    memset(registry, 0, sizeof(*registry) * ASCII_CHAR_AMOUNT);

    return registry;
}

void pen_command_registry_destroy(PenCommandRegistry registry)
{
    free(registry);
}

bool pen_command_registry_add(PenCommandRegistry registry, char ch,
                              PenCommand command)
{
    if (ch < 0 || ch >= ASCII_CHAR_AMOUNT || registry[(int)ch])
        return false;

    registry[(int)ch] = command;
    return true;
}

bool pen_command_registry_remove(PenCommandRegistry registry, char ch)
{
    if (ch < 0 || ch >= ASCII_CHAR_AMOUNT)
        return false;

    registry[(int)ch] = NULL;
    return true;
}

bool pen_command_registry_set(PenCommandRegistry registry, char ch,
                              PenCommand command)
{
    if (ch < 0 || ch >= ASCII_CHAR_AMOUNT)
        return false;

    registry[(int)ch] = command;
    return true;
}

void pen_commands_execute(Pen *pen, const PenCommandRegistry registry,
                          const char *commands)
{
    for (const char *command = commands; *command; command++)
    {
        PenCommand pen_command = registry[(int)*command];
        if (pen_command)
        {
            pen_command(pen);
        }
    }
}
