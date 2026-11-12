/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:25 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:41:16 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_one_dongle(t_simulation *sim, int i)
{
	t_dongle	*dongle;

	dongle = &sim->dongles[i];
	dongle->is_taken = 0;
	dongle->cooldown_until = 0;
	dongle->busy_until = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (0);
	if (!heap_init(&dongle->requests, sim->config.number_of_coders,
			sim->config.scheduler))
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (0);
	}
	return (1);
}

static int	init_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (!init_one_dongle(sim, i))
		{
			while (--i >= 0)
				destroy_one_dongle(&sim->dongles[i]);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	init_coders(t_simulation *sim)
{
	int	i;
	int	n;

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
		sim->coders[i].sim = sim;
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&sim->coders[i].state_mutex);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	init_parts(t_simulation *sim)
{
	if (!init_global_mutexes(sim))
		return (0);
	if (!init_dongles(sim))
	{
		destroy_global_mutexes(sim);
		return (0);
	}
	if (!init_coders(sim))
	{
		destroy_dongles(sim);
		destroy_global_mutexes(sim);
		return (0);
	}
	return (1);
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	int	n;

	if (sim == NULL || config == NULL || config->number_of_coders <= 0)
		return (0);
	n = config->number_of_coders;
	sim->config = *config;
	sim->start_time = 0;
	sim->next_sequence = 0;
	sim->stop = 0;
	sim->coders = malloc(sizeof(t_coder) * n);
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (sim->coders != NULL && sim->dongles != NULL && init_parts(sim))
		return (1);
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
	return (0);
}
