#include "cub3d.h"
void cast_rays(void *mlx, void *win)
{
    for (int x = 0; x < 800; x++) // Loop through each column
    {
        // Calculate ray position and direction
        double cameraX = 2 * x / (double)800 - 1; // x-coordinate in camera space
        double rayDirX = player.dirX + player.planeX * cameraX;
        double rayDirY = player.dirY + player.planeY * cameraX;

        // Which box of the map we're in
        int mapX = (int)player.x;
        int mapY = (int)player.y;

        // Length of ray from one side to the next in x or y
        double sideDistX, sideDistY;

        // Length of ray from one x-side to next x-side or y-side
        double deltaDistX = fabs(1 / rayDirX);
        double deltaDistY = fabs(1 / rayDirY);

        double perpWallDist;

        // Direction to step in x or y (either +1 or -1)
        int stepX, stepY;

        int hit = 0; // Was a wall hit?
        int side; // Was it a NS or a EW wall?

        // Calculate step and initial sideDist
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
        while (hit == 0)
        {
            // Jump to next map square, either in x-direction, or in y-direction
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
            // Check if ray has hit a wall
            if (worldMap[mapY][mapX] > 0) hit = 1;
        }

        // Calculate distance to the point of impact
        if (side == 0) perpWallDist = (mapX - player.x + (1 - stepX) / 2) / rayDirX;
        else          perpWallDist = (mapY - player.y + (1 - stepY) / 2) / rayDirY;

        // Calculate height of line to draw on screen
        int lineHeight = (int)(600 / perpWallDist);

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + 600 / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + 600 / 2;
        if (drawEnd >= 600) drawEnd = 599;

        // Draw the vertical line (you can add textures/colors here)
        for (int y = drawStart; y < drawEnd; y++)
        {
            mlx_pixel_put(mlx, win, x, y, 0xFFFFFF); // White color
        }
    }
}
