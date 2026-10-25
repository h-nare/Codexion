
#include "codexion.h"

int main(int argc, char **argv)
{
    t_config        config;
    t_simulation    sim;
    int             i;

    if (!parse_arguments(argc, argv, &config))
        return (1);
    if (!init_simulation(&sim, &config))
        return (1);
    i = 0;
    while (i < sim.config.number_of_coders)
    {
        printf("Coder %d: left=%td, right=%td\n",
            sim.coders[i].id,
            sim.coders[i].left_dongle - sim.dongles,
            sim.coders[i].right_dongle - sim.dongles);
        i++;
    }
    destroy_simulation(&sim);
    return (0);
}
