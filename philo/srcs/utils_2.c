/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 21:55:24 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/25 00:55:58 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

// add proper exit
void	print_exit(char *str)
{
	printf("%s\n", str);
	exit(1);
}

void	destroy_free(t_simulation_parameters *sim_params)
{
	int	i;

	if (pthread_mutex_destroy(&sim_params->print_mutex))
		print_exit("Error: pthread_mutex_destroy failed\n ");
	if (pthread_mutex_destroy(&sim_params->finished_mutex))
		print_exit("Error: pthread_mutex_destroy failed\n ");
	if (pthread_mutex_destroy(&sim_params->death_mutex))
		print_exit("Error: pthread_mutex_destroy failed\n ");	
	i = -1;	
	while (++i < sim_params->number_of_philos)
	{
		if (pthread_mutex_destroy(&sim_params->forks[i]))
			print_exit("Error: pthread_mutex_destroy failed\n ");
		if (pthread_mutex_destroy(&sim_params->philos[i].meal_mutex))
			print_exit("Error: pthread_mutex_destroy failed\n ");
	}
	free(sim_params->philos);
	free(sim_params->forks);
	free(sim_params->args);
}

long long	current_timestamp(struct timeval start_time)
{
	struct timeval	now;
	long long		elapsed_time;

	gettimeofday(&now, NULL);
	elapsed_time = (now.tv_sec - start_time.tv_sec) * 1000;
	elapsed_time += (now.tv_usec - start_time.tv_usec) / 1000;
	return (elapsed_time);
}

void	handle_single_philosopher_case(t_philosopher_args *args)
{
	printf("%lld %d has taken a fork\n",
		current_timestamp(args->sim_params->start_time), args->philo->id);
	usleep(args->sim_params->time_to_die * 1000);
	printf("%lld %d died\n", current_timestamp(args->sim_params->start_time),
		args->philo->id);
}
