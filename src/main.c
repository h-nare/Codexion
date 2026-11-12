/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:40:37 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:30:09 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "codexion.h"
 
static int	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler(fifo|edf)\n");
	return (1);
}
 
int	main(int argc, char **argv)
{
	t_config		config;
	t_simulation	sim;
	pthread_t		monitor_thread;
 
	if (!parse_arguments(argc, argv, &config))
		return (print_usage());
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
