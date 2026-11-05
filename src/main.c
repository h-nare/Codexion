/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:40:37 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/03 19:40:38 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"

int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	sim;
	pthread_t		monitor_thread;

	if (!parse_arguments(argc, argv, &config))
		return (1);
	if (!init_simulation(&sim, &config))
		return (1);
	if (!start_coders(&sim))
	{
		destroy_simulation(&sim);
		return (1);
	}
	if (pthread_create(&monitor_thread, NULL, monitor_routine, &sim) != 0)
	{
		stop_simulation(&sim);
		join_coders(&sim, sim.config.number_of_coders);
		destroy_simulation(&sim);
		return (1);
	}
	pthread_join(monitor_thread, NULL);
	join_coders(&sim, sim.config.number_of_coders);
	destroy_simulation(&sim);
	return (0);
}
