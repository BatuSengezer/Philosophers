#include "philosophers.h"

void	print_exit(char *str)
{
	printf("%s\n", str);
	exit(1);
}

void	destroy_free(t_simulation_parameters *sim_params)
{
	int	i;

	i = 0;
	if (pthread_mutex_destroy(&sim_params->print_mutex))
		print_exit("Error: pthread_mutex_destroy failed\n ");
	while (i < sim_params->number_of_philosophers)
	{
		if (pthread_mutex_destroy(&sim_params->forks[i].mutex_fork))
			print_exit("Error: pthread_mutex_destroy failed\n ");
		i++;
	}
	free(sim_params->philosophers);
	free(sim_params->forks);
	free(sim_params->args);
}
