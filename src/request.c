/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:21 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/03 19:45:02 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_both_dongles(t_coder *coder, long request_time)
{
	t_simulation	*sim;
	t_request		request;
	t_dongle		*left;
	t_dongle		*right;
	int				success;

	success = 0;
	sim = coder->sim;
	left = coder->left_dongle;
	right = coder->right_dongle;
	if (left == right)
		return (0);
	request.coder_id = coder->id;
	request.request_time = request_time;
	pthread_mutex_lock(&coder->state_mutex);
	request.deadline = coder->last_compile + sim->config.time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	pthread_mutex_lock(&sim->scheduler_mutex);
	pthread_mutex_lock(&sim->sequence_mutex);
	if (left < right)
	{
		pthread_mutex_lock(&left->mutex);
		pthread_mutex_lock(&right->mutex);
	}
	else
	{
		pthread_mutex_lock(&right->mutex);
		pthread_mutex_lock(&left->mutex);
	}
	request.sequence = sim->next_sequence++;
	success = heap_push(&left->requests, request);
	if (success)
	{
		success = heap_push(&right->requests, request);
		if (!success)
			heap_remove(&left->requests, coder->id);
	}
	pthread_mutex_unlock(&left->mutex);
	pthread_mutex_unlock(&right->mutex);
	pthread_mutex_unlock(&sim->sequence_mutex);
	pthread_mutex_unlock(&sim->scheduler_mutex);
	return (success);
}
