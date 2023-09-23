#include "SDL.h"
#include "SDL2_framerate.h"
#include "SDL_stdinc.h"
#include "config.h"
#include "l_system.h"
#include "pen.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>

#ifndef PEN_COMMAND_WITH_DATA
_Static_assert(0, "Please define PEN_COMMAND_WITH_DATA in pen.h");
#endif

#define ROTATION_RAD(rand_val)                                                 \
    (M_PI_4 / 2.5 - M_PI / 180 * (rand_val % 26 - 13))
#define MAX_LENGTH 3

#define MAX_TREE_SIZE 1ULL << 29 // 0.5 GB

typedef struct CurveElement
{
    char ch;
    bool done;
    double max;
    double current;
    double data;
} CurveElement;

typedef struct CurveVecHeader
{
    size_t len;
    size_t allocated;
    CurveElement arr[];
} CurveVecHeader;

typedef CurveElement *Curve;

static int depth = 0;
static SDL_Color leaf_color_a = {114, 107, 118, SDL_ALPHA_OPAQUE};
static SDL_Color leaf_color_b = {124, 107, 148, SDL_ALPHA_OPAQUE};
static SDL_Color leaf_color_c = {74, 87, 98, SDL_ALPHA_OPAQUE};

CurveVecHeader *curve_vec_get_header(Curve curve)
{
    return &((CurveVecHeader *)curve)[-1];
}

size_t curve_vec_len(Curve curve)
{
    return curve_vec_get_header(curve)->len;
}

CurveElement *curve_vec_end(Curve curve)
{
    return curve + curve_vec_len(curve);
}

Curve curve_vec_create()
{
    CurveVecHeader *h = xmalloc(sizeof(CurveVecHeader));
    h->len = 0;
    h->allocated = 0;

    return h->arr;
}

void curve_vec_destroy(Curve curve)
{
    free(curve_vec_get_header(curve));
}

void curve_vec_realloc(CurveVecHeader **h_ptr)
{
    CurveVecHeader *h = *h_ptr;

    h->allocated *= 2;
    h->allocated = h->allocated ?: 1;

    *h_ptr = xrealloc(h, sizeof(*h) + sizeof(*h->arr) * h->allocated);
}

void curve_vec_insert_vec(Curve *curve_ptr, size_t index, Curve other)
{
    CurveVecHeader *h = curve_vec_get_header(*curve_ptr);
    CurveVecHeader *h_other = curve_vec_get_header(other);

    while (h->allocated < h_other->len + h->len)
    {
        curve_vec_realloc(&h);
        *curve_ptr = h->arr;
    }

    memmove(h->arr + index + h_other->len, h->arr + index,
            sizeof(*h->arr) * (h->len - index));
    memmove(h->arr + index, h_other->arr, sizeof(*h_other->arr) * h_other->len);

    h->len += h_other->len;
}

void curve_vec_insert(Curve *curve_ptr, size_t index, char ch, bool done)
{
    CurveVecHeader *h = curve_vec_get_header(*curve_ptr);
    CurveElement el = {
        .ch = ch,
        .max = MAX_LENGTH,
        .done = done,
        .data = rand(),
    };

    if (h->allocated <= h->len)
    {
        curve_vec_realloc(&h);
        *curve_ptr = h->arr;
    }

    memmove(h->arr + index + 1, h->arr + index,
            sizeof(*h->arr) * (h->len - index));

    h->arr[index] = el;

    h->len++;
}

void curve_vec_append(Curve *curve_ptr, char ch, bool done)
{
    curve_vec_insert(curve_ptr, curve_vec_len(*curve_ptr), ch, done);
}

void curve_vec_remove(Curve curve, size_t index)
{
    CurveVecHeader *h = curve_vec_get_header(curve);
    memmove(h->arr + index, h->arr + index + 1,
            sizeof(*h->arr) * (h->len - index - 1));
    h->len--;
}

void leaf(Pen *pen, int data)
{
    double prev_width = pen->width;
    pen->width = 3;

    switch (data % 3)
    {
        case 0:
            pen->color = leaf_color_a;
            break;
        case 1:
            pen->color = leaf_color_b;
            break;
        case 2:
            pen->color = leaf_color_c;
            break;
    }

    pen_move_forward(pen, 1);
    pen->width = prev_width;
}

void push(Pen *pen, int)
{
    pen_state_save(pen);
    pen->width = SDL_max(pen->width * 0.7, 1);
}

