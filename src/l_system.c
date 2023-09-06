#include "l_system.h"
#include "utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *l_system_apply_rule(const char *string, const Rule *rule)
{
    size_t allocated = 1;
    char *res = xmalloc(1);
    *res = '\0';

    char ch = 0;
    while ((ch = *string++))
    {
        char *target = NULL;
        char char_buf[2] = {0};

        if (rule->from == ch)
        {
            target = rule->to;
        }
        else
        {
            char_buf[0] = ch;
            target = char_buf;
        }

        while (allocated <= strlen(res) + strlen(target))
        {
            allocated *= 2;
            res = xrealloc(res, allocated);
        }

        strcat(res, target);
    }

    res = xrealloc(res, strlen(res) + 1);

    return res;
}

char *l_system_apply_rules(const char *string, const Rule *rules, int size)
{
    char *res = strdup(string);
    char *temp = NULL;

    for (const Rule *it = rules; it < rules + size; it++)
    {
        temp = l_system_apply_rule(res, it);
        free(res);
        res = temp;
    }

    return res;
}
