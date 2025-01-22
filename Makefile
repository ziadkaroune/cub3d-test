CC = cc
NAME	=	Cub3D	
CFLAGS 	= -Wall -Wextra -Werror -DGL_SILENCE_DEPRECATION -I$(MLX_INC)
MLX_DIR = mlx  
MLX_LIB = $(MLX_DIR)/libmlx.a
MLX_INC = $(MLX_DIR) 
RM	=	rm -f

LIBS = -L$(MLX_DIR) -lmlx -framework OpenGL -framework AppKit

SRC = main.c	
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LIBS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(NAME) $(OBJ)

fclean:	clean
		$(RM) $(NAME) $(OBJ)



re: clean all
