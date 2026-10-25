#include "codexion.h"

static int higher_priority(t_request a, t_request b, t_scheduler scheduler)
{
    if (scheduler == FIFO)
    {
        if (a.sequence != b.sequence)
            return (a.sequence < b.sequence);
    }
    else
    {
        if (a.deadline != b.deadline)
            return (a.deadline < b.deadline);
        if (a.sequence != b.sequence)
            return (a.sequence < b.sequence);
    }
    return (a.coder_id < b.coder_id);
}

int heap_push(t_heap *heap, t_request request)
{
    int i;
    int parent;
    t_request temp;

    if (heap->size >= heap->capacity)
        return (0);
    i = heap->size;
    heap->requests[i] = request;
    heap->size++;
    while(i > 0)
    {
        parent = (i - 1) / 2;
        if (!higher_priority(heap->requests[i], heap->requests[parent], heap->scheduler))
            break;
        temp = heap->requests[i];
        heap->requests[i] = heap->requests[parent];
        heap->requests[parent] = temp;
        i = parent;
    }
    return (1);
}

int heap_pop(t_heap *heap, t_request *result)
{
    int i;
    int left;
    int right;
    int smallest;
    t_request temp;

    if (heap->size == 0)
        return (0);
    *result = heap->requests[0];
    heap->size--;
    if (heap->size == 0)
        return (1);
    heap->requests[0] = heap->requests[heap->size];
    i = 0;
    while(1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        smallest = i;
        if (left < heap->size && higher_priority(heap->requests[left], heap->requests[smallest], heap->scheduler))
            smallest = left;
        if (right < heap->size && higher_priority(heap->requests[right], heap->requests[smallest], heap->scheduler))
            smallest = right;
        if (smallest == i)
            break;
        temp = heap->requests[i];
        heap->requests[i] = heap->requests[smallest];
        heap->requests[smallest] = temp;
        i = smallest;
    }
    return (1);
}

int heap_init(t_heap *heap, int capacity, t_scheduler scheduler)
{
    if (heap == NULL || capacity <= 0)
        return (0);
    heap->requests = malloc(sizeof(t_request) * capacity);
    if (heap->requests == NULL)
        return (0);
    heap->size = 0;
    heap->capacity = capacity;
    heap->scheduler = scheduler;
    return (1);
}


void heap_destroy(t_heap *heap)
{
    if (heap == NULL)
        return;
    free(heap->requests);
    heap->requests = NULL;
    heap->size = 0;
    heap->capacity = 0;
}
