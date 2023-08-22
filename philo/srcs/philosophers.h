#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef enum e_philosopher_state
{
	HAS_FORK = 1,
	HAS_FORKS,
	EATING,
	SLEEPING,
	THINKING,
	DIED
}							t_philosopher_state;

// typedef enum e_fork_state
// {
// 	FREE,
// 	IN_USE
// }							t_fork_state;

// typedef enum e_death_state
// {
// 	EVERYONE_ALIVE = 0,
// 	SOMEONE_DIED
// }							t_death_state;

typedef enum e_hunger_state
{
	PHILOSOPHERS_NOT_FULL_YET = 0,
	PHILOSOPHERS_ARE_FULL
}							t_hunger_state;

typedef enum e_hunger_check
{
	ON = 0,
	OFF
}							t_hunger_check;
typedef struct s_fork
{
	int						id;
	// t_fork_state			state;
	pthread_mutex_t			mutex_fork;
}							t_fork;

typedef struct s_philosopher
{
	int						id;
	pthread_t				p_thread;
	t_philosopher_state		state;
	t_fork					*fork_right;
	t_fork					*fork_left;
	long long				last_meal_timestamp;
	int						meals_eaten;

}							t_philosopher;

// forward decleration so that we can use it in the struct before defining it

struct								s_philosopher_args;
typedef struct s_philosopher_args	t_philosopher_args;

typedef struct s_simulation_parameters
{
	int						number_of_philosophers;
	int						time_to_die;
	int						time_to_eat;
	int						time_to_sleep;
	struct timeval			start_time;
	// t_death_state			death_state;
	int						number_of_times_each_philo_must_eat;
	int						total_meals_to_be_eaten;
	int						total_meals_eaten;
	t_hunger_check			hunger_check;
	t_hunger_state			hunger_state;
	t_philosopher			*philosophers;
	t_fork					*forks;
	pthread_mutex_t			print_mutex;
	pthread_t				monitor_thread;
	t_philosopher_args		*args;
}							t_simulation_parameters;

typedef struct s_philosopher_args
{
	t_philosopher			*philosopher;
	t_simulation_parameters	*sim_params;
	pthread_mutex_t			*print_mutex;
}							t_philosopher_args;

//initilization functions

void		allocate(t_simulation_parameters *sim_params);
void		init_sim_param(t_simulation_parameters *sim_params,
				int argc, char **argv);
void		init_philosophers_and_forks(t_simulation_parameters *sim_params,
				int i);

//simulation functions

void		*monitor_death(void *arg);
void		simulation(t_simulation_parameters *sim_params);
void		print_state(t_philosopher_args *args, t_philosopher_state state);
int			pick_up_forks(t_philosopher_args *args);
void		eat(t_philosopher_args	*args);
void		*eat_sleep_think(void *arg);

//util functions

void		input_check(int argc, char **argv);
long long	current_timestamp(struct timeval start_time);
void		handle_single_philosopher_case(t_philosopher_args *args);
int			check_death(t_philosopher_args *args);
long long	ft_atoi(const char *nptr);
void		print_exit(char *str);
void		destroy_free(t_simulation_parameters *sim_params);

#endif