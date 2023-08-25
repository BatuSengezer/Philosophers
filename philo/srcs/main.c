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

//allocate memory for philosophers, forks, and args
void	allocate(t_simulation_parameters *sim_params)
{
	sim_params->philosophers = (t_philosopher *)malloc(sizeof(t_philosopher)
			* sim_params->number_of_philosophers);
	if (!sim_params->philosophers)
		print_exit("Error: malloc philosophers failed\n");
	sim_params->forks_mutexes = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)
			* sim_params->number_of_philosophers);
	if (!sim_params->forks_mutexes)
		print_exit("Error: malloc forks failed\n");
	sim_params->args = (t_philosopher_args *)malloc(sizeof(t_philosopher_args)
			* sim_params->number_of_philosophers);
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
		sim_params->number_of_times_each_philo_must_eat = -1;
	}
	if (argc == 6)
	{
		sim_params->hunger_check = ON;
		sim_params->hunger_state = PHILOSOPHERS_NOT_FULL_YET;
		sim_params->number_of_times_each_philo_must_eat = ft_atoi(argv[5]);
	}
	sim_params->number_of_philosophers = ft_atoi(argv[1]);
	sim_params->time_to_die = ft_atoi(argv[2]);
	sim_params->time_to_eat = ft_atoi(argv[3]);
	sim_params->time_to_sleep = ft_atoi(argv[4]);
	sim_params->total_meals_to_be_eaten = sim_params->number_of_philosophers
		* sim_params->number_of_times_each_philo_must_eat;
	if (gettimeofday(&sim_params->start_time, NULL) == -1)
		print_exit("Error: gettimeofday failed\n");
	allocate(sim_params);
	if (pthread_mutex_init(&sim_params->print_mutex, NULL))
		print_exit("Error: pthread_mutex_init failed\n");
	if (pthread_mutex_init(&sim_params->death_mutex, NULL))
		print_exit("Error: pthread_mutex_init failed\n");
}

void	init_philosophers_and_forks(t_simulation_parameters *sim_params)
{
	int i;

	i = -1;
	while(++i < sim_params->number_of_philosophers)
		pthread_mutex_init(&sim_params->forks_mutexes[i], NULL);
	i = -1;
	while(++i < sim_params->number_of_philosophers)
	{
		pthread_mutex_init(&sim_params->philosophers[i].meal_mutex, NULL);
		if(sim_params->number_of_times_each_philo_must_eat == -1)
			sim_params->philosophers[i].meals_to_eat = -1;
		else
			sim_params->philosophers[i].meals_to_eat = sim_params->number_of_times_each_philo_must_eat;	
		sim_params->philosophers[i].id = i + 1;
		sim_params->philosophers[i].state = THINKING;
		sim_params->philosophers[i].death_state = EVERYONE_ALIVE;
		if (i == 0)
		{
			sim_params->philosophers[i].fork_left = &sim_params->forks_mutexes[i];
			sim_params->philosophers[i].fork_right = &sim_params->forks_mutexes[
			sim_params->number_of_philosophers - 1];
		}
		else
		{
			sim_params->philosophers[i].fork_left = &sim_params->forks_mutexes[i];
			sim_params->philosophers[i].fork_right = &sim_params->forks_mutexes[i - 1];
		}
		sim_params->philosophers[i].last_meal_timestamp
		= current_timestamp(sim_params->start_time);
		sim_params->philosophers[i].meals_eaten = 0;
		sim_params->args[i].philosopher = &sim_params->philosophers[i];
		sim_params->args[i].sim_params = sim_params;
		sim_params->args[i].print_mutex = &sim_params->print_mutex;
	}
}

void	simulation(t_simulation_parameters *sim_params)
{
	long long	i;

	init_philosophers_and_forks(sim_params);
	i = -1;
	while (++i < sim_params->number_of_philosophers)
	{
		pthread_create(&sim_params->philosophers[i].p_thread, NULL,
				eat_sleep_think, &sim_params->args[i]);
	}
	i = -1;
	while (++i < sim_params->number_of_philosophers)
	{
		pthread_create(&sim_params->philosophers[i].monitor_thread, NULL,
				monitor_death, &sim_params->args[i]);
	}
	i = -1;
	while (++i < sim_params->number_of_philosophers)
	{
		if (pthread_join(sim_params->philosophers[i].p_thread, NULL))
			print_exit("Error: pthread_join failed\n");
	}
	i = -1;
	while (++i < sim_params->number_of_philosophers)
	{
		if (pthread_join(sim_params->philosophers[i].monitor_thread, NULL))
			print_exit("Error: pthread_join failed\n");
	}
}

int	main(int argc, char **argv)
{
	t_simulation_parameters	sim_params;

	input_check(argc, argv);
	init_sim_param(&sim_params, argc, argv);
	simulation(&sim_params);
	destroy_free(&sim_params);
	if (sim_params.hunger_check == ON
		&& sim_params.hunger_state == PHILOSOPHERS_ARE_FULL)
		printf("Everyone ate enough\n");
	return (0);
}
