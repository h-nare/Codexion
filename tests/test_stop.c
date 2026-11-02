
#include "codexion.h"

int main(void)
{
    t_simulation sim;

    memset(&sim, 0, sizeof(sim));
    if (pthread_mutex_init(&sim.stop_mutex, NULL) != 0)
        return (1);

    printf("Before: %d\n", simulation_stopped(&sim));

    stop_simulation(&sim);

    printf("After: %d\n", simulation_stopped(&sim));

    pthread_mutex_destroy(&sim.stop_mutex);
    return (0);
}
