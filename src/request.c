/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:41:21 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:29:40 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
 
static int	push_both(t_coder *coder, t_request *request)
{
	t_dongle	*left;
	t_dongle	*right;
	int			success;
 
	left = coder->left_dongle;
	right = coder->right_dongle;
	lock_dongles(coder);
	success = heap_push(&left->requests, *request);
	if (success)
	{
		success = heap_push(&right->requests, *request);
		if (!success)
			heap_remove(&left->requests, coder->id);
	}
	unlock_dongles(coder);
	return (success);
}
 
int	request_both_dongles(t_coder *coder)
{
	t_request	request;
	int			success;
 
	if (coder->left_dongle == coder->right_dongle)
		return (0);
	request.coder_id = coder->id;
	pthread_mutex_lock(&coder->state_mutex);
	request.deadline = coder->last_compile + coder->config->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	pthread_mutex_lock(&coder->sim->scheduler_mutex);
	request.sequence = coder->sim->next_sequence;
	coder->sim->next_sequence++;
	success = push_both(coder, &request);
	pthread_mutex_unlock(&coder->sim->scheduler_mutex);
	return (success);
}
