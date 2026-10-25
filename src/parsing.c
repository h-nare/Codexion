#include "codexion.h"

static int parse_number(const char *str, long *result, long max)
{
    long number;
    int i;
    int digit;

    if(!str || !str[0])
        return (0);
    i = 0;
    if(str[i] == '+')
        i++;
    if(!str[i])
        return (0);
    number = 0;
    while(str[i])
    {
        if(str[i] < '0' || str[i] > '9')
            return (0);
        digit = str[i] - '0';
        if(number > (max - digit)/10)
            return (0);
        number = number * 10 + digit;
        i++;
    }
    *result = number;
    return (1);
}

static int parse_scheduler(const char *str, t_scheduler *scheduler)
{
    if (strcmp(str, "fifo") == 0)
        *scheduler = FIFO;
    else if (strcmp(str,"edf") == 0)
        *scheduler = EDF;
    else
        return (0);
    return (1);
}

int parse_arguments(int argc, char **argv, t_config *config)
{
    long values[7];
    long max;
    int i;

    if(argc != 9 || !config)
        return (0);
    i = 0;
    while(i < 7)
    {
        max = LONG_MAX;
        if (i == 0 || i == 5)
            max = INT_MAX;
        if(!parse_number(argv[i + 1], &values[i], max))
            return (0);
        i++;
    }
    if(values[0] == 0)
        return (0);
    if(!parse_scheduler(argv[8], &config->scheduler))
        return (0);
    config->number_of_coders = (int)values[0];
    config->time_to_burnout = values[1];
    config->time_to_compile = values[2];
    config->time_to_debug = values[3];
    config->time_to_refactor = values[4];
    config->number_of_compiles_required = (int)values[5];
    config->dongle_cooldown = values[6];
    return (1);
}
