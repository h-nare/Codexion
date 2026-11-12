/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   schedule.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 21:31:54 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:44:03 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	safe_add(long a, long b)
{
	if (a == LONG_MAX || b > LONG_MAX - a)
		return (LONG_MAX);
	return (a + b);
}

static long	free_at(t_coder *coder, t_dongle *dongle)
{
	if (dongle->is_taken)
		return (safe_add(dongle->busy_until, coder->config->dongle_cooldown));
	return (dongle->cooldown_until);
}

t_coder	*coder_ahead(t_coder *coder, t_dongle *dongle)
{
	t_request	*top;

	top = heap_top(&dongle->requests);
	if (top == NULL || top->coder_id == coder->id)
		return (NULL);
	return (&coder->sim->coders[top->coder_id - 1]);
}

static long	blocked_by(t_coder *coder, t_dongle *dongle, long now, int depth)
{
	t_coder	*ahead;
	long	start;

	ahead = coder_ahead(coder, dongle);
	if (ahead == NULL)
		return (now);
	start = earliest_start(ahead, now, depth + 1);
	start = safe_add(start, coder->config->time_to_compile);
	return (safe_add(start, coder->config->dongle_cooldown));
}

long	earliest_start(t_coder *coder, long now, int depth)
{
	long	t;
	long	x;

	if (depth >= coder->config->number_of_coders)
		return (LONG_MAX);
	t = now;
	x = free_at(coder, coder->left_dongle);
	if (x > t)
		t = x;
	x = free_at(coder, coder->right_dongle);
	if (x > t)
		t = x;
	x = blocked_by(coder, coder->left_dongle, now, depth);
	if (x > t)
		t = x;
	x = blocked_by(coder, coder->right_dongle, now, depth);
	if (x > t)
		t = x;
	return (t);
}
