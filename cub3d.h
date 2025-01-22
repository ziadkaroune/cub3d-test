#ifndef CUB3D_H
# define CUB3D_H


# include "mlx.h"
# include <errno.h>
# include <fcntl.h>
# include <math.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#include "./mlx/mlx.h"


#define ARGS  "error args ./cub3d ./mapexample"
#define WIDTH   500
#define HEIGTH  500
#define SPEED  3
#define MAP_WIDTH 6
#define MAP_HEIGHT 5

int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1}
};

typedef struct s_player {
    double x, y;    // Position
    double dirX, dirY; // Direction vector
    double planeX, planeY; // Camera plane (for FOV)
}               t_player;

t_player player = {2.5, 2.5, -1, 0, 0, 0.66};  
 
void cast_rays(void *mlx, void *win);
#endif