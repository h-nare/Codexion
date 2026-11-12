/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:00 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:39:58 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_dongles(t_coder *coder)
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

void	unlock_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}

static void	take_dongles(t_coder *coder, long now)
{
	long	busy;

	busy = now + coder->config->time_to_compile;
	lock_dongles(coder);
	heap_remove(&coder->left_dongle->requests, coder->id);
	heap_remove(&coder->right_dongle->requests, coder->id);
	coder->left_dongle->is_taken = 1;
	coder->right_dongle->is_taken = 1;
	coder->left_dongle->busy_until = busy;
	coder->right_dongle->busy_until = busy;
	unlock_dongles(coder);
}

int	try_acquire_dongles(t_coder *coder)
{
	t_simulation	*sim;
	long			now;

	sim = coder->sim;
	pthread_mutex_lock(&sim->scheduler_mutex);
	now = get_time_ms();
	if (now == -1 || !eligible(coder, now))
	{
		pthread_mutex_unlock(&sim->scheduler_mutex);
		if (now == -1)
			stop_simulation(sim);
		return (0);
	}
	take_dongles(coder, now);
	pthread_mutex_unlock(&sim->scheduler_mutex);
	log_action(coder, "has taken a dongle");
	log_action(coder, "has taken a dongle");
	return (1);
}

void	release_dongles(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
	long			until;

	sim = coder->sim;
	pthread_mutex_lock(&sim->scheduler_mutex);
	lock_dongles(coder);
	now = get_time_ms();
	until = now + coder->config->dongle_cooldown;
	coder->left_dongle->is_taken = 0;
	coder->right_dongle->is_taken = 0;
	if (now != -1)
	{
		coder->left_dongle->cooldown_until = until;
		coder->right_dongle->cooldown_until = until;
	}
	unlock_dongles(coder);
	pthread_mutex_unlock(&sim->scheduler_mutex);
	if (now == -1)
		stop_simulation(sim);
}
