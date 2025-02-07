#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RAY 0xffd43b
#define COLOR_RAY_BLUR 0xbd6800
#define RAYS_NUMBER 1000

#define M_PI (3.14159265358979323846)


typedef struct {
    double x;
    double y;
    double r; //radius
} Circle;

typedef struct {
    double x_start, y_start;
    double a; // angle
} Ray;

void FillCircle(SDL_Surface* surface, Circle circle, Uint32 color)
{
    double radius_sq = pow(circle.r, 2);
    for (double i = circle.x - circle.r; i <= circle.x + circle.r; i++)
    {
        for (double j = circle.y - circle.r; j <= circle.y + circle.r; j++)
        {
            double distance_sq = pow(i - circle.x, 2) + pow(j - circle.y, 2);
            if (distance_sq < radius_sq)
            {
                SDL_Rect pixel = {i, j, 1, 1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

void generateRays(Circle circle, Ray rays[RAYS_NUMBER])
{
    for (int i = 0; i < RAYS_NUMBER; i++)
    {
        double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
        Ray ray = {circle.x, circle.y, angle};
        rays[i] = ray;
        // printf("angle = %f\n", angle);
    }
    
}

void FillRays(SDL_Surface* surface, Ray rays[RAYS_NUMBER], Uint32 color, Circle object)
{   
    double radius_sq = pow(object.r, 2);
    for (int i = 0; i < RAYS_NUMBER; i++)
    {
        Ray ray = rays[i];

        int end_of_screen = 0;
        int object_hit = 0;

        double step = 1;

        double x_draw = ray.x_start;
        double y_draw = ray.y_start;

        while (!end_of_screen && !object_hit)
        {
            x_draw += step * cos(ray.a);
            y_draw += step * sin(ray.a);

            SDL_Rect pixel = {x_draw, y_draw, 1, 1};
            SDL_FillRect(surface, &pixel, color);

            if(x_draw < 0 || x_draw > WIDTH)
                end_of_screen = 1;
            if(y_draw < 0 || y_draw > HEIGHT)
                end_of_screen = 1;


            // Checking collision between ray and object
            double distance_sq = pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);
            if (distance_sq < radius_sq)
            {
                break;
            }
        }
    }
}

void bounceCircle()
{

}
int main()
{
    printf("raytracing...\b");

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect erase_rect = {0, 0, WIDTH, HEIGHT};
    
    Circle light = {200, 200, 40};
    Ray rays[RAYS_NUMBER];
    generateRays(light, rays);
    
    Circle shadow = {550, 300, 140};
    double obstacle_speed_y = 4;

    int running = 1;
    SDL_Event event; 
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }

            if (event.type == SDL_MOUSEMOTION && event.motion.state != 0)
            {
                light.x = event.motion.x;
                light.y = event.motion.y; 
                generateRays(light, rays);
            }
        }

        SDL_FillRect(surface, &erase_rect, COLOR_BLACK);
        
        FillRays(surface, rays, COLOR_RAY, shadow);
        FillCircle(surface, light, COLOR_WHITE); 
        

        FillCircle(surface, shadow, COLOR_WHITE);
        shadow.y += obstacle_speed_y;
        if (shadow.y - shadow.r < 0)
            obstacle_speed_y = -obstacle_speed_y;
        if (shadow.y + shadow.r > HEIGHT)
            obstacle_speed_y = -obstacle_speed_y;

        
        SDL_UpdateWindowSurface(window);
        SDL_Delay(7);
    }
    return 0;
}
