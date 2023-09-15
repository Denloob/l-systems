#include "SDL.h"
#include "SDL_stdinc.h"
#include "config.h"
#include "l_system.h"
#include "pen.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>

#define ROTATION_RAD (M_PI_4 / 2.5 - M_PI / 180 * (rand() % 26 - 13))

static int depth = 0;

void leaf(Pen *pen)
{
    pen->width = 3;

    switch (rand() % 3)
    {
        case 0:
            pen->color = (SDL_Color){114, 107, 118, SDL_ALPHA_OPAQUE};
            break;
        case 1:
            pen->color = (SDL_Color){124, 107, 148, SDL_ALPHA_OPAQUE};
            break;
        case 2:
            pen->color = (SDL_Color){74, 87, 98, SDL_ALPHA_OPAQUE};
            break;
    }

    pen_move_forward(pen, 1);
}

void push(Pen *pen)
{
    pen_state_save(pen);
    pen->width = SDL_max(pen->width * 0.7, 1);
}

void pop(Pen *pen)
{
    pen_state_restore(pen);
}
void internode(Pen *pen)
{
    pen->color = (SDL_Color){0, 0, 0, SDL_ALPHA_OPAQUE};
    pen_move_forward(pen, 3);
}

void right(Pen *pen)
{
    pen->rotation_rad += ROTATION_RAD;
}

void left(Pen *pen)
{
    pen->rotation_rad -= ROTATION_RAD;
}

void random_turn(Pen *pen)
{
    if (rand() % 2)
        right(pen);
    else
        left(pen);
}

const char *increase_depth()
{
    depth++;
    return "[";
}

const char *decrease_depth()
{
    depth--;
    return "]";
}

const char *internode_rule()
{
    if (rand() % 100 < 45 && depth > 3)
        return "G[^GL]";
    else
        return "G";
}

Config *config_create(SDL_Renderer *renderer)
{
    srand(time(NULL));
    Rule rules[] = {
        {.type = RULE_TYPE_STRING,   'I', .to.string = "FI"},
        {.type = RULE_TYPE_STRING,   'L', .to.string = "I[-FL][+L]"},
        {.type = RULE_TYPE_FUNCTION, 'F', .to.func = internode_rule},
        {.type = RULE_TYPE_FUNCTION, '[', .to.func = increase_depth},
        {.type = RULE_TYPE_FUNCTION, ']', .to.func = decrease_depth},
    };
    Config *config = xmalloc(sizeof(*config) + sizeof(rules));
    *config = (Config){
        .starting_string = "GGGGL",
        .iterations = 13,
        .background_color = {255, 255, 255, SDL_ALPHA_OPAQUE},
        .pen =
            {
                .pos = {.x = 250, .y = 500},
                .width = 14,
                .rotation_rad = -M_PI_2,
                .down = true,
                .renderer = renderer,
            },

        .registry = pen_command_registry_create(),
        .rules_size = sizeof(rules) / sizeof(rules[0]),
    };

    memcpy(config->rules, rules, sizeof(rules));

    pen_command_registry_add(config->registry, 'L', leaf);
    pen_command_registry_add(config->registry, 'I', internode);
    pen_command_registry_add(config->registry, 'F', internode);
    pen_command_registry_add(config->registry, 'G', internode);
    pen_command_registry_add(config->registry, '-', left);
    pen_command_registry_add(config->registry, '^', random_turn);
    pen_command_registry_add(config->registry, '+', right);
    pen_command_registry_add(config->registry, '[', push);
    pen_command_registry_add(config->registry, ']', pop);

    return config;
}

void config_destroy(Config *config)
{
    pen_command_registry_destroy(config->registry);
    free(config);
}

void config_execute(Config *config)
{
    char *prev_curve = NULL;
    char *curve = strdup(config->starting_string);
    for (uint i = 0; i < config->iterations; i++)
    {
        prev_curve = curve;
        curve =
            l_system_apply_rules(prev_curve, config->rules, config->rules_size);
        free(prev_curve);
    }

    SDL_Color c = config->background_color;
    SDL_SetRenderDrawColor(config->pen.renderer, c.r, c.g, c.b, c.a);
    SDL_RenderClear(config->pen.renderer);

    pen_commands_execute(&config->pen, config->registry, curve);
    free(curve);

    SDL_RenderPresent(config->pen.renderer);

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                done = true;
        }
    }
}
