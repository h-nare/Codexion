/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:34 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:29:06 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
void	join_coders(t_simulation *sim, int created)
{
	int	i;
 
	i = 0;
	while (i < created)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}
 
static int	create_threads(t_simulation *sim)
{
	int	i;
 
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, coder_routine,
				&sim->coders[i]) != 0)
			break ;
		i++;
	}
	return (i);
}
 
int	start_coders(t_simulation *sim)
{
	int	i;
 
	sim->start_time = get_time_ms();
	if (sim->start_time == -1)
		return (0);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile = sim->start_time;
		i++;
	}
	i = create_threads(sim);
	if (i == sim->config.number_of_coders)
		return (1);
	stop_simulation(sim);
	join_coders(sim, i);
	return (0);
}