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

//allocate memory for philos, forks, and args
void	allocate(t_simulation_parameters *sim_params)
{
	sim_params->philos = (t_philosopher *)malloc(sizeof(t_philosopher)
			* sim_params->number_of_philos);
	if (!sim_params->philos)
		print_exit("Error: malloc philos failed\n");
	sim_params->forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* sim_params->number_of_philos);
	if (!sim_params->forks)
		print_exit("Error: malloc forks failed\n");
	sim_params->args = (t_philosopher_args *)malloc(sizeof(t_philosopher_args)
			* sim_params->number_of_philos);
	if (!sim_params->args)
		print_exit("Error: malloc args failed\n");
}

void	init_sim_param(t_simulation_parameters *sim_params,
		int argc, char **argv)
{
	if (argc == 5)
	{
		sim_params->hunger_check = OFF;
		sim_params->hunger_state = PHILOSOPHERS_NOT_FULL_YET;
		sim_params->meals_to_eat_each = 0;
	}
	if (argc == 6)
	{
		sim_params->hunger_check = ON;
		sim_params->hunger_state = PHILOSOPHERS_NOT_FULL_YET;
		sim_params->meals_to_eat_each = ft_atoi(argv[5]);
	}
	sim_params->number_of_philos = ft_atoi(argv[1]);
	sim_params->time_to_die = ft_atoi(argv[2]);
	sim_params->time_to_eat = ft_atoi(argv[3]);
	sim_params->time_to_sleep = ft_atoi(argv[4]);
	sim_params->total_meals_to_be_eaten = sim_params->number_of_philos
		* sim_params->meals_to_eat_each;
	gettimeofday(&sim_params->start_time, NULL);
}

void	init_mutexes(t_simulation_parameters *sim_params)
{
	int	i;

	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		pthread_mutex_init(&sim_params->forks[i], NULL);
		pthread_mutex_init(&sim_params->philos[i].meal_mutex, NULL);
	}
	pthread_mutex_init(&sim_params->print_mutex, NULL);
	pthread_mutex_init(&sim_params->death_mutex, NULL);
	pthread_mutex_init(&sim_params->finished_mutex, NULL);
}

void	init_philos(t_simulation_parameters *sim_params)
{
	int	i;

	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		sim_params->philos[i].meals_to_eat = sim_params->meals_to_eat_each;
		if (sim_params->meals_to_eat_each == 0)
			sim_params->philos[i].meals_to_eat = -1;
		sim_params->philos[i].id = i + 1;
		sim_params->philos[i].state = THINKING;
		sim_params->philos[i].death_state = EVERYONE_ALIVE;
		if (i == 0)
		{
			sim_params->philos[i].fork_left = &sim_params->forks[i];
			sim_params->philos[i].fork_right = &sim_params->forks[
				sim_params->number_of_philos - 1];
		}
		else
		{
			sim_params->philos[i].fork_left = &sim_params->forks[i];
			sim_params->philos[i].fork_right = &sim_params->forks[i - 1];
		}
		sim_params->philos[i].last_meal_timestamp
			= current_timestamp(sim_params->start_time);
	}
}

void	init_args(t_simulation_parameters *sim_params)
{
	int	i;

	i = -1;
	while (++i < sim_params->number_of_philos)
	{
		sim_params->args[i].philo = &sim_params->philos[i];
		sim_params->args[i].sim_params = sim_params;
		sim_params->args[i].print_mutex = &sim_params->print_mutex;
	}
}

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
	// if (sim_params.hunger_check == ON
	// 	&& sim_params.hunger_state == PHILOSOPHERS_ARE_FULL)
	// 	printf("%lld Everyone ate enough\n", current_timestamp(sim_params.start_time));
	return (0);
}
