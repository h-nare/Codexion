/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:46 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:27:16 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
long	get_time_ms(void)
{
	struct timespec	ts;
 
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		return (-1);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}
 
void	sleep_ms(long duration)
{
	long	start;
	long	now;
 
	if (duration <= 0)
		return ;
	start = get_time_ms();
	if (start == -1)
		return ;
	while (1)
	{
		now = get_time_ms();
		if (now == -1 || now - start >= duration)
			break ;
		usleep(500);
	}
}
 
int	sleep_or_stop(t_simulation *sim, long duration)
{
	long	start;
	long	now;
 
	start = get_time_ms();
	if (start == -1)
		return (0);
	while (!simulation_stopped(sim))
	{
		now = get_time_ms();
		if (now == -1)
			return (0);
		if (now - start >= duration)
			return (1);
		usleep(500);
	}
	return (0);
}