#pragma once

typedef struct Rule
{
    char from;
    char *to;
} Rule;

/**
 * @brief Applies one rule to a string.
 *
 * @param string The string to apply the rule to.
 * @param rule The rule to apply.
 * @return String with the rule applied. (Managed by the caller).
 */
char *l_system_apply_rule(const char *string, const Rule *rule);

/**
 * @brief Applies all rules to a string.
 *
 * @param string The string to apply the rules to.
 * @param rules The rules to apply.
 * @param size The amount of rules.
 * @return String with the rules applied. (Managed by the caller).
 */
char *l_system_apply_rules(const char *string, const Rule *rules, int size);
