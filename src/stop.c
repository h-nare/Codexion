#include "codexion.h"

int simulation_stopped(t_simulation *sim)
{
    int stopped;

    pthread_mutex_lock(&sim->stop_mutex);
    stopped = sim->stop;
    pthread_mutex_unlock(&sim->stop_mutex);
    return (stopped);
}


int check_coder_burnout(t_coder *coder, long now)
{
    t_simulation *sim;
    int burned_out;

    sim = coder->sim;
    burned_out = 0;
    pthread_mutex_lock(&sim->stop_mutex);
    pthread_mutex_lock(&coder->state_mutex);

    if (!sim->stop
        && coder->compiles_done
            < coder->config->number_of_compiles_required
        && now - coder->last_compile
            >= coder->config->time_to_burnout)
    {
        sim->stop = 1;
        burned_out = 1;
    }

    pthread_mutex_unlock(&coder->state_mutex);
    pthread_mutex_unlock(&sim->stop_mutex);
    return (burned_out);
}

void stop_simulation(t_simulation *sim)
{
    pthread_mutex_lock(&sim->stop_mutex);
    sim->stop = 1;
    pthread_mutex_unlock(&sim->stop_mutex);
}