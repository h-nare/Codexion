/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:00 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/05 20:45:41 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	lock_dongles(t_coder *coder)
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

static void	unlock_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}

static int	dongles_available(t_coder *coder, long now)
{
	if (coder->left_dongle == coder->right_dongle)
		return (0);
	if (coder->left_dongle->is_taken || coder->right_dongle->is_taken)
		return (0);
	if (now < coder->left_dongle->cooldown_until
		|| now < coder->right_dongle->cooldown_until)
		return (0);
	return (1);
}

static int    is_queue_head(t_heap *heap, int coder_id)
{
    if (heap->size == 0)
        return (0);
    return (heap->requests[0].coder_id == coder_id);
}

static int	higher_priority(t_request a, t_request b, t_scheduler scheduler)
{
	if (scheduler == EDF && a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	if (a.sequence != b.sequence)
		return (a.sequence < b.sequence);
	return (a.coder_id < b.coder_id);
}

static int    eligible(t_coder *coder, long now, t_request *request)
{
    if (!dongles_available(coder, now))
        return (0);
    if (!is_queue_head(&coder->left_dongle->requests, coder->id))
        return (0);
    if (!is_queue_head(&coder->right_dongle->requests, coder->id))
        return (0);
    *request = coder->left_dongle->requests.requests[0];
    return (1);
}

static int	best_eligible_coder(t_simulation *sim, long now)
{
	int			i;
	int			best;
	t_request	request;
	t_request	best_request;

	i = 0;
	best = -1;
	while (i < sim->config.number_of_coders)
	{
		if (eligible(&sim->coders[i], now, &request))
		{
			if (best == -1 || higher_priority(request, best_request,
					sim->config.scheduler))
			{
				best = i;
				best_request = request;
			}
		}
		i++;
	}
	return (best);
}

int try_acquire_dongles(t_coder *coder, long now)
{
    t_simulation *sim;
    int best;

    sim = coder->sim;
    if (coder->left_dongle == coder->right_dongle)
        return (0);

    pthread_mutex_lock(&sim->scheduler_mutex);

    now = get_time_ms();
    if (now == -1)
    {
        pthread_mutex_unlock(&sim->scheduler_mutex);
        stop_simulation(sim);
        return (0);
    }

    best = best_eligible_coder(sim, now);
    if (best != coder->id - 1)
    {
        pthread_mutex_unlock(&sim->scheduler_mutex);
        return (0);
    }

    lock_dongles(coder);

    heap_remove(&coder->left_dongle->requests, coder->id);
    heap_remove(&coder->right_dongle->requests, coder->id);

    coder->left_dongle->is_taken = 1;
    coder->right_dongle->is_taken = 1;

    unlock_dongles(coder);
    pthread_mutex_unlock(&sim->scheduler_mutex);

    log_action(coder, "has taken a dongle");
    log_action(coder, "has taken a dongle");

    return (1);
}

void release_dongles(t_coder *coder)
{
    t_simulation *sim;
    long now;

    sim = coder->sim;

    pthread_mutex_lock(&sim->scheduler_mutex);
    lock_dongles(coder);

    now = get_time_ms();

    coder->left_dongle->is_taken = 0;
    coder->right_dongle->is_taken = 0;

    if (now != -1)
    {
        coder->left_dongle->cooldown_until =
            now + coder->config->dongle_cooldown;
        coder->right_dongle->cooldown_until =
            now + coder->config->dongle_cooldown;
    }

    unlock_dongles(coder);
    pthread_mutex_unlock(&sim->scheduler_mutex);

    if (now == -1)
        stop_simulation(sim);
}

