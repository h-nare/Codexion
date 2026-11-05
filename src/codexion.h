/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narhakob <narhakob@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:47:12 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/03 19:47:13 by narhakob         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <limits.h>
# include <pthread.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO,
	EDF
}							t_scheduler;

typedef struct s_config
{
	int						number_of_coders;
	long					time_to_burnout;
	long					time_to_compile;
	long					time_to_debug;
	long					time_to_refactor;
	int						number_of_compiles_required;
	long					dongle_cooldown;
	t_scheduler				scheduler;
}							t_config;

typedef struct s_request
{
	int						coder_id;
	long					request_time;
	long					deadline;
	unsigned long			sequence;
}							t_request;

typedef struct s_heap
{
	t_request				*requests;
	int						size;
	int						capacity;
	t_scheduler				scheduler;
}							t_heap;

typedef struct s_dongle
{
	pthread_mutex_t			mutex;
	int						is_taken;
	long					cooldown_until;
	t_heap					requests;
}							t_dongle;

typedef struct s_simulation	t_simulation;

typedef struct s_coder
{
	int						id;
	pthread_t				thread;
	int						compiles_done;
	long					last_compile;
	t_dongle				*left_dongle;
	t_dongle				*right_dongle;
	t_config				*config;
	t_simulation			*sim;
	pthread_mutex_t			state_mutex;
}							t_coder;

struct						s_simulation
{
	t_config				config;
	t_coder					*coders;
	t_dongle				*dongles;
	pthread_mutex_t			sequence_mutex;
	unsigned long			next_sequence;
	long					start_time;

	pthread_mutex_t			stop_mutex;
	int						stop;
	pthread_mutex_t			log_mutex;
	pthread_mutex_t			scheduler_mutex;
};

/*Parser*/
int							parse_arguments(int argc, char **argv,
								t_config *config);

/*heap*/
int							heap_init(t_heap *heap, int capacity,
								t_scheduler scheduler);
int							heap_push(t_heap *heap, t_request request);
int							heap_pop(t_heap *heap, t_request *result);
void						heap_destroy(t_heap *heap);
int							heap_remove(t_heap *heap, int coder_id);

/*simulation*/
int							init_simulation(t_simulation *sim,
								t_config *config);
void						destroy_simulation(t_simulation *sim);

/*request dongle*/
int							request_both_dongles(t_coder *coder,
								long request_time);

/*time*/
long						get_time_ms(void);
long						elapsed_time_ms(t_simulation *sim);
void						sleep_ms(long duration);
int							sleep_or_stop(t_simulation *sim, long duration);

/*stop simulation*/
int							simulation_stopped(t_simulation *sim);
void						stop_simulation(t_simulation *sim);

/*acquire dongle*/
int							try_acquire_dongles(t_coder *coder, long now);
void						release_dongles(t_coder *coder);

/*coder*/
void						*coder_routine(void *arg);
int							start_coders(t_simulation *sim);
int							coder_finished(t_coder *coder);
void						monitor_simulation(t_simulation *sim);
void						join_coders(t_simulation *sim, int created);
void						cancel_dongle_request(t_coder *coder,
								t_dongle *dongle);

/*printing*/
void						log_action(t_coder *coder, const char *action);
void						log_burnout(t_coder *coder);

/*monitor*/
void						*monitor_routine(void *arg);

#endif