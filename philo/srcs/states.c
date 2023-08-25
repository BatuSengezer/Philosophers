/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/25 04:58:12 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	print_state(t_philosopher_args *args, t_philosopher_state state)
{
	pthread_mutex_lock(&args->sim_params->death_mutex);
	pthread_mutex_lock(args->print_mutex);
	printf("%lld %d ", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	if (state == EATING && args->philosopher->death_state == EVERYONE_ALIVE)
		printf("is eating\n");
	else if (state == SLEEPING && args->philosopher->death_state == EVERYONE_ALIVE)
		printf("is sleeping\n");
	else if (state == THINKING && args->philosopher->death_state == EVERYONE_ALIVE)
		printf("is thinking\n");
	else if (state == DIED && args->philosopher->death_state == EVERYONE_ALIVE)
	{
		printf("died\n");
		exit(1);
	}
	else if (state == HAS_FORK && args->philosopher->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	else if (state == HAS_FORKS && args->philosopher->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	pthread_mutex_unlock(args->print_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
}

int	pick_up_forks(t_philosopher_args *args)
{
	//this if solves fork problem
	// prevent philosophers from immediately trying 
	// to eat or pick up forks too frequently
	if (current_timestamp(args->sim_params->start_time) 
		- args->philosopher->last_meal_timestamp 
		< args->sim_params->time_to_eat + args->sim_params->time_to_sleep + 10)
		usleep(500);
	if (args->philosopher->id % 2 == 0)
	{
		pthread_mutex_lock(args->philosopher->fork_right);
		args->philosopher->state = HAS_FORK;
		print_state(args, HAS_FORK);
		pthread_mutex_lock(args->philosopher->fork_left);
		args->philosopher->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	if (args->philosopher->id % 2 == 1)
	{
		pthread_mutex_lock(args->philosopher->fork_left);
		print_state(args, HAS_FORK);
		args->philosopher->state = HAS_FORK;
		pthread_mutex_lock(args->philosopher->fork_right);
		args->philosopher->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	return (1);
}

void	eat(t_philosopher_args	*args)
{
	pick_up_forks(args);
	pthread_mutex_lock(&args->philosopher->meal_mutex);
	args->philosopher->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	if (args->sim_params->hunger_check == ON)
	{
		args->philosopher->meals_eaten++;
		args->sim_params->total_meals_eaten++;
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
			args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
	}
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	pthread_mutex_unlock(&args->philosopher->meal_mutex);
	// added drop fork for even and odd
		if (args->philosopher->id % 2 == 1)
			if (pthread_mutex_unlock(args->philosopher->fork_right) 
				|| pthread_mutex_unlock(args->philosopher->fork_left))
				print_exit("Error: could not unlock fork mutex\n");
		if (args->philosopher->id % 2 == 0)
			if (pthread_mutex_unlock(args->philosopher->fork_left) 
				|| pthread_mutex_unlock(args->philosopher->fork_right))
				print_exit("Error: could not unlock fork mutex\n");
}

void	*eat_sleep_think(void *arg)
{
	t_philosopher_args		*args;

	args = (t_philosopher_args *)arg;
	// print_state(args, args->philosopher->state);
	if (args->sim_params->number_of_philosophers == 1)
	{
		handle_single_philosopher_case(args);
		return (NULL);
	}
	pthread_mutex_lock(&args->sim_params->death_mutex);
	while (args->philosopher->death_state == EVERYONE_ALIVE 
		&& 	args->philosopher->meals_to_eat != 0)
	{
		pthread_mutex_unlock(&args->sim_params->death_mutex);
		print_state(args, THINKING);
		eat(args);
		// if (pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork) 
		// 	|| pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork))
		// 	print_exit("Error: could not unlock fork mutex\n");

		if (args->sim_params->hunger_check == ON
			&& args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			return (pthread_mutex_unlock(&args->philosopher->meal_mutex), NULL);
		// args->philosopher->state = SLEEPING;
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
		// args->philosopher->state = THINKING;
		// print_state(args, THINKING);
		pthread_mutex_lock(&args->sim_params->death_mutex);
	}
	pthread_mutex_unlock(&args->sim_params->death_mutex);
	return (NULL);
}


void	*monitor_death(void *arg)
{
	t_philosopher_args	*args;
	long long			j;

	args = (t_philosopher_args *)arg;
	while (1)
	{
			// printf("rem_meals %d\n", rem_meals);
			pthread_mutex_lock(&args->philosopher->meal_mutex);
			if (args->philosopher->meals_to_eat
				&& (current_timestamp(args->sim_params->start_time)
				- args->philosopher->last_meal_timestamp
				> args->sim_params->time_to_die))
			{
				if (args->philosopher->meals_to_eat > 0)
					args->philosopher->meals_to_eat--;
				// printf("total_meals_to_be_eaten: %d\n", args->sim_params->total_meals_to_be_eaten);
				// printf("total_meals_eaten: %d\n", args->sim_params->total_meals_eaten);
				// printf("philosoper meals to eat: %d\n",args->philosopher->meals_to_eat); 
				// args->philosopher->state = DIED;
				print_state(args, DIED);
				pthread_mutex_lock(&args->sim_params->death_mutex);
				j = -1;
				while (++j < args->sim_params->number_of_philosophers)
					args->sim_params->philosophers[j].death_state = SOMEONE_DIED;
				//can do death_check everyone is alive
				pthread_mutex_unlock(&args->sim_params->death_mutex);
				pthread_mutex_unlock(&args->philosopher->meal_mutex);
				return (NULL);
			}
		// might need to add a mutex unlock here in if and after while loop
			else if (!args->philosopher->meals_to_eat)
				return (pthread_mutex_unlock(&args->philosopher->meal_mutex), NULL);
			pthread_mutex_unlock(&args->philosopher->meal_mutex);
	}
}
