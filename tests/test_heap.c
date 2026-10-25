#include "codexion.h"

static int run_test(t_scheduler scheduler)
{
    t_heap heap;
    t_request requests[4] = {
        {3, 100, 900, 0},
        {1, 100, 400, 1},
        {2, 150, 600, 2},
        {4, 200, 400, 3}
    };
    int fifo_expected[4] = {3, 1, 2, 4};
    int edf_expected[4] = {1, 4, 2, 3};
    int *expected = (scheduler == FIFO) ? fifo_expected : edf_expected;
    t_request out;
    int i;

    if (!heap_init(&heap, 4, scheduler))
        return (0);
    i = 0;
    while (i < 4)
    {
        if (!heap_push(&heap, requests[i]))
            return (heap_destroy(&heap), 0);
        i++;
    }
    if (heap_push(&heap, requests[0]))
        return (heap_destroy(&heap), 0);
    i = 0;
    while (i < 4)
    {
        if (!heap_pop(&heap, &out) || out.coder_id != expected[i])
            return (heap_destroy(&heap), 0);
        i++;
    }
    if (heap_pop(&heap, &out))
        return (heap_destroy(&heap), 0);
    heap_destroy(&heap);
    return (1);
}

int main(void)
{
    if (!run_test(FIFO) || !run_test(EDF))
    {
        printf("Heap tests FAILED\n");
        return (1);
    }
    printf("FIFO and EDF heap tests PASSED\n");
    return (0);
}