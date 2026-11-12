/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:44:20 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 19:12:16 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
int	heap_init(t_heap *heap, int capacity, t_scheduler scheduler)
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
 
void	heap_destroy(t_heap *heap)
{
	if (heap == NULL)
		return ;
	free(heap->requests);
	heap->requests = NULL;
	heap->size = 0;
	heap->capacity = 0;
}
 
int	heap_push(t_heap *heap, t_request request)
{
	if (heap->size >= heap->capacity)
		return (0);
	heap->requests[heap->size] = request;
	heap->size++;
	heap_sift_up(heap, heap->size - 1);
	return (1);
}


static int	higher_priority(t_request *a, t_request *b,
	t_scheduler scheduler)
{
	if (scheduler == FIFO)
	{
		if (a->sequence != b->sequence)
			return (a->sequence < b->sequence);
	}
	else if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->coder_id < b->coder_id);
}


int	heap_remove(t_heap *heap, int coder_id)
{
	int	i;
	int	parent;

	i = 0;
	while (i < heap->size
		&& heap->requests[i].coder_id != coder_id)
		i++;
	if (i == heap->size)
		return (0);
	heap->size--;
	if (i == heap->size)
		return (1);
	heap->requests[i] = heap->requests[heap->size];
	if (i > 0)
	{
		parent = (i - 1) / 2;
		if (higher_priority(&heap->requests[i],
				&heap->requests[parent], heap->scheduler))
		{
			heap_sift_up(heap, i);
			return (1);
		}
	}
	heap_sift_down(heap, i);
	return (1);
}
 
t_request	*heap_top(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (&heap->requests[0]);
}
