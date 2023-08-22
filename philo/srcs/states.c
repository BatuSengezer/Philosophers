#include "philosophers.h"

void	print_state(t_philosopher_args *args, t_philosopher_state state)
{
	if (pthread_mutex_lock(args->print_mutex))
		print_exit("Error: could not lock print mutex\n");
	// commenting out following solves died multiple print at same line
	// if (args->sim_params->death_state == SOMEONE_DIED && death_announced == 1)
	// {
	// 	if (pthread_mutex_unlock(args->print_mutex))
	// 		print_exit("Error: could not unlock print mutex\n");
	// }
	printf("%lld %d ", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	if (state == EATING)
		printf("is eating\n");
	else if (state == SLEEPING)
		printf("is sleeping\n");
	else if (state == THINKING)
		printf("is thinking\n");
	else if (state == DIED)
	{
		// death_announced = 1;
		printf("died\n");
		exit(1);
	}
	else if (state == HAS_FORK)
		printf("has taken a fork\n");
	else if (state == HAS_FORKS)
		printf("has taken a fork\n");
	if (pthread_mutex_unlock(args->print_mutex))
		print_exit("Error: could not unlock print mutex\n");
}

int	pick_up_forks(t_philosopher_args *args)
{
	if (args->philosopher->id % 2 == 0)
	{
		pthread_mutex_lock(&args->philosopher->fork_right->mutex_fork);
		args->philosopher->state = HAS_FORK;
		print_state(args, HAS_FORK);
		pthread_mutex_lock(&args->philosopher->fork_left->mutex_fork);
		args->philosopher->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	if (args->philosopher->id % 2 == 1)
	{
		pthread_mutex_lock(&args->philosopher->fork_left->mutex_fork);
		print_state(args, HAS_FORK);
		args->philosopher->state = HAS_FORK;
		pthread_mutex_lock(&args->philosopher->fork_right->mutex_fork);
		args->philosopher->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	return (1);
}

void	eat(t_philosopher_args	*args)
{
	long long i;

	i = 0;
	pick_up_forks(args);
	args->philosopher->state = EATING;
	if (args->sim_params->hunger_check == ON)
	{
		args->philosopher->meals_eaten++;
		args->sim_params->total_meals_eaten++;
		// if (args->sim_params->total_meals_eaten >= args->sim_params
		// 	->total_meals_to_be_eaten)
		// 	args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
		{
			while(args->philosopher[i].meals_eaten >= args->sim_params->number_of_times_each_philo_must_eat)
			{
				i++;
				if (i == args->sim_params->number_of_philosophers)
					args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
			}
		}
	}
	args->philosopher->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	if (pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork))
		print_exit("Error: could not unlock fork mutex\n");
	if (pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork))
		print_exit("Error: could not unlock fork mutex\n");
}

void	*eat_sleep_think(void *arg)
{
	t_philosopher_args		*args;

	args = (t_philosopher_args *)arg;
	// to fix no print thinking for the start (use visualizer)
	print_state(args, args->philosopher->state);
	if (args->sim_params->number_of_philosophers == 1)
	{
		handle_single_philosopher_case(args);
		return (NULL);
	}
	while (1)
	{
		// if (check_death(args))
		// 	break ;
		eat(args);
		if (args->sim_params->hunger_check == ON
			&& args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			break ;
		args->philosopher->state = SLEEPING;
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
		args->philosopher->state = THINKING;
		print_state(args, THINKING);
	}
	return (NULL);
}
