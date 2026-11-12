/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 21:31:12 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:36:34 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_global_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	if (pthread_mutex_init(&sim->scheduler_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (0);
	}
	return (1);
}

void	destroy_global_mutexes(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->scheduler_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
}

void	destroy_one_dongle(t_dongle *dongle)
{
	heap_destroy(&dongle->requests);
	pthread_mutex_destroy(&dongle->mutex);
}

void	destroy_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		destroy_one_dongle(&sim->dongles[i]);
		i++;
	}
}

void	destroy_simulation(t_simulation *sim)
{
	int	i;

	if (sim == NULL)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_destroy(&sim->coders[i].state_mutex);
		i++;
	}
	destroy_dongles(sim);
	destroy_global_mutexes(sim);
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
}
