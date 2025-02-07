#include <stdio.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600

#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000

#define GRAVITY 0.2
#define BOUNCE 0.85
#define FRICTION 0.95 // 0.05

typedef struct circle {
    double x, y;
    double vx, vy;
    double r;
} Circle;

void FillCircle(SDL_Surface* surface, Circle circle, Uint32 color)
{
    double radius_sq = pow(circle.r, 2);
    for(double i = circle.x - circle.r; i < circle.x + circle.r; i++)
    {
        for(double j = circle.y - circle.r; j < circle.y + circle.r; j++)
        {
            double distance_sq = pow(i - circle.x, 2) + pow(j - circle.y, 2);
            if(distance_sq < radius_sq)
            {
                SDL_Rect pixel = {i, j, 1, 1};
                SDL_FillRect(surface, &pixel, COLOR_WHITE);
            }
        }
    }
}

void updateBall(Circle* ball)
{
    ball->x += ball->vx;
    ball->y += ball->vy;
    ball->vy += GRAVITY; 

    // Left Border
    if(ball->x - ball->r < 0)
    {
        ball->x = 0 + ball->r;
        ball->vx = -ball->vx * BOUNCE;
    }

    //Right border
    if(ball->x + ball->r > WIDTH)
    {
        ball->x = WIDTH - ball->r;
        ball->vx = -ball->vx * BOUNCE;
    }

    //Top Border
    if(ball->y - ball->r < 0)
    {
        ball->y = 0 + ball->r;
        ball->vy = -ball->vy * BOUNCE;
    }

    //Bottom border
    if(ball->y + ball->r > HEIGHT)
    {
        ball->y = HEIGHT - ball->r;
        ball->vy = -ball->vy * BOUNCE;
        ball->vx *= FRICTION;
    }

    printf("x: %f, vx: %f\n", ball->x, ball->vx);
    printf("y: %f, vy: %f\n", ball->y, ball->vy);
}
int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("bouncing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    SDL_Rect clear = {0, 0, WIDTH, HEIGHT};

    Circle ball = {450, 300, 100, 100, 50};
    FillCircle(surface, ball, COLOR_WHITE);
    
    int running = 1;
    SDL_Event event;
    while(running)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        // Clearing the screen
        SDL_FillRect(surface, &clear, COLOR_BLACK);

        updateBall(&ball);
        FillCircle(surface, ball, COLOR_WHITE);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(7);
    }
}