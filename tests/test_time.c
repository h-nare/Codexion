#include "codexion.h"

int main(void)
{
    t_simulation sim;

    sim.start_time = get_time_ms();
    if (sim.start_time == -1)
        return (1);
    printf("Before sleep: %ld ms\n", elapsed_time_ms(&sim));
    sleep_ms(200);
    printf("After sleep: %ld ms\n", elapsed_time_ms(&sim));
    return (0);
}