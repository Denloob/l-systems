#include "SDL.h"
#include "config.h"
#include "l_system.h"
#include "pen.h"
#include "utils.h"

#define PI_x_2_3 2.094395102393195316 /* pi * 2/3 */

void forward(Pen *pen)
{
    pen_move_forward(pen, 7);
}

void right(Pen *pen)
{
    pen->rotation_rad += PI_x_2_3;
}

void left(Pen *pen)
{
    pen->rotation_rad -= PI_x_2_3;
}

Config *config_create(SDL_Renderer *renderer)
{
    Rule rules[] = {
        {'F', "F+G-F-G+F"},
        {'G', "GG"},
    };
    Config *config = xmalloc(sizeof(*config) + sizeof(rules));
    *config = (Config){
        .starting_string = "F+F+F",
        .iterations = 6,
        .background_color = {0, 0, 0, SDL_ALPHA_OPAQUE},
        .pen =
            {
                .color = {255, 255, 255, SDL_ALPHA_OPAQUE},
                .pos = {.x = 25, .y = 450},
                .width = 1,
                .rotation_rad = 0,
                .down = true,
                .renderer = renderer,
            },

        .registry = pen_command_registry_create(),
        .rules_size = sizeof(rules) / sizeof(rules[0]),
    };

    memcpy(config->rules, rules, sizeof(rules));

    pen_command_registry_add(config->registry, 'F', forward);
    pen_command_registry_add(config->registry, 'G', forward);
    pen_command_registry_add(config->registry, '+', left);
    pen_command_registry_add(config->registry, '-', right);

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
