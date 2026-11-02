#include "codexion.h"

void join_coders(t_simulation *sim, int created)
{
    int i;

    i = 0;
    while(i < created)
    {
        pthread_join(sim->coders[i].thread, NULL);
        i++;
    }
}


int start_coders(t_simulation *sim)
{
    int i;

    i = 0;
    sim->start_time = get_time_ms();
    if (sim->start_time == -1)
        return (0);
    i = 0;
    while(i < sim->config.number_of_coders)
    {
        sim->coders[i].last_compile = sim->start_time;
        i++;
    }

    i = 0;
    while(i < sim->config.number_of_coders)
    {
        if(pthread_create(&sim->coders[i].thread, NULL, coder_routine, &sim->coders[i]) != 0)
            break;
        i++;
    }
    if(i != sim->config.number_of_coders)
    {
        stop_simulation(sim);
        join_coders(sim, i);
        return (0);
    }
    return (1);
}