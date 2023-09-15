#pragma once

typedef enum RuleType
{
    RULE_TYPE_STRING,
    RULE_TYPE_FUNCTION
} RuleType;
typedef const char *(*RuleFunction)();

typedef struct Rule
{
    RuleType type;
    char from;
    union
    {
        char *string;
        RuleFunction func;
    } to;
} Rule;

/**
 * @brief Applies all rules to a string.
 *
 * @param string The string to apply the rules to.
 * @param rules The rules to apply.
 * @param size The amount of rules.
 * @return String with the rules applied. (Managed by the caller).
 */
char *l_system_apply_rules(const char *string, const Rule *rules, int size);
