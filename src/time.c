#include "codexion.h"

long get_time_ms()
{
    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return (-1);

    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

long elapsed_time_ms(t_simulation *sim)
{
    long now;

    now = get_time_ms();
    if(now == -1)
        return (-1);
    return (now - sim->start_time);
}

void sleep_ms(long duration)
{
    long start;
    long now;

    if (duration <= 0)
        return ;
    start = get_time_ms();
    if(start == -1)
        return ;
    while(1)
    {
        now = get_time_ms();
        if (now == -1 || now - start >= duration)
            break;
        usleep(500);
    }
}

int sleep_or_stop(t_simulation *sim, long duration)
{
    long start;
    long now;

    start = get_time_ms();
    if (start == -1)
        return (0);
    while (!simulation_stopped(sim))
    {
        now = get_time_ms();
        if (now == -1)
            return (0);
        if (now - start >= duration)
            return (1);
        usleep(500);
    }
    return (0);
}
