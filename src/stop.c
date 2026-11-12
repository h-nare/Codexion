/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:26 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:22:57 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
int	simulation_stopped(t_simulation *sim)
{
	int	stopped;
 
	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}
 
void	stop_simulation(t_simulation *sim)
{
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->stop_mutex);
}