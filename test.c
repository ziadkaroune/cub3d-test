#include "mlx.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define MOVE_SPEED 0.1  
#define ROT_SPEED 0.05

// Key codes for movement
#define KEY_LEFT 65361
#define KEY_RIGHT 65363
#define KEY_W 119
#define KEY_S 115
#define KEY_A 97
#define KEY_D 100
#define KEY_ESC 65307

// World map: 1 represents walls, 0 represents empty spaces
#define MAP_WIDTH 14
#define MAP_HEIGHT 5
 
int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

typedef struct s_img {
    void *img;
    char *addr;
    int buts_per_pixel;
    int line_length;
    int endian;
} t_img;

typedef struct s_player {
    double translateX;
    double translateY;
    double dirX, dirY;
    double planeX, planeY;
} t_player;

typedef struct s_mlx_para {
    void *mlx;
    void *window;
    t_player player;
    t_img image;
} t_mlx_para;

void init_data(t_mlx_para *data) {
    data->mlx = NULL;
    data->window = NULL;
    data->image.img = NULL;
    data->image.buts_per_pixel = 0;
    data->image.line_length = 0;
    data->image.endian = 0;

    data->player.translateX = 3.5;
    data->player.translateY = 3.5;
    data->player.dirX = -1;
    data->player.dirY = 0.0;
    data->player.planeX = 0.0;
    data->player.planeY = 0.6;
}

void clean_exit(t_mlx_para *data) {
    if (!data)
        exit(0);
    if (data->window && data->mlx)
        mlx_destroy_window(data->mlx, data->window);
    if (data->mlx) {
        mlx_destroy_display(data->mlx);
        mlx_loop_end(data->mlx);
        free(data->mlx);
    }
    free(data);
    exit(0);
}

 int is_wall(double x, double y) {
    int mapX = (int)x;
    int mapY = (int)y;
    if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT)
        return 1;  // Out of bounds, treat as wall
    return worldMap[mapY][mapX] == 1;  // Check if it's a wall
}

int handle_key(int keycode, t_player *player) {
    double newTranslateX = player->translateX;
    double newTranslateY = player->translateY;

    if (keycode == 65307)  // ESC key to exit
        exit(0);

    if (keycode == 119) {  // W key to move forward
        newTranslateX += player->dirX * MOVE_SPEED;
        newTranslateY += player->dirY * MOVE_SPEED;
        if (!is_wall(newTranslateX, newTranslateY)) {
            player->translateX = newTranslateX;
            player->translateY = newTranslateY;
        }
    }

    if (keycode == 115) {  // S key to move backward
        newTranslateX -= player->dirX * MOVE_SPEED;
        newTranslateY -= player->dirY * MOVE_SPEED;
        if (!is_wall(newTranslateX, newTranslateY)) {
            player->translateX = newTranslateX;
            player->translateY = newTranslateY;
        }
    }

   if (keycode == 97) {  // A key to rotate left (counter-clockwise)
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(-ROT_SPEED) - player->dirY * sin(-ROT_SPEED);
    player->dirY = oldDirX * sin(-ROT_SPEED) + player->dirY * cos(-ROT_SPEED);

    // Update the camera plane (to ensure movement happens in the rotated direction)
    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(-ROT_SPEED) - player->planeY * sin(-ROT_SPEED);
    player->planeY = oldPlaneX * sin(-ROT_SPEED) + player->planeY * cos(-ROT_SPEED);
}

if (keycode == 100) {  // D key to rotate right (clockwise)
    double oldDirX = player->dirX;
    player->dirX = player->dirX * cos(ROT_SPEED) - player->dirY * sin(ROT_SPEED);
    player->dirY = oldDirX * sin(ROT_SPEED) + player->dirY * cos(ROT_SPEED);

    // Update the camera plane (to ensure movement happens in the rotated direction)
    double oldPlaneX = player->planeX;
    player->planeX = player->planeX * cos(ROT_SPEED) - player->planeY * sin(ROT_SPEED);
    player->planeY = oldPlaneX * sin(ROT_SPEED) + player->planeY * cos(ROT_SPEED);
}
    return 0;
}


void img_pixel_put(t_img *img, int x, int y, int color) {
    if (x >= 0 && x < WIN_WIDTH && y >= 0 && y < WIN_HEIGHT) {
        char *dst = img->addr + (y * img->line_length + x * (img->buts_per_pixel / 8));
        *(unsigned int *)dst = color;
    }
}

void clear_image(t_img *img) {
    for (int y = 0; y < WIN_HEIGHT; y++) {
        for (int x = 0; x < WIN_WIDTH; x++)
            img_pixel_put(img, x, y, 0x000000);
    }
}

void cast_rays(t_player *player, t_img *img) {
    for (int x = 0; x < WIN_WIDTH; x++) {
        double cameraX = 2 * x / (double)WIN_WIDTH - 1;
        double rayDirX = player->dirX + player->planeX * cameraX;
        double rayDirY = player->dirY + player->planeY * cameraX;

        int mapX = (int)player->translateX;
        int mapY = (int)player->translateY;

        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT)
            continue;

        double sideDistX, sideDistY;
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);
        double perpWallDist;

        int stepX, stepY;
        int hit = 0;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->translateX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->translateX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->translateY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player->translateY) * deltaDistY;
        }

        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (worldMap[mapY][mapX] > 0) hit = 1;
        }

        if (side == 0)
            perpWallDist = (mapX - player->translateX + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - player->translateY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(WIN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + WIN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + WIN_HEIGHT / 2;
        if (drawEnd >= WIN_HEIGHT) drawEnd = WIN_HEIGHT - 1;

        int color;
        if (side == 0)
            color = 0xFF0000;
        else
            color = 0x0000FF;

        for (int y = drawStart; y <= drawEnd; y++)
            img_pixel_put(img, x, y, color);
    }
}

int render_scene(t_mlx_para *data) {
    clear_image(&data->image);
    cast_rays(&data->player, &data->image);
    mlx_put_image_to_window(data->mlx, data->window, data->image.img, 0, 0);
    return 0;
}

int main() {
    t_mlx_para mlx;
    t_img image;
    init_data(&mlx);
    mlx.mlx = mlx_init();
    if (!mlx.mlx)
        clean_exit(&mlx);
    mlx.window = mlx_new_window(mlx.mlx, WIN_WIDTH, WIN_HEIGHT, "cub3d");
    if (!mlx.window)
        clean_exit(&mlx);

    image.img = mlx_new_image(mlx.mlx, WIN_WIDTH, WIN_HEIGHT);
    image.addr = mlx_get_data_addr(image.img, &image.buts_per_pixel, &image.line_length, &image.endian);
    if (!image.addr) {
        fprintf(stderr, "Failed to get image data address.\n");
        clean_exit(&mlx);
    }

    mlx.image = image;

    mlx_hook(mlx.window, 2, 1L << 0, handle_key, &mlx.player);
    mlx_loop_hook(mlx.mlx, render_scene, &mlx);
    mlx_loop(mlx.mlx);

    return 0;
}
