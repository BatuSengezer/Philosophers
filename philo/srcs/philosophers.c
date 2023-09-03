/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 20:47:42 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/25 04:45:54 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	simulation(t_simulation_parameters *sim_params)
{
	long long	i;

	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		pthread_create(&sim_params->philos[i].p_thread, NULL,
			eat_sleep_think, &sim_params->args[i]);
	}
	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		pthread_create(&sim_params->philos[i].monitor_thread, NULL,
			monitor_death, &sim_params->args[i]);
	}
	i = -1;
	while (++i < sim_params->number_of_philos)
		pthread_join(sim_params->philos[i].p_thread, NULL);
	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		pthread_join(sim_params->philos[i].monitor_thread, NULL);
	}
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
	print_state(args, THINKING);
	pthread_mutex_lock(&args->sim_params->death_mutex);
	pthread_mutex_lock(&args->sim_params->finished_mutex);
	while (args->philo->death_state == EVERYONE_ALIVE 
		&& (args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL
		|| args->philo->meals_to_eat))
	{
		pthread_mutex_unlock(&args->sim_params->finished_mutex);
		pthread_mutex_unlock(&args->sim_params->death_mutex);
		pthread_mutex_lock(&args->sim_params->finished_mutex);
		if (args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL
			|| args->philo->meals_to_eat)
		// if (args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL)
		// if (args->philo->meals_to_eat)
		{
			pthread_mutex_unlock(&args->sim_params->finished_mutex);
			eat(args);
			pthread_mutex_lock(&args->sim_params->finished_mutex);
			if (args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			{
				pthread_mutex_unlock(&args->sim_params->finished_mutex);
				print_state(args, SLEEPING);
				// printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
				return (NULL);
			}
			pthread_mutex_unlock(&args->sim_params->finished_mutex);
		}
		else
			pthread_mutex_unlock(&args->sim_params->finished_mutex);
		pthread_mutex_lock(&args->sim_params->finished_mutex);
		if (args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL)
		{
			pthread_mutex_unlock(&args->sim_params->finished_mutex);
			print_state(args, SLEEPING);
			usleep(args->sim_params->time_to_sleep * 1000);
		}
		else
			pthread_mutex_unlock(&args->sim_params->finished_mutex);
		print_state(args, THINKING);
		pthread_mutex_lock(&args->sim_params->death_mutex);
		pthread_mutex_lock(&args->sim_params->finished_mutex);
	}
	pthread_mutex_unlock(&args->sim_params->finished_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
	return (NULL);
}

void	*monitor_death(void *arg)
{
	t_philosopher_args	*args;
	int					i;

	args = (t_philosopher_args *)arg;
	while (1)
	{
		pthread_mutex_lock(&args->philo->meal_mutex);
		if (args->philo->meals_to_eat
			&& (current_timestamp(args->sim_params->start_time)
				- args->philo->last_meal_timestamp
				> args->sim_params->time_to_die) && args->sim_params->hunger_state != PHILOSOPHERS_ARE_FULL)
		{
			// printf("%d total meals eaten\n", args->sim_params->total_meals_eaten);
			// printf("%d meals to eat\n", args->philo->meals_to_eat);

			print_state(args, DIED);
			pthread_mutex_lock(&args->sim_params->death_mutex);
			i = -1;
			while (++i < args->sim_params->number_of_philos)
				args->sim_params->philos[i].death_state = SOMEONE_DIED;
			pthread_mutex_unlock(&args->sim_params->death_mutex);
			pthread_mutex_unlock(&args->philo->meal_mutex);
			return (NULL);
		}
		if (!args->philo->meals_to_eat || args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			return (pthread_mutex_unlock(&args->philo->meal_mutex), NULL);
		pthread_mutex_unlock(&args->philo->meal_mutex);
		usleep(500);
	}
}

int	main(int argc, char **argv)
{
	t_simulation_parameters	sim_params;

	input_check(argc, argv);
	init_sim_param(&sim_params, argc, argv);
	allocate(&sim_params);
	init_mutexes(&sim_params);
	init_philos(&sim_params);
	init_args(&sim_params);
	simulation(&sim_params);
	destroy_free(&sim_params);
	return (0);
}
