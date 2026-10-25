#include "codexion.h"

static int_dongle(t_dongle *dongle, int coder_id, long now)
{
    if (dongle->is_taken)
        return (0);
    if (now < dongle->cooldown_until)
        return (0);
    if (dongle->requests.size == 0)
        return (0);
    if (dongle->requests.requests[0].coder_id != coder_id)
        return (0);
    return (1);
}

static int can_acquire_dongle(t_coder *coder, long now)
{
    if (!dongle_ready(coder->left_dongle, coder->id, now))
        return (0);
    if (!dongle_ready(coder->right_dongle, coder->id, now))
        return (0);
    return (1);
}

static void lock_dongles(t_coder *coder)
{
    if (coder->left_dongle < coder->right_dongle)
    {
        pthread_mutex_lock(&coder->left_dongle->mutex);
        pthread_mutex_lock(&coder->right_dongle->mutex);
    }
    else
    {
        pthread_mutex_lock(&coder->right_dongle->mutex);
        pthread_mutex_lock(&coder->left_dongle->mutex);
    }
}

static void unlock_dongles(t_coder *coder)
{
    pthread_mutex_unlock(&coder->left_dongle->mutex);
    pthread_mutex_unlock(&coder->right_dongle->mutex);
}


int try_acquire_dongles(t_coder *coder, long now)
{
    t_request request;

    if(coder->left_dongle == coder->right_dongle)
        return (0);
    lock_dongles(coder);
    if(!can_acquire_dongle(coder, now))
    {
        unlock_dongles(coder);
        return (0);
    }
    if (!heap_pop(&coder->left_dongle->requests, &request))
    {
        unlock_dongles(coder);
        return (0);
    }

    

}