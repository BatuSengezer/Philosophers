/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bsengeze <bsengeze@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 21:55:11 by bsengeze          #+#    #+#             */
/*   Updated: 2023/08/25 02:05:42 by bsengeze         ###   ########.fr       */
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
