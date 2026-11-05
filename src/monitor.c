/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:04 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/03 19:44:41 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	coder_burned_out(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
	int				burned_out;

	sim = coder->sim;
	burned_out = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	pthread_mutex_lock(&coder->state_mutex);
	now = get_time_ms();
	if (now == -1)
		sim->stop = 1;
	else if (!sim->stop
		&& coder->compiles_done < coder->config->number_of_compiles_required
		&& now - coder->last_compile >= coder->config->time_to_burnout)
	{
		sim->stop = 1;
		burned_out = 1;
	}
	pthread_mutex_unlock(&coder->state_mutex);
	pthread_mutex_unlock(&sim->stop_mutex);
	if (burned_out)
		log_burnout(coder);
	return (burned_out);
}

static int	all_coders_finished(t_simulation *sim)
{
	int	i;
	int	finished;

	i = 0;
	finished = 1;
	pthread_mutex_lock(&sim->stop_mutex);
	while (i < sim->config.number_of_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		if (sim->coders[i].compiles_done < sim->config.number_of_compiles_required)
			finished = 0;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (!finished)
			break ;
		i++;
	}
	if (finished && !sim->stop)
		sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (finished);
}

void	monitor_simulation(t_simulation *sim)
{
	int	i;

	while (!simulation_stopped(sim))
	{
		i = 0;
		while (i < sim->config.number_of_coders)
		{
			if (coder_burned_out(&sim->coders[i]))
				return ;
			i++;
		}
		if (all_coders_finished(sim))
			return ;
		sleep_ms(1);
	}
}

void	*monitor_routine(void *arg)
{
	t_simulation *sim;

	sim = (t_simulation *)arg;
	monitor_simulation(sim);
	return (NULL);
}