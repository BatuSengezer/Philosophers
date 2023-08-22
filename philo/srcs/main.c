#include "philosophers.h"

//allocate memory for philosophers, forks, and args
void	allocate(t_simulation_parameters *sim_params)
{
	sim_params->philosophers = (t_philosopher *)malloc(sizeof(t_philosopher)
			* sim_params->number_of_philosophers);
	if (!sim_params->philosophers)
		print_exit("Error: malloc philosophers failed\n");
	sim_params->forks = (t_fork *)malloc(sizeof(t_fork)
			* sim_params->number_of_philosophers);
	if (!sim_params->forks)
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
		sim_params->number_of_times_each_philo_must_eat = 0;
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
	// sim_params->death_state = EVERYONE_ALIVE;
	sim_params->total_meals_to_be_eaten = sim_params->number_of_philosophers
		* sim_params->number_of_times_each_philo_must_eat;
	if (gettimeofday(&sim_params->start_time, NULL) == -1)
		print_exit("Error: gettimeofday failed\n");
	allocate(sim_params);
	if (pthread_mutex_init(&sim_params->print_mutex, NULL))
		print_exit("Error: pthread_mutex_init failed\n");
}

void	init_philosophers_and_forks(t_simulation_parameters *sim_params, int i)
{
	sim_params->philosophers[i].id = i + 1;
	sim_params->philosophers[i].state = THINKING;
	if (i == 0)
	{
		sim_params->philosophers[i].fork_left = &sim_params->forks[i];
		sim_params->philosophers[i].fork_right = &sim_params->forks[
			sim_params->number_of_philosophers - 1];
	}
	else
	{
		sim_params->philosophers[i].fork_left = &sim_params->forks[i];
		sim_params->philosophers[i].fork_right = &sim_params->forks[i - 1];
	}
	sim_params->philosophers[i].last_meal_timestamp
		= current_timestamp(sim_params->start_time);
	sim_params->philosophers[i].meals_eaten = 0;
	// sim_params->forks[i].state = FREE;
	sim_params->forks[i].id = i + 1;
	if (pthread_mutex_init(&sim_params->forks[i].mutex_fork, NULL))
		print_exit("Error: pthread_mutex_init failed\n");
}

void	simulation(t_simulation_parameters *sim_params)
{
	long long	i;

	i = -1;
	if (pthread_create(&sim_params->monitor_thread, NULL, monitor_death, sim_params->args))
    	print_exit("Error: pthread_create for monitor thread failed\n");
	while (++i < sim_params->number_of_philosophers)
	{
		init_philosophers_and_forks(sim_params, i);
		sim_params->args[i].philosopher = &sim_params->philosophers[i];
		sim_params->args[i].sim_params = sim_params;
		sim_params->args[i].print_mutex = &sim_params->print_mutex;
		if (pthread_create(&sim_params->philosophers[i].p_thread, NULL,
				eat_sleep_think, &sim_params->args[i]))
			print_exit("Error: pthread_create failed\n");
	}
	i = -1;
	while (++i < sim_params->number_of_philosophers)
	{
		if (pthread_join(sim_params->philosophers[i].p_thread, NULL))
			print_exit("Error: pthread_join failed\n");
	}
	if (pthread_join(sim_params->monitor_thread, NULL))
        print_exit("Error: pthread_join for monitor thread failed\n");
}

void *monitor_death(void *arg) 
{
    t_philosopher_args	*args;
	long long i;

	args = (t_philosopher_args *)arg;
    while (1) 
	{
		i = 0;
		while (i < args->sim_params->number_of_philosophers)
		{
       		if (current_timestamp(args->sim_params->start_time)
				- args->philosopher[i].last_meal_timestamp
				>= args->sim_params->time_to_die)
			{
				args->philosopher[i].state = DIED;
				// args->sim_params->death_state = SOMEONE_DIED;
				print_state(args, DIED);
			}
			i++;
		}
		if (args->sim_params->hunger_state == PHILOSOPHERS_ARE_FULL)
			break;
    }
	return (NULL);
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
	// printf("%lld Exiting program\n", current_timestamp(sim_params.start_time));
	return (0);
}
