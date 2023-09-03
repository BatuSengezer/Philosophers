/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/09/02 18:24:55 by bsengeze         ###   ########.fr       */
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
		// && args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL
	else if (state == DIED && args->philo->death_state == EVERYONE_ALIVE)
		printf("died\n");
	else if (state == HAS_FORK && args->philo->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	else if (state == HAS_FORKS && args->philo->death_state == EVERYONE_ALIVE)
		printf("has taken a fork\n");
	pthread_mutex_unlock(args->print_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
}

void	eat(t_philosopher_args	*args)
{
	pick_up_forks(args);
	pthread_mutex_lock(&args->philo->meal_mutex);
	args->philo->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	if (args->sim_params->hunger_check == ON)
	{
		args->sim_params->total_meals_eaten++;
		args->philo->meals_to_eat--;
		pthread_mutex_lock(&args->sim_params->finished_mutex);
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
			args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
	}
	pthread_mutex_unlock(&args->philo->meal_mutex);
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	drop_forks(args);
}
