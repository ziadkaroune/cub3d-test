#include "mlx.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define MAP_WIDTH 6
#define MAP_HEIGHT 5

// Key codes for arrow keys
#define KEY_LEFT  0
#define KEY_RIGHT 17

// World map: 1 represents walls, 0 represents empty spaces
int worldMap[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1}
};

// Player structure
typedef struct s_player {
    double x, y;        // Player's position
    double dirX, dirY;  // Direction vector
    double planeX, planeY; // Camera plane for FOV
}               t_player;

// Global variables
void *mlx;
void *win;
t_player player;

// Function to draw a vertical line
void draw_vertical_line(int x, int drawStart, int drawEnd, int color)
{
    for (int y = drawStart; y < drawEnd; y++)
        mlx_pixel_put(mlx, win, x, y, color);
}

// Function to cast rays and render the scene
void cast_rays()
{
    for (int x = 0; x < WIN_WIDTH; x++) // For each vertical line on the screen
    {
        double cameraX = 2 * x / (double)WIN_WIDTH - 1; // Map x to camera space (-1 to 1)
        double rayDirX = player.dirX + player.planeX * cameraX;
        double rayDirY = player.dirY + player.planeY * cameraX;

        // Which box of the map we are in
        int mapX = (int)player.x;
        int mapY = (int)player.y;

        // Length of ray from one x-side to next x-side or y-side
        double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1 / rayDirY);

        double sideDistX, sideDistY;

        // Step and initial sideDist
        int stepX, stepY;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (player.x - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player.x) * deltaDistX;
        }

        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (player.y - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player.y) * deltaDistY;
        }

        // Perform DDA
        int hit = 0;
        int side; // Was a NS or a EW wall hit?

        while (hit == 0)
        {
            // Jump to next map square
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            // Check if the ray has hit a wall
            if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT)
            {
                if (worldMap[mapY][mapX] > 0)
                    hit = 1;
            }
            else
            {
                // If out of bounds, stop the ray
                hit = 1;
            }
        }

        // Calculate distance to the point of impact
        double perpWallDist;
        if (side == 0)
            perpWallDist = (mapX - player.x + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - player.y + (1 - stepY) / 2) / rayDirY;

        // Calculate height of the line to draw on screen
        int lineHeight = (int)(WIN_HEIGHT / perpWallDist);

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + WIN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + WIN_HEIGHT / 2;
        if (drawEnd >= WIN_HEIGHT) drawEnd = WIN_HEIGHT - 1;

        // Choose wall color based on side
        int color = (side == 0) ? 0xFF0000 : 0x00FF00; // Red for NS walls, green for EW walls

        // Draw the vertical line for the wall slice
        draw_vertical_line(x, drawStart, drawEnd, color);
    }
}

// Function to render the scene
int render_scene()
{
    mlx_clear_window(mlx, win); // Clear the window
    cast_rays(); // Cast rays and render the walls
    return (0);
}

// Function to handle key press events
int handle_key(int keycode)
{
    double oldDirX, oldPlaneX;
    double rotationSpeed = 0.05; // Rotation speed

    if (keycode)
    {
        // Rotate to the left
        oldDirX = player.dirX;
        player.dirX = player.dirX * cos(-rotationSpeed) - player.dirY * sin(-rotationSpeed);
        player.dirY = oldDirX * sin(-rotationSpeed) + player.dirY * cos(-rotationSpeed);

        oldPlaneX = player.planeX;
        player.planeX = player.planeX * cos(-rotationSpeed) - player.planeY * sin(-rotationSpeed);
        player.planeY = oldPlaneX * sin(-rotationSpeed) + player.planeY * cos(-rotationSpeed);
    }
    else if (keycode == KEY_RIGHT)
    {
        // Rotate to the right
        oldDirX = player.dirX;
        player.dirX = player.dirX * cos(rotationSpeed) - player.dirY * sin(rotationSpeed);
        player.dirY = oldDirX * sin(rotationSpeed) + player.dirY * cos(rotationSpeed);

        oldPlaneX = player.planeX;
        player.planeX = player.planeX * cos(rotationSpeed) - player.planeY * sin(rotationSpeed);
        player.planeY = oldPlaneX * sin(rotationSpeed) + player.planeY * cos(rotationSpeed);
    }
    return (0);
}

// Main function
int main()
{
    // Initialize MiniLibX and player position
    mlx = mlx_init();
    win = mlx_new_window(mlx, WIN_WIDTH, WIN_HEIGHT, "Raycasting Example");

    // Initialize player position and direction
    player.x = 3.5; // Valid starting position
    player.y = 3.5;
    player.dirX = -1;
    player.dirY = 0;
    player.planeX = 0;
    player.planeY = 0.66; // Camera plane for FOV

    // Set hooks
    mlx_hook(win, 2, 1L << 0, handle_key, NULL); // Key press event
    mlx_loop_hook(mlx, render_scene, NULL); // Rendering loop

    // Start MiniLibX event loop
    mlx_loop(mlx);
    return (0);
}
