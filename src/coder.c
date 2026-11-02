#include "codexion.h"

void cancel_dongle_request(t_coder *coder, t_dongle *dongle)
{
    pthread_mutex_lock(&coder->sim->scheduler_mutex);
    pthread_mutex_lock(&dongle->mutex);
    heap_remove(&dongle->requests, coder->id);
    pthread_mutex_unlock(&dongle->mutex);
    pthread_mutex_unlock(&coder->sim->scheduler_mutex);
}

static int get_dongles(t_coder *coder)
{
    long now;

    if (coder->left_dongle == coder->right_dongle)
    {
        while (!simulation_stopped(coder->sim))
            sleep_ms(1);
        return (0);
    }
    now = get_time_ms();
    if (now == -1)
    {
        stop_simulation(coder->sim);
        return (0);
    }
    if (!request_both_dongles(coder, now))
    {
        stop_simulation(coder->sim);
        return (0);
        }
    while (!simulation_stopped(coder->sim))
    {
        now = get_time_ms();
        if (now == -1)
        {
            stop_simulation(coder->sim);
            break;
        }
        if (try_acquire_dongles(coder, now))
            return (1);
        sleep_ms(1);
    }
    cancel_dongle_request(coder, coder->left_dongle);
    cancel_dongle_request(coder, coder->right_dongle);
    return (0);
}



static int start_compilation(t_coder *coder)
{
    t_simulation *sim;
    long now;
    int allowed;

    sim = coder->sim;
    allowed = 0;

    pthread_mutex_lock(&sim->stop_mutex);
    pthread_mutex_lock(&coder->state_mutex);

    now = get_time_ms();
    if (now == -1)
        sim->stop = 1;
    else if (!sim->stop
        && now - coder->last_compile
            < coder->config->time_to_burnout)
    {
        coder->last_compile = now;
        allowed = 1;
    }

    pthread_mutex_unlock(&coder->state_mutex);
    pthread_mutex_unlock(&sim->stop_mutex);

    return (allowed);
}


static void compile_code(t_coder *coder)
{
    int completed;

    if (!start_compilation(coder))
    {
        release_dongles(coder);
        return;
    }
    log_action(coder, "is compiling");

    completed = sleep_or_stop(coder->sim,
            coder->config->time_to_compile);

    if (completed)
    {
        pthread_mutex_lock(&coder->state_mutex);
        coder->compiles_done++;
        pthread_mutex_unlock(&coder->state_mutex);
    }

    release_dongles(coder);
}


static void debug_code(t_coder *coder)
{
    log_action(coder, "is debugging");
    sleep_or_stop(coder->sim, coder->config->time_to_debug);
}

static void refactor_code(t_coder *coder)
{
    log_action(coder, "is refactoring");
    sleep_or_stop(coder->sim, coder->config->time_to_refactor);
}

int coder_finished(t_coder *coder)
{
    int finished;

    pthread_mutex_lock(&coder->state_mutex);
    finished = (coder->compiles_done
        >= coder->config->number_of_compiles_required);
    pthread_mutex_unlock(&coder->state_mutex);
    return (finished);
}

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;

    while (!simulation_stopped(coder->sim))
    {
        if (coder_finished(coder))
            break;
        if (!get_dongles(coder))
            break;
        if (simulation_stopped(coder->sim))
        {
            release_dongles(coder);
            break;
        }
        compile_code(coder);
        if (simulation_stopped(coder->sim))
            break;
        if (coder_finished(coder))
            break;
        debug_code(coder);
        if (simulation_stopped(coder->sim))
            break;
        refactor_code(coder);
    }
    return (NULL);
}
