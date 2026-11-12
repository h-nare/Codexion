/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: narehakobyan <narehakobyan@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/03 19:47:12 by narhakob          #+#    #+#             */
/*   Updated: 2026/11/12 18:21:58 by narehakobya      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
 
# include <limits.h>
# include <pthread.h>
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
	long					busy_until;
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
 
struct s_simulation
{
	t_config				config;
	t_coder					*coders;
	t_dongle				*dongles;
	unsigned long			next_sequence;
	long					start_time;
	pthread_mutex_t			stop_mutex;
	int						stop;
	pthread_mutex_t			log_mutex;
	pthread_mutex_t			scheduler_mutex;
};
 
/* parsing */
int							parse_arguments(int argc, char **argv,
								t_config *config);
 
/* heap */
int							heap_init(t_heap *heap, int capacity,
								t_scheduler scheduler);
int							heap_push(t_heap *heap, t_request request);
int							heap_remove(t_heap *heap, int coder_id);
t_request					*heap_top(t_heap *heap);
void						heap_destroy(t_heap *heap);
void						heap_sift_up(t_heap *heap, int i);
void						heap_sift_down(t_heap *heap, int i);
 
/* init and cleanup */
int							init_simulation(t_simulation *sim,
								t_config *config);
int							init_global_mutexes(t_simulation *sim);
void						destroy_simulation(t_simulation *sim);
void						destroy_global_mutexes(t_simulation *sim);
void						destroy_one_dongle(t_dongle *dongle);
void						destroy_dongles(t_simulation *sim);
 
/* time */
long						get_time_ms(void);
void						sleep_ms(long duration);
int							sleep_or_stop(t_simulation *sim, long duration);
 
/* stop */
int							simulation_stopped(t_simulation *sim);
void						stop_simulation(t_simulation *sim);
 
/* dongles */
int							request_both_dongles(t_coder *coder);
void						lock_dongles(t_coder *coder);
void						unlock_dongles(t_coder *coder);
int							try_acquire_dongles(t_coder *coder);
void						release_dongles(t_coder *coder);
 
/* scheduling */
t_coder						*coder_ahead(t_coder *coder, t_dongle *dongle);
long						earliest_start(t_coder *coder, long now, int depth);
int							eligible(t_coder *coder, long now);
 
/* coder */
void						*coder_routine(void *arg);
int							coder_finished(t_coder *coder);
int							get_dongles(t_coder *coder);
void						cancel_requests(t_coder *coder);
int							compile_code(t_coder *coder);
 
/* threads and monitor */
int							start_coders(t_simulation *sim);
void						join_coders(t_simulation *sim, int created);
void						*monitor_routine(void *arg);
 
/* log */
void						log_action(t_coder *coder, const char *action);
void						log_burnout(t_coder *coder);
 
#endif