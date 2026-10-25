#ifndef CODEXION_H
#define CODEXION_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

typedef enum e_scheduler
{
    FIFO,
    EDF
} t_scheduler;

typedef struct s_config
{
    int number_of_coders;
    long time_to_burnout;
    long time_to_compile;
    long time_to_debug;
    long time_to_refactor;
    int number_of_compiles_required;
    long dongle_cooldown;
    t_scheduler scheduler;
} t_config;

typedef struct s_request
{
    int coder_id;
    long request_time;
    long deadline;
    unsigned long sequence;
} t_request;

typedef struct s_heap
{
    t_request *requests;
    int size;
    int capacity;
    t_scheduler scheduler;
} t_heap;

typedef struct s_dongle
{
    pthread_mutex_t mutex;
    int is_taken;
    long cooldown_until;
    t_heap requests;
} t_dongle;

typedef struct s_coder
{
    int id;
    pthread_t thread;
    int compiles_done;
    long last_compile;
    t_dongle *left_dongle;
    t_dongle *right_dongle;
    t_config *config;
} t_coder;

typedef struct s_simulation
{
    t_config config;
    t_coder *coders;
    t_dongle *dongles;
    pthread_mutex_t sequence_mutex;
    unsigned long   next_sequence;
    long start_time;
} t_simulation;


/*Parser*/
int parse_arguments(int argc, char **argv, t_config *config);

/*heap*/
int     heap_init(t_heap *heap, int capacity, t_scheduler scheduler);
int     heap_push(t_heap *heap, t_request request);
int     heap_pop(t_heap *heap, t_request *result);
void    heap_destroy(t_heap *heap);

/*simulation*/
int init_simulation(t_simulation *sim, t_config *config);
void destroy_simulation(t_simulation *sim);

/*request dongle*/
int request_dongle(t_simulation *sim, t_coder *coder, t_dongle *dongle, long request_time);

/*time*/
long get_time_ms();
long elapsed_time_ms(t_simulation *sim);
void sleep_ms(long duration);
#endif