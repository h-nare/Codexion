/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:09 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:26:18 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
int	coder_finished(t_coder *coder)
{
	int	finished;
 
	pthread_mutex_lock(&coder->state_mutex);
	finished = coder->compiles_done
		>= coder->config->number_of_compiles_required;
	pthread_mutex_unlock(&coder->state_mutex);
	return (finished);
}
 
static void	debug_code(t_coder *coder)
{
	log_action(coder, "is debugging");
	sleep_or_stop(coder->sim, coder->config->time_to_debug);
}
 
static void	refactor_code(t_coder *coder)
{
	log_action(coder, "is refactoring");
	sleep_or_stop(coder->sim, coder->config->time_to_refactor);
}
 
static int	coder_cycle(t_coder *coder)
{
	if (coder_finished(coder) || !get_dongles(coder))
		return (0);
	if (simulation_stopped(coder->sim))
	{
		release_dongles(coder);
		return (0);
	}
	if (!compile_code(coder) || simulation_stopped(coder->sim)
		|| coder_finished(coder))
		return (0);
	debug_code(coder);
	if (simulation_stopped(coder->sim))
		return (0);
	refactor_code(coder);
	return (1);
}
 
void	*coder_routine(void *arg)
{
	t_coder	*coder;
 
	coder = (t_coder *)arg;
	while (!simulation_stopped(coder->sim))
	{
		if (!coder_cycle(coder))
			break ;
	}
	return (NULL);
}
