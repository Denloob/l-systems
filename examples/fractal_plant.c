#include "SDL.h"
#include "config.h"
#include "l_system.h"
#include "pen.h"
#include "utils.h"

#define ROTATION_ANGLE 0.4363323129985824 /* 25 degrees in radians */

void forward(Pen *pen)
{
    pen_move_forward(pen, 3);
}

void right(Pen *pen)
{
    pen->rotation_rad += ROTATION_ANGLE;
}

void left(Pen *pen)
{
    pen->rotation_rad -= ROTATION_ANGLE;
}

Config *config_create(SDL_Renderer *renderer)
{
    Rule rules[] = {
        {.type = RULE_TYPE_STRING, 'F', .to.string="FF"},
        {.type = RULE_TYPE_STRING, 'X', .to.string="F+[[X]-X]-F[-FX]+X"},
    };
    Config *config = xmalloc(sizeof(*config) + sizeof(rules));
    *config = (Config){
        .starting_string = "X",
        .iterations = 6,
        .background_color = {0, 0, 0, SDL_ALPHA_OPAQUE},
        .pen =
            {
                .color = {220, 250, 240, SDL_ALPHA_OPAQUE},
                .width = 1,
                .pos = {.x = 0, .y = 500},
                .rotation_rad = -M_PI_4,
                .down = true,
                .renderer = renderer,
            },

        .registry = pen_command_registry_create(),
        .rules_size = sizeof(rules) / sizeof(rules[0]),
    };

    memcpy(config->rules, rules, sizeof(rules));

    pen_command_registry_add(config->registry, 'F', forward);
    pen_command_registry_add(config->registry, '+', left);
    pen_command_registry_add(config->registry, '-', right);
    pen_command_registry_add(config->registry, '[', pen_state_save);
    pen_command_registry_add(config->registry, ']', pen_state_restore);

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
