#include "codexion.h"

static int init_dongles(t_simulation *sim)
{
    int i;
    int n;

    n = sim->config.number_of_coders;
    i = 0;
    while (i < n)
    {
        sim->dongles[i].is_taken = 0;
        sim->dongles[i].cooldown_until = 0;
        if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
            break;
        if (!heap_init(&sim->dongles[i].requests,
                n, sim->config.scheduler))
        {
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            break;
        }
        i++;
    }
    if (i == n)
        return (1);
    while (--i >= 0)
    {
        heap_destroy(&sim->dongles[i].requests);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
    }
    return (0);
}

static void init_coders(t_simulation *sim)
{
    int i;
    int n;

    n = sim->config.number_of_coders;
    i = 0;
    while (i < n)
    {
        sim->coders[i].id = i + 1;
        sim->coders[i].compiles_done = 0;
        sim->coders[i].last_compile = 0;
        sim->coders[i].left_dongle = &sim->dongles[i];
        sim->coders[i].right_dongle = &sim->dongles[(i + 1) % n];
        sim->coders[i].config = &sim->config;
        i++;
    }
}


static void destroy_dongles(t_simulation *sim)
{
    int i;

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        heap_destroy(&sim->dongles[i].requests);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        i++;
    }
}


int init_simulation(t_simulation *sim, t_config *config)
{
    int n;

    sim->start_time = 0;
    if(sim == NULL || config == NULL)
        return (0);
    n = config->number_of_coders;
    if(n <= 0)
        return (0);
    sim->config = *config;
    sim->coders = NULL;
    sim->dongles = NULL;
    sim->coders = malloc(sizeof(t_coder) * n);
    if(!sim->coders)
        return (0);
    sim->dongles = malloc(sizeof(t_dongle) * n);
    if(!sim->dongles)
    {
        free(sim->coders);
        sim->coders = NULL;
        return (0);
    }
    if(!init_dongles(sim))
    {
        free(sim->dongles);
        free(sim->coders);
        sim->dongles = NULL;
        sim->coders = NULL;
        return (0);
    }
    sim->next_sequence = 0;
    if(pthread_mutex_init(&sim->sequence_mutex, NULL) != 0)
    {
        destroy_dongles(sim);
        free(sim->dongles);
        free(sim->coders);
        sim->dongles = NULL;
        sim->coders = NULL;
        return (0);   
    }
    init_coders(sim);
    return (1);
}

void destroy_simulation(t_simulation *sim)
{
    int i;

    if(sim == NULL)
        return ;
    if(sim->dongles != NULL)
    {
        i = 0;
        while(i < sim->config.number_of_coders)
        {
            heap_destroy(&sim->dongles[i].requests);
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            i++;
        }
    }
    free(sim->coders);
    free(sim->dongles);
    sim->coders = NULL;
    sim->dongles = NULL;
}