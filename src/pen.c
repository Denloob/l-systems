#include "SDL.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL_assert.h"
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
        thickLineRGBA(pen->renderer, pen->pos.x, pen->pos.y, dest.x, dest.y,
                      pen->width, color.r, color.g, color.b, color.a);
    }

    pen->pos = dest;
}

void pen_state_save(Pen *pen)
{
    PenState *state = xmalloc(sizeof(*state));
    state->pos = pen->pos;
    state->width = pen->width;
    state->rotation_rad = pen->rotation_rad;

    stack_push(&pen->position_stack, state);
}

void pen_position_restore(Pen *pen)
{
    PenState *state = stack_pop(&pen->position_stack);
    SDL_assert_always(state != NULL && "Restoring position from empty stack.");
    pen->pos = state->pos;
    pen->width = state->width;
    pen->rotation_rad = state->rotation_rad;

    free(state);
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
