/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eligible.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/12 21:31:34 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 21:40:36 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongles_available(t_coder *coder, long now)
{
	if (coder->left_dongle->is_taken || coder->right_dongle->is_taken)
		return (0);
	if (now < coder->left_dongle->cooldown_until
		|| now < coder->right_dongle->cooldown_until)
		return (0);
	return (1);
}

static int	has_request(t_dongle *dongle, int coder_id)
{
	int	i;

	i = 0;
	while (i < dongle->requests.size)
	{
		if (dongle->requests.requests[i].coder_id == coder_id)
			return (1);
		i++;
	}
	return (0);
}

static int	can_bypass(t_coder *coder, t_dongle *dongle, long now)
{
	t_coder	*ahead;
	long	start;

	ahead = coder_ahead(coder, dongle);
	if (ahead == NULL)
		return (1);
	start = earliest_start(ahead, now, 0);
	if (start == LONG_MAX)
		return (0);
	return (coder->config->time_to_compile + coder->config->dongle_cooldown
		+ 2 <= start - now);
}

int	eligible(t_coder *coder, long now)
{
	if (!dongles_available(coder, now))
		return (0);
	if (!has_request(coder->left_dongle, coder->id)
		|| !has_request(coder->right_dongle, coder->id))
		return (0);
	return (can_bypass(coder, coder->left_dongle, now) && can_bypass(coder,
			coder->right_dongle, now));
}
