/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/24 14:04:48 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	print_state(t_philosopher_args *args, t_philosopher_state state)
{
	pthread_mutex_lock(&args->sim_params->death_mutex);
	pthread_mutex_lock(args->print_mutex);
	// printf("%lld %d ", current_timestamp(args->sim_params->start_time),
	// 	args->philosopher->id);
	if (state == EATING && args->philosopher->death_state != SOMEONE_DIED)
		printf("%lld %d is eating\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	else if (state == SLEEPING && args->philosopher->death_state != SOMEONE_DIED)
		printf("%lld %d is sleeping\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	else if (state == THINKING && args->philosopher->death_state != SOMEONE_DIED)
		printf("%lld %d is thinking\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	else if (state == DIED && args->philosopher->death_state != SOMEONE_DIED)
	{
		printf("%lld %d died\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
		// exit (1);
	}
	else if (state == HAS_FORK && args->philosopher->death_state != SOMEONE_DIED)
		printf("%lld %d has taken a fork\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	else if (state == HAS_FORKS && args->philosopher->death_state != SOMEONE_DIED)
		printf("%lld %d has taken a fork\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
	pthread_mutex_unlock(args->print_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
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
	// long long	i;

	// i = 0;
	// if (current_timestamp(
	// 		args->sim_params->start_time) - args->philosopher->last_meal_timestamp < args->sim_params->time_to_eat +args->sim_params->time_to_sleep + 10)
	// 	usleep(500);
	pick_up_forks(args);
	pthread_mutex_lock(&args->philosopher->last_meal_mutex);
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
	pthread_mutex_unlock(&args->philosopher->last_meal_mutex);
	if (args->philosopher->id % 2 == 0)
		if (pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork) 
			|| pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork))
			print_exit("Error: could not unlock fork mutex\n");
	if (args->philosopher->id % 2 == 1)
		if (pthread_mutex_unlock(&args->philosopher->fork_right->mutex_fork) 
			|| pthread_mutex_unlock(&args->philosopher->fork_left->mutex_fork))
			print_exit("Error: could not unlock fork mutex\n");
}

void	*eat_sleep_think(void *arg)
{
	t_philosopher_args		*args;

	args = (t_philosopher_args *)arg;
	if (args->sim_params->number_of_philosophers == 1)
	{
		handle_single_philosopher_case(args);
		return (NULL);
	}
	// print_state(args, args->philosopher->state);
	pthread_mutex_lock(&args->sim_params->death_mutex);
	while (args->philosopher->death_state != SOMEONE_DIED && args->sim_params
		->hunger_state != PHILOSOPHERS_ARE_FULL)
	{
		pthread_mutex_unlock(&args->sim_params->death_mutex);
		args->philosopher->state = THINKING;
		print_state(args, THINKING);
		if (args->philosopher->death_state != SOMEONE_DIED)
			eat(args);
		if (args->sim_params->hunger_check == ON
			&& args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			break ;
		args->philosopher->state = SLEEPING;
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
		pthread_mutex_lock(&args->sim_params->death_mutex);
	}
	pthread_mutex_unlock(&args->sim_params->death_mutex);
	return (NULL);
}
