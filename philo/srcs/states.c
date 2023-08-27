/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:51:31 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/26 14:21:22 by bsengeze         ###   ########.fr       */
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

int	pick_up_forks(t_philosopher_args *args)
{
	//this if solves fork problem
	// prevent philos from immediately trying 
	// to eat or pick up forks too frequently
	// can add && args->sim_params->number_of_philos %2 == 1 to if cond
	// it increased survivability for even num of philos
	if (current_timestamp(args->sim_params->start_time) 
		- args->philo->last_meal_timestamp 
		< args->sim_params->time_to_eat + args->sim_params->time_to_sleep + 10)
		usleep(500);
	if (args->philo->id % 2 == 0)
	{
		pthread_mutex_lock(args->philo->fork_right);
		args->philo->state = HAS_FORK; // no need for these
		print_state(args, HAS_FORK);
		pthread_mutex_lock(args->philo->fork_left);
		args->philo->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	if (args->philo->id % 2 == 1)
	{
		pthread_mutex_lock(args->philo->fork_left);
		print_state(args, HAS_FORK);
		args->philo->state = HAS_FORK;
		pthread_mutex_lock(args->philo->fork_right);
		args->philo->state = HAS_FORKS;
		print_state(args, HAS_FORKS);
	}
	return (1);
}

void	eat(t_philosopher_args	*args)
{
	pick_up_forks(args);
	pthread_mutex_lock(&args->philo->meal_mutex);
	args->philo->last_meal_timestamp = current_timestamp(
			args->sim_params->start_time);
	if (args->sim_params->hunger_check == ON)
	{
		args->philo->meals_eaten++;
		args->sim_params->total_meals_eaten++;
		args->philo->meals_to_eat--;
		if (args->sim_params->total_meals_eaten >= args->sim_params
			->total_meals_to_be_eaten)
			args->sim_params->hunger_state = PHILOSOPHERS_ARE_FULL;
	}
	print_state(args, EATING);
	usleep(args->sim_params->time_to_eat * 1000);
	pthread_mutex_unlock(&args->philo->meal_mutex);
	if (args->philo->id % 2 == 1)
		if (pthread_mutex_unlock(args->philo->fork_right) 
			|| pthread_mutex_unlock(args->philo->fork_left))
			print_exit("Error: could not unlock fork mutex\n");
	if (args->philo->id % 2 == 0)
		if (pthread_mutex_unlock(args->philo->fork_left) 
			|| pthread_mutex_unlock(args->philo->fork_right))
			print_exit("Error: could not unlock fork mutex\n");
}

void	*eat_sleep_think(void *arg)
{
	t_philosopher_args		*args;

	args = (t_philosopher_args *)arg;
	if (args->sim_params->number_of_philos == 1)
	{
		handle_single_philosopher_case(args);
		return (NULL);
	}
	pthread_mutex_lock(&args->sim_params->death_mutex);
	//
	while (args->philo->death_state == EVERYONE_ALIVE 
		&& args->philo->meals_to_eat)
	{
		pthread_mutex_unlock(&args->sim_params->death_mutex);
		print_state(args, THINKING); // can add state instead of thinking
		eat(args);
		print_state(args, SLEEPING);
		usleep(args->sim_params->time_to_sleep * 1000);
		// args->philo->state = THINKING;
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
		pthread_mutex_lock(&args->philo->meal_mutex);
		if (args->philo->meals_to_eat 
			&& (current_timestamp(args->sim_params->start_time)
				- args->philo->last_meal_timestamp
				> args->sim_params->time_to_die))
		{
			print_state(args, DIED);
			pthread_mutex_lock(&args->sim_params->death_mutex);
			j = -1;
			while (++j < args->sim_params->number_of_philos)
				args->sim_params->philos[j].death_state = SOMEONE_DIED;
			pthread_mutex_unlock(&args->sim_params->death_mutex);
			//test without usleep(70) to see if it increases efficiency;
			// usleep(70);
			pthread_mutex_unlock(&args->philo->meal_mutex);
			return (NULL);
		}
		if (!args->philo->meals_to_eat)
			return (pthread_mutex_unlock(&args->philo->meal_mutex), NULL);
		pthread_mutex_unlock(&args->philo->meal_mutex);
	}
}
