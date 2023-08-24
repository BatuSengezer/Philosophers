/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/24 21:34:08 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	print_state(t_philosopher_args *args, t_philosopher_state state)
{
	pthread_mutex_lock(&args->sim_params->death_mutex);
	if (pthread_mutex_lock(args->print_mutex))
		print_exit("Error: could not lock print mutex\n");
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
		printf("died\n");
		exit(1);
	}
	else if (state == HAS_FORK)
		printf("has taken a fork\n");
	else if (state == HAS_FORKS)
		printf("has taken a fork\n");
	if (pthread_mutex_unlock(args->print_mutex))
		print_exit("Error: could not unlock print mutex\n");
	if (pthread_mutex_unlock(&args->sim_params->death_mutex))
		print_exit("Error: could not lock death mutex\n");
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
	pick_up_forks(args);
	pthread_mutex_lock(&args->philosopher->meal_mutex);
	args->philosopher->state = EATING;
	if (args->sim_params->hunger_check == ON)
	{
		args->philosopher->meals_eaten++;
		args->sim_params->total_meals_eaten++;
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
			args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
	}
	args->philosopher->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	pthread_mutex_unlock(&args->philosopher->meal_mutex);
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
	while (args->philosopher->death_state== EVERYONE_ALIVE)
	{
		pthread_mutex_unlock(&args->sim_params->death_mutex);
		eat(args);
		// added drop fork for even and odd
		if (args->philosopher->id % 2 == 1)
			if (pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork) 
				|| pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork))
				print_exit("Error: could not unlock fork mutex\n");
		if (args->philosopher->id % 2 == 0)
			if (pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork) 
				|| pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork))
				print_exit("Error: could not unlock fork mutex\n");
		// if (pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork) 
		// 	|| pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork))
		// 	print_exit("Error: could not unlock fork mutex\n");

		if (args->sim_params->hunger_check == ON
			&& args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			break ;
		// args->philosopher->state = SLEEPING;
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
		// args->philosopher->state = THINKING;
		// print_state(args, THINKING);
				print_state(args, THINKING);
		// pthread_mutex_lock(&args->sim_params->death_mutex);
	}
	// pthread_mutex_unlock(&args->sim_params->death_mutex);
	return (NULL);
}
