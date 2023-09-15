#pragma once

#include "SDL.h"
#include "stack.h"
#include <stdbool.h>

typedef struct PenPosition
{
    int width;
    SDL_FPoint pos;
    double rotation_rad;
} PenState;

typedef struct Pen
{
    Stack *position_stack; /* Stack of PenPosition.
                              NULL means empty.*/
    SDL_Renderer *renderer;
    SDL_FPoint pos;
    double width;
    double rotation_rad; /* Pen rotation in radians.
                            0 is to the right.
                            Adding will rotate right.
                            Subtracting will rotate left.*/
    SDL_Color color;
    bool down;
} Pen;

typedef void (*PenCommand)(Pen *pen);

typedef PenCommand *PenCommandRegistry;

/**
 * @brief Moves the pen forward. If it's down, will draw a line.
 *
 * @param amount The amount by which to move the pen forward.
 */
void pen_move_forward(Pen *pen, float amount);

/**
 * @brief Saves the current pen position onto the pen position stack.
 */
void pen_state_save(Pen *pen);

/**
 * @brief Restores the pen position from the pen position stack.
 */
void pen_state_restore(Pen *pen);

/**
 * @brief Creates a registry for pen commands.
 *
 * @return Pointer to the registry.
 *
 * @see pen_command_registry_destroy
 */
PenCommandRegistry pen_command_registry_create();

/**
 * @brief Destroys the given registry.
 */
void pen_command_registry_destroy(PenCommandRegistry registry);

/**
 * @brief Adds a pen command to the registry only if the char is not already
 *          registered.
 *
 * @param ch The char to which the command will be registered.
 * @param command The command to register.
 *
 * @return Whether the command was added.
 *
 * @see pen_command_registry_remove
 * @see pen_command_registry_set
 */
bool pen_command_registry_add(PenCommandRegistry registry, char ch,
                              PenCommand command);

/**
 * @brief Removes a pen command from the registry.
 *
 * @param ch The char to remove from the registry.
 *
 * @return Whether the command was removed.
 *
 * @see pen_command_registry_add
 * @see pen_command_registry_set
 */
bool pen_command_registry_remove(PenCommandRegistry registry, char ch);

/**
 * @brief Sets a pen command in the registry.
 *
 * @param registry Pointer to the registry.
 * @param ch The char to which the command will be registered.
 * @param command The command to register.
 * @return Whether the command was set.
 *
 * @see pen_command_registry_add
 * @see pen_command_registry_remove
 */
bool pen_command_registry_set(PenCommandRegistry registry, char ch,
                              PenCommand command);

/**
 * @brief Executes the pen commands using a given registry.
 *
 * @param pen The pen to use.
 * @param registry The registry to use.
 * @param commands The commands to execute.
 */
void pen_commands_execute(Pen *pen, const PenCommandRegistry registry,
                          const char *commands);
