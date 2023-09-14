#pragma once

typedef struct StackNode Stack;

/**
 * @brief Pops an element from the stack.
 *
 * @return The popped element or NULL if the stack is empty.
 *         Managed by the caller.
 */
void *stack_pop(Stack **stack_ptr);

/**
 * @brief Pushes an element onto the stack.
 *
 * @param stack_ptr Pointer to the stack.
 *                  If the pointer points to null [aka (*stack_ptr == NULL)]
 *                  creates the stack.
 * @param data Pointer to the data you want to push onto the stack.
 *             Managed by the stack until poped.
 */
void stack_push(Stack **stack_ptr, void *data);
