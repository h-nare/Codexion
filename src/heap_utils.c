/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 21:31:40 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:41:00 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	request_before(t_request *a, t_request *b, t_scheduler scheduler)
{
	if (scheduler == FIFO)
	{
		if (a->sequence != b->sequence)
			return (a->sequence < b->sequence);
		return (a->coder_id < b->coder_id);
	}
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->coder_id < b->coder_id);
}

static void	heap_swap(t_heap *heap, int i, int j)
{
	t_request	temp;

	temp = heap->requests[i];
	heap->requests[i] = heap->requests[j];
	heap->requests[j] = temp;
}

void	heap_sift_up(t_heap *heap, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!request_before(&heap->requests[i], &heap->requests[parent],
				heap->scheduler))
			break ;
		heap_swap(heap, i, parent);
		i = parent;
	}
}

void	heap_sift_down(t_heap *heap, int i)
{
	int	left;
	int	right;
	int	best;

	while (1)
	{
		left = 2 * i + 1;
		right = left + 1;
		best = i;
		if (left < heap->size && request_before(&heap->requests[left],
				&heap->requests[best], heap->scheduler))
			best = left;
		if (right < heap->size && request_before(&heap->requests[right],
				&heap->requests[best], heap->scheduler))
			best = right;
		if (best == i)
			break ;
		heap_swap(heap, i, best);
		i = best;
	}
}
