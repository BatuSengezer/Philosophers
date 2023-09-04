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

void	print_exit(char *str)
{
	printf("%s\n", str);
	exit(1);
}

void	death_and_finished_lock(t_philosopher_args *args)
{
	pthread_mutex_lock(&args->sim_params->death_mutex);
	pthread_mutex_lock(&args->sim_params->finished_mutex);
}

void	death_and_finished_unlock(t_philosopher_args *args)
{
	pthread_mutex_unlock(&args->sim_params->finished_mutex);
	pthread_mutex_unlock(&args->sim_params->death_mutex);
}
