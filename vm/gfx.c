#include <vm.h>

#include <SDL2/SDL.h>

SDL_Texture *fb = NULL;
SDL_Renderer *ren = NULL;
SDL_Window *win = NULL;
uint32_t *pixels = NULL;

// Initialize
void gfx_init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
    win = SDL_CreateWindow("Virtual Machine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 100, SDL_WINDOW_OPENGL);
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    fb = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 160, 100);

    pixels = malloc(160 * 100 * sizeof(uint32_t));
}

// Update
void gfx_update(uint8_t *mem)
{
    for (unsigned int i = 0; i < 160 * 100; i++)
    {
        pixels[i] = 0xffffffff;
    }

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if ((e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
        {
            printf("Exit\n");
            exit(-1);
            gfx_fini();
        }
    }

    SDL_UpdateTexture(fb, NULL, pixels, 160 * sizeof(uint32_t));

    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, fb, NULL, NULL);
    SDL_RenderPresent(ren);
}

// Finalize
void gfx_fini()
{
    SDL_DestroyWindow(win);
    SDL_Quit();
}
