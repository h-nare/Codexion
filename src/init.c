/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:25 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/03 19:44:26 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_simulation *sim)
{
	int	i;
	int	n;

	n = sim->config.number_of_coders;
	i = 0;
	while (i < n)
	{
		sim->dongles[i].is_taken = 0;
		sim->dongles[i].cooldown_until = 0;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			break ;
		if (!heap_init(&sim->dongles[i].requests, n, sim->config.scheduler))
		{
			pthread_mutex_destroy(&sim->dongles[i].mutex);
			break ;
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

static void	destroy_dongles(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		heap_destroy(&sim->dongles[i].requests);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
}

int	init_simulation(t_simulation *sim, t_config *config)
{
	int	n;
	int	i;

	if (sim == NULL || config == NULL)
		return (0);
	n = config->number_of_coders;
	if (n <= 0)
		return (0);
	sim->start_time = 0;
	sim->config = *config;
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->coders = malloc(sizeof(t_coder) * n);
	if (!sim->coders)
		return (0);
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (!sim->dongles)
	{
		free(sim->coders);
		sim->coders = NULL;
		return (0);
	}
	if (!init_dongles(sim))
	{
		free(sim->dongles);
		free(sim->coders);
		sim->dongles = NULL;
		sim->coders = NULL;
		return (0);
	}
	sim->next_sequence = 0;
	sim->stop = 0;
	if (pthread_mutex_init(&sim->sequence_mutex, NULL) != 0)
	{
		destroy_dongles(sim);
		free(sim->dongles);
		free(sim->coders);
		return (0);
	}
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->sequence_mutex);
		destroy_dongles(sim);
		free(sim->dongles);
		free(sim->coders);
		return (0);
	}
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->sequence_mutex);
		destroy_dongles(sim);
		free(sim->dongles);
		free(sim->coders);
		return (0);
	}
	if (!init_coders(sim))
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->sequence_mutex);
		destroy_dongles(sim);
		free(sim->dongles);
		free(sim->coders);
		return (0);
	}
	if (pthread_mutex_init(&sim->scheduler_mutex, NULL) != 0)
	{
		i = 0;
		while (i < n)
		{
			pthread_mutex_destroy(&sim->coders[i].state_mutex);
			i++;
		}
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_mutex_destroy(&sim->sequence_mutex);
		destroy_dongles(sim);
		free(sim->dongles);
		free(sim->coders);
		sim->dongles = NULL;
		sim->coders = NULL;
		return (0);
	}
	return (1);
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
	pthread_mutex_destroy(&sim->scheduler_mutex);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->sequence_mutex);
	free(sim->coders);
	free(sim->dongles);
	sim->coders = NULL;
	sim->dongles = NULL;
}
