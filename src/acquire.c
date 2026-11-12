#include "codexion.h"
 
void	cancel_requests(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	lock_dongles(coder);
	heap_remove(&coder->left_dongle->requests, coder->id);
	heap_remove(&coder->right_dongle->requests, coder->id);
	unlock_dongles(coder);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
}
 
static int	wait_alone(t_coder *coder)
{
	log_action(coder, "has taken a dongle");
	while (!simulation_stopped(coder->sim))
		sleep_ms(1);
	return (0);
}
 
int	get_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
		return (wait_alone(coder));
	if (!request_both_dongles(coder))
	{
		stop_simulation(coder->sim);
		return (0);
	}
	while (!simulation_stopped(coder->sim))
	{
		if (try_acquire_dongles(coder))
			return (1);
		sleep_ms(1);
	}
	cancel_requests(coder);
	return (0);
}