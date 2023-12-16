#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#include "sdl.h"
#include "dynamic_array.h"

#define MOLE_LIFETIME 4000
#define SPAWN_INTERVAL 500

typedef struct
{
    SDL_Texture *tex;
    SDL_Rect rect;
    Uint32 spawn_time;
} Mole;

int main()
{

    // Inicializace SDL a vytvoření okna
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Whack-a-Mole", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Inicializace TTF
    TTF_Init();

    // Načtení obrázků
    SDL_Texture *emoji_tex = IMG_LoadTexture(renderer, "emojipedia.jpg");
    TTF_Font *font = TTF_OpenFont("Arial.ttf", 24);

    // Získání velikosti emoji encyklopedie
    int ew, eh;
    SDL_QueryTexture(emoji_tex, NULL, NULL, &ew, &eh);

    // Inicializace proměnných
    int spawn_interval = SPAWN_INTERVAL;
    int score = 0;

    // Pole pro uložení krtků
    dynarray moles;
    dynarray_init(&moles, 10);

    SDL_Event event;

    while (1)
    {

        // Obsluha událostí
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                goto cleanup;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                SDL_Point click = {event.button.x, event.button.y};
                // Zjištění zásahu krtka
                for (int i = 0; i < moles.size; i++)
                {
                    Mole *m = moles.items[i];
                    if (SDL_PointInRect(&click, &m->rect))
                    {
                        // Odebrání krtka při zásahu
                        dynarray_remove(&moles, moles.items[i]);
                        free(m);
                        // Zvýšení skóre
                        score++;
                        // Zrychlení hry
                        spawn_interval -= 10;
                    }
                }
            }
        }

        // Vymazání obrazovky
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Spawnování nových krtků
        Uint32 now = SDL_GetTicks();
        if (now - spawn_time >= spawn_interval)
        {
            // Vytvoření nového krtka
            Mole *m = malloc(sizeof(Mole));
            m->spawn_time = now;
            m->rect.w = 50 + rand() % 100;
            m->rect.h = 50 + rand() % 100;
            m->rect.x = rand() % (800 - m->rect.w);
            m->rect.y = rand() % (600 - m->rect.h);
            int index = (int)(rand() % ((int)(ew / 64) * (int)(eh / 64)));
            m->tex = emoji_tex;
            // Přidání krtka do pole
            dynarray_push(&moles, m);
        }

        // Vykreslení krtků
        for (int i = 0; i < moles.size; i++)
        {
            Mole *m = moles.items[i];

            // Výpočet pozice v textuře
            int ex = (index % 18) * 64;
            int ey = (index / 18) * 64;
            SDL_Rect src_rect = {ex, ey, 64, 64};

            // Vykreslení krtka
            SDL_RenderCopyEx(renderer, m->tex, &src_rect, &m->rect, 0, NULL, SDL_FLIP_NONE);

            // Vykreslení odpočtu života
            int lifetime = MOLE_LIFETIME - (now - m->spawn_time);
            char text[32];
            snprintf(text, 32, "%d", lifetime / 1000);
            SDL_Color color = {255, 255, 0};
            if (lifetime < 1000)
            {
                color.r = 255 * lifetime / 1000;
                color.g = 255 - 255 * lifetime / 1000;
            }
            sdl_draw_text(renderer, font, color,
                          (SDL_Rect){m->rect.x, m->rect.y - 20, 0, 0}, text);
        }

        // Vykreslení skóre
        char score_text[32];
        snprintf(score_text, 32, "Skóre: %d", score);
        sdl_draw_text(renderer, font, (SDL_Color){0, 0, 0}, (SDL_Rect){10, 10, 0, 0}, score_text);

        // Swap buffers
        SDL_RenderPresent(renderer);
    }
cleanup:

    // Úklid paměti
    dynarray_free(&moles);
    TTF_CloseFont(font);
    SDL_DestroyTexture(emoji_tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}
