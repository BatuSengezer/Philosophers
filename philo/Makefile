NAME = philo
CC = cc
RM = rm -rf
FLAGS = -Wall -Wextra -Werror -g 
OBJ_DIR = objs/
SRC_DIR = srcs/

SRC 	= philosophers.c init.c states.c forks.c utils.c utils_2.c

SRCS	= $(addprefix $(SRC_DIR), $(SRC))

OBJ		= $(SRC:.c=.o)
OBJS 	= $(addprefix $(OBJ_DIR), $(OBJ))

all: $(OBJ_DIR) $(NAME) 

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(NAME): $(OBJS)
		$(CC) $(FLAGS) $(OBJS) -o $(NAME)
#		$(CC) $(FLAGS) -fsanitize=thread -lpthread $(OBJS) -o $(NAME) 

clean:
		$(RM) $(OBJ_DIR)

fclean: clean
		$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re