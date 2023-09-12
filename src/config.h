#pragma once

#include "l_system.h"
#include "pen.h"

typedef struct Config
{
    PenCommandRegistry registry;
    char *starting_string;
    size_t rules_size;
    uint iterations;
    SDL_Color background_color;
    Pen pen;
    Rule rules[];
} Config;

/**
 * @brief Creates a config for the l-system.
 *
 * @param renderer Renderer on which to draw the l-system when executing.
 * @return Pointer to the l-system config.
 *
 * @see config_destroy
 * @see config_execute
 */
Config *config_create(SDL_Renderer *renderer);

/**
 * @brief Executes l-system described by the given config.
 *
 * @param config The config to use.
 *
 * @see config_create
 * @see config_destroy
 */
void config_execute(Config *config);

/**
 * @brief Destroys the given config.
 *
 * @param config The config to destroy.
 *
 * @see config_create
 * @see config_execute
 */
void config_destroy(Config *config);
