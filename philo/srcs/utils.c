/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 21:55:11 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/24 19:54:40 by bsengeze         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	input_check(int argc, char **argv)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	if (argc < 5 || argc > 6)
		print_exit("Usage: argument count can be 5 or 6\n");
	while (++i < argc)
	{
		if (argv[i][0] == '-')
			print_exit("Error: arguments an only be positive integer\n");
		while (argv[i][j])
		{
			while (argv[i][j] == '+')
				j++;
			if (argv[i][j] < '0' || argv[i][j] > '9')
				print_exit("Error: arguments an only be positive integer\n");
			j++;
		}
		j = 0;
	}
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
		current_timestamp(args->sim_params->start_time), args->philosopher->id);
	usleep(args->sim_params->time_to_die * 1000);
	printf("%lld %d died\n", current_timestamp(args->sim_params->start_time),
		args->philosopher->id);
}

void	*monitor_death(void *arg)
{
	t_philosopher_args	*args;
	long long			i;
	long long			j;
	args = (t_philosopher_args *)arg;
	while (1)
	{
		i = 0;
		while (i < args->sim_params->number_of_philosophers)
		{
			//maybe problem here mutex lock
			pthread_mutex_lock(&args->philosopher->meal_mutex);
			if (current_timestamp(args->sim_params->start_time)
				- args->philosopher->last_meal_timestamp
				>= args->sim_params->time_to_die)
			{
				args->philosopher->state = DIED;
				print_state(&args[i], DIED);
				pthread_mutex_lock(&args->sim_params->death_mutex);
				j = -1;
				while (++j < args->sim_params->number_of_philosophers)
					args->sim_params->philosophers[j].death_state = SOMEONE_DIED;
				//can do death_check everyone is alive
				pthread_mutex_unlock(&args->sim_params->death_mutex);
			}
			pthread_mutex_unlock(&args->philosopher->meal_mutex);
			i++;
		}
		// might need to add a mutex unlock here in if and after while loop
		if (args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			break ;
	}
	return (NULL);
}

long long	ft_atoi(const char *nptr)
{
	long long	result;
	int			neg;

	result = 0;
	neg = 1;
	while ((*nptr >= 9 && *nptr <= 13) || *nptr == ' ')
		nptr++;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr++ == '-')
			neg *= -1;
	}
	while (*nptr >= '0' && *nptr <= '9')
		result = result * 10 + neg * (*nptr++ - '0');
	return (result);
}
