/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/09/03 14:49:01 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	print_state(t_philosopher_args *args, t_philosopher_state state)
{
	pthread_mutex_lock(&args->sim_params->death_mutex);
	pthread_mutex_lock(args->print_mutex);
	if (args->philo->death_state == EVERYONE_ALIVE)
		printf("%lld %d ", current_timestamp(args->sim_params->start_time),
			args->philo->id);
	if (state == EATING && args->philo->death_state == EVERYONE_ALIVE)
		printf("is eating\n");
	else if (state == SLEEPING && args->philo->death_state == EVERYONE_ALIVE)
		printf("is sleeping\n");
	else if (state == THINKING && args->philo->death_state == EVERYONE_ALIVE)
		printf("is thinking\n");
	else if (state == DIED && args->philo->death_state == EVERYONE_ALIVE)
		printf("died\n");
	else if (state == HAS_FORK && args->philo->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	else if (state == HAS_FORKS && args->philo->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	pthread_mutex_unlock(args->print_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
}

int	eat_routine(t_philosopher_args	*args)
{
	pick_up_forks(args);
	pthread_mutex_lock(&args->philo->meal_mutex);
	args->philo->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	if (args->sim_params->hunger_check == ON)
	{
		pthread_mutex_lock(&args->sim_params->finished_mutex);
		args->sim_params->total_meals_eaten++;
		args->philo->meals_to_eat--;
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
			args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
	}
	pthread_mutex_unlock(&args->philo->meal_mutex);
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	drop_forks(args);
	pthread_mutex_lock(&args->sim_params->finished_mutex);
	if (args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
	{
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
		return (print_state(args, SLEEPING), 0);
	}
	return (1);
}

void	sleep_routine(t_philosopher_args *args)
{
	pthread_mutex_lock(&args->sim_params->finished_mutex);
	if (args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL)
	{
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
	}
	else
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
}
