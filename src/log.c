/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:31 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:30:36 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
void	log_action(t_coder *coder, const char *action)
{
	t_simulation	*sim;
	long			now;
 
	sim = coder->sim;
	pthread_mutex_lock(&sim->log_mutex);
	now = get_time_ms();
	if (now != -1 && !simulation_stopped(sim))
		printf("%ld %d %s\n", now - sim->start_time, coder->id, action);
	pthread_mutex_unlock(&sim->log_mutex);
}
 
void	log_burnout(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
 
	sim = coder->sim;
	pthread_mutex_lock(&sim->log_mutex);
	now = get_time_ms();
	if (now != -1)
		printf("%ld %d burned out\n", now - sim->start_time, coder->id);
	pthread_mutex_unlock(&sim->log_mutex);
}