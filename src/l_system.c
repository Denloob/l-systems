#include "l_system.h"
#include "utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *l_system_apply_rules(const char *string, const Rule *rules, int size)
{
    size_t allocated = 1;
    char *res = xmalloc(1);
    *res = '\0';

    char ch = 0;
    while ((ch = *string++))
    {
        const char *target = &ch;
        size_t target_len = 1;

        for (const Rule *rule = rules; rule < rules + size; rule++)
        {
            if (rule->from != ch)
                continue;

            switch (rule->type)
            {
                case RULE_TYPE_STRING:
                    target = rule->to.string;
                    break;
                case RULE_TYPE_FUNCTION:
                    target = rule->to.func();
                    break;
            }
            target_len = strlen(target);
        }

        while (allocated <= strlen(res) + target_len)
        {
            allocated *= 2;
            res = xrealloc(res, allocated);
        }

        strncat(res, target, target_len);
    }

    res = xrealloc(res, strlen(res) + 1);

    return res;
}
