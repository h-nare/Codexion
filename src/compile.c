#include "codexion.h"
 
static int	start_compilation(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
	int				allowed;
 
	sim = coder->sim;
	allowed = 0;
	pthread_mutex_lock(&sim->stop_mutex);
	pthread_mutex_lock(&coder->state_mutex);
	now = get_time_ms();
	if (now == -1)
		sim->stop = 1;
	else if (!sim->stop
		&& now - coder->last_compile < coder->config->time_to_burnout)
	{
		coder->last_compile = now;
		allowed = 1;
	}
	pthread_mutex_unlock(&coder->state_mutex);
	pthread_mutex_unlock(&sim->stop_mutex);
	return (allowed);
}
 
int	compile_code(t_coder *coder)
{
	int	completed;
 
	if (!start_compilation(coder))
	{
		release_dongles(coder);
		return (0);
	}
	log_action(coder, "is compiling");
	completed = sleep_or_stop(coder->sim, coder->config->time_to_compile);
	if (completed)
	{
		pthread_mutex_lock(&coder->state_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->state_mutex);
	}
	release_dongles(coder);
	return (completed);
}
 