#include "codexion.h"

int main(void)
{
    t_config        config = {0};
    t_simulation    sim;
    t_request       out;

    config.number_of_coders = 3;
    config.time_to_burnout = 800;
    config.scheduler = FIFO;
    if (!init_simulation(&sim, &config))
        return (1);
    if (!request_dongle(&sim, &sim.coders[0],
            &sim.dongles[1], 100)
        || !request_dongle(&sim, &sim.coders[1],
            &sim.dongles[1], 200))
    {
        destroy_simulation(&sim);
        return (1);
    }
    while (heap_pop(&sim.dongles[1].requests, &out))
        printf("Coder %d, sequence %lu\n",
            out.coder_id, out.sequence);
    destroy_simulation(&sim);
    return (0);
}