void pop(Pen *pen, int)
{
    pen_state_restore(pen);
}

void internode(Pen *pen, int)
{
    pen->color = (SDL_Color){0, 0, 0, SDL_ALPHA_OPAQUE};
    pen_move_forward(pen, 3);
}

void internode_pen_up(Pen *pen, int _)
{
    pen->down = false;
    internode(pen, _);
    pen->down = true;
}

void right(Pen *pen, int data)
{
    pen->rotation_rad += ROTATION_RAD(data);
}

void left(Pen *pen, int data)
{
    pen->rotation_rad -= ROTATION_RAD(data);
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
    {
        if (rand() % 2)
            return "G[+GL]";
        else
            return "G[-GL]";
    }
    else
    {
        return "G";
    }
}

Config *config_create(SDL_Renderer *renderer)
{
    srand(time(NULL));
    Rule rules[] = {
        {.type = RULE_TYPE_STRING,  'I', .to.string = "FI"},
        {.type = RULE_TYPE_STRING,  'L', .to.string = "I[-FL][+L]"},
        {.type = RULE_TYPE_FUNCTION, 'F', .to.func = internode_rule},
        {.type = RULE_TYPE_FUNCTION, '[', .to.func = increase_depth},
        {.type = RULE_TYPE_FUNCTION, ']', .to.func = decrease_depth},
    };

    SDL_Window *window = SDL_RenderGetWindow(renderer);
    SDL_Point window_size = {0};
    SDL_GetWindowSize(window, &window_size.x, &window_size.y);

    Config *config = xmalloc(sizeof(*config) + sizeof(rules));
    *config = (Config){
        .starting_string = "GGGGL",
        .background_color = {255, 255, 255, SDL_ALPHA_OPAQUE},
        .pen =
            {
                .pos = {window_size.x / 2.0, window_size.y},
                .width = 12,
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

Curve get_replacement(Config *config, char ch)
{
    Curve curve = curve_vec_create();

    for (const Rule *rule = config->rules;
         rule < config->rules + config->rules_size; rule++)
    {
        if (rule->from != ch)
            continue;

        const char *target = NULL;

        switch (rule->type)
        {
            case RULE_TYPE_STRING:
                target = rule->to.string;
                break;
            case RULE_TYPE_FUNCTION:
                target = rule->to.func();
                break;
        }

        for (const char *ch = target; *ch; ch++)
            curve_vec_append(&curve, *ch, false);

        return curve;
    }

    curve_vec_append(&curve, ch, true);

    return curve;
}

Curve create_initial_curve(Config *config)
{
    Curve curve = curve_vec_create();
    for (char *ch = config->starting_string; *ch; ch++)
        curve_vec_append(&curve, *ch, false);

    return curve;
}

void execute_pen_commands(Config *config, Curve curve)
{
    for (size_t idx = 0; idx < curve_vec_len(curve); idx++)
    {
        PenCommand pen_command = config->registry[(int)curve[idx].ch];
        if (pen_command)
        {
            pen_command(&config->pen, curve[idx].data);
        }
    }
}

void config_execute(Config *config)
{
    Curve curve = create_initial_curve(config);

    SDL_FPoint starting_pos = config->pen.pos;
    double starting_rotation = config->pen.rotation_rad;

    bool done = false;
    while (!done)
    {
        for (size_t idx = 0; idx < curve_vec_len(curve); idx++)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    done = true;
            }

            if (done)
                break;

            CurveElement *curve_element = &curve[idx];
            if (curve_element->done)
            {
                curve_element->current = curve_element->max;
                continue;
            }

            if (curve_element->current < curve_element->max)
            {
                curve_element->current++;
                continue;
            }

            Curve replacement = get_replacement(config, curve_element->ch);
            curve_vec_remove(curve, idx);
            curve_vec_insert_vec(&curve, idx, replacement);
            curve_vec_destroy(replacement);

            SDL_Color c = config->background_color;
            SDL_SetRenderDrawColor(config->pen.renderer, c.r, c.g, c.b, c.a);
            SDL_RenderClear(config->pen.renderer);

            execute_pen_commands(config, curve);

            config->pen.pos = starting_pos;
            config->pen.rotation_rad = starting_rotation;

            SDL_RenderPresent(config->pen.renderer);
        }

        if (curve_vec_len(curve) * sizeof(*curve) > MAX_TREE_SIZE)
        {
            curve_vec_destroy(curve);
            curve = create_initial_curve(config);
        }
    }

    curve_vec_destroy(curve);
}
