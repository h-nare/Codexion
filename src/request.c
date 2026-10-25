#include "codexion.h"

int request_dongle(t_simulation *sim, t_coder *coder, t_dongle *dongle, long request_time)
{
    t_request request;
    int success;


    if(sim == NULL || coder == NULL || dongle == NULL)
        return (0);
    
    request.coder_id = coder->id;
    request.request_time = request_time;
    request.deadline = coder->last_compile + sim->config.time_to_burnout;
    pthread_mutex_lock(&sim->sequence_mutex);
    request.sequence = sim->next_sequence++;
    pthread_mutex_lock(&dongle->mutex);
    success = heap_push(&dongle->requests, request);
    pthread_mutex_unlock(&dongle->mutex);
    pthread_mutex_unlock(&sim->sequence_mutex);

    return (success);
}


