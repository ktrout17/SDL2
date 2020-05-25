#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

/* So now we have access to Game.screen who’s properties are w the width of the 
window, h the height of the window, name the string which will appear and the 
top of our window, window a pointer to our SDL window, and renderer a pointer to 
our SDL renderer. The width, height, scale, and name are specified as follows in 
the game header file */

#define SCREEN_w 640
#define SCREEN_H 480
#define SCREEN_SCALE 1
#define SCREEN_NAME "Prototype"

void game_init(void);
void game_quit(void);

static struct {
    // define "attributes"
    SDL_bool running;
    
    struct {
        unsigned int w;
        unsigned int h;
        const char *name;
        SDL_Window *window;
        SDL_Renderer *renderer;
    } screen;

    struct {
        unsigned int n;
        SDL_Surface **spritesheet;
    } gfx;

    /// define methods
    void (*init)(void);
    void (*quit)(void);
} Game = {
    SDL_FALSE,
    {
        SCREEN_SCALE*SCREEN_w,
        SCREEN_SCALE*SCREEN_H,
        SCREEN_NAME,
        NULL,
        NULL
    },
    { 0, NULL },
    game_init,
    game_quit
};

/* This takes our original Game “soliton” and adds a “gfx” module. The graphics 
module contains unsigned int n;  which is the number of 8×8 sprites found within 
our .bmp spritesheet, and SDL_Surface** spritesheet; which is an array of SDL 
surfaces. Each 8×8 sprite will receive its own SDL surface. Later we will arrange 
these surfaces into textures to be drawn with out renderer. */

/* Next we will read in “spritesheet.bmp”, set the color key to 0xFF00FF and 
populate the spritesheet array with surfaces. This code should be placed within 
the void game_init(void); function. */

void game_init(void) {
    printf("game_init()\n");

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL error -> %s\n", SDL_GetError());
        exit(1);
    }

    unsigned int w = Game.screen.w;
    unsigned int h = Game.screen.h;
    const char *name = Game.screen.name;

    SDL_Surface *surface = SDL_LoadBMP("spritesheet.bmp");
    int n = ((surface->w/8)*(surface->h/8) + 1);

    Game.gfx.n = n;
    Game.gfx.spritesheet = (SDL_Surface **)malloc(sizeof(SDL_Surface *) *n);

    int i, x, y;
    SDL_Rect rect = { 0, 0, 8, 8 };
    for (i = 0; i < n; i++) {
        Game.gfx.spritesheet[i] = SDL_CreateRGBSurface(0, 8, 8, 24, 0x00, 0x00, 0x00, 0x00);
        SDL_SetColorKey(Game.gfx.spritesheet[i], 1, 0xFF00FF);
        SDL_FillRect(Game.gfx.spritesheet[i], 0, 0xFF00FF);
        if (i != 0) {
            x = (i - 1) % (surface->w/8);
            y = (i - x) / (surface->w/8);
            rect.x = x * 8;
            rect.y = y * 8;
            SDL_BlitSurface(surface, &rect, Game.gfx.spritesheet[i], NULL);
        }
    }
    SDL_FreeSurface(surface);

/* This will give us an array of sprite surfaces with the first sprite 
(spritesheet[0] ) being a completely transparent surface. This is done on purpose 
so that the surface array an easily be integrated with the Tiled map editor, which 
we will discuss in future tutorials. */

    Game.screen.window = SDL_CreateWindow(
        name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        w, h, 0
    );
    Game.screen.renderer = SDL_CreateRenderer(
        Game.screen.window, -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
    );
    Game.running = SDL_TRUE;
}

void game_quit(void) {
    int i;
    for(i=0; i<Game.gfx.n; i++)
        SDL_FreeSurface(Game.gfx.spritesheet[i]);
    free(Game.gfx.spritesheet);
    Game.gfx.spritesheet = NULL;
 
    SDL_DestroyRenderer(Game.screen.renderer);
    Game.screen.renderer = NULL;
 
    SDL_DestroyWindow(Game.screen.window);
    Game.screen.window = NULL;
 
    SDL_Quit();
}

/* Next we will move to our int main(int argc, char* argv[]); function where 
we will create textures for the sprites we wish to render. Ideally, the textures 
should be allocated for each room/level during the loading screen. This works 
well for a 2D game but isn’t required as most computers can handle the dynamic 
allocation without much problem. After modifying the code we are left with the 
following. If you would like to use the same spritesheet as I’m using here you 
can get it from here. */

int main(int argc, char **argv) {
    Game.init();

    int x = Game.screen.w/2-8;
    int y = Game.screen.h/2-8;

    SDL_Rect rect = { 0, 0, 8*2, 8*2 };
    SDL_Texture *texture1 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.gfx.spritesheet[17]);
    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.gfx.spritesheet[18]);
    SDL_Texture *texture3 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.gfx.spritesheet[29]);
    SDL_Texture *texture4 = SDL_CreateTextureFromSurface(Game.screen.renderer, Game.gfx.spritesheet[30]);

    SDL_Event event;
    while (Game.running) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT: {
                    Game.running = SDL_FALSE;
                } break;
            }
        }
        SDL_RenderClear(Game.screen.renderer);
        rect.x = 0+x, rect.y = 0+y;
        SDL_RenderCopy(Game.screen.renderer, texture1, NULL, &rect);
 
        rect.x = 8*2+x, rect.y = 0+y;
        SDL_RenderCopy(Game.screen.renderer, texture2, NULL, &rect);
 
        rect.x = 0+x, rect.y = 8*2+y;
        SDL_RenderCopy(Game.screen.renderer, texture3, NULL, &rect);
 
        rect.x = 8*2+x, rect.y = 8*2+y;
        SDL_RenderCopy(Game.screen.renderer, texture4, NULL, &rect);
 
        SDL_RenderPresent(Game.screen.renderer);
    }
 
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyTexture(texture4);
 
    Game.quit();
 
    return 0;
}

/* This will draw a chest in the middle of the screen. Here we are creating a texture 
for each 8×8 sprite. This is highly inefficient, what we will learn to do later is 
create a surface with all the 8×8 sprites and then create a texture from this bigger 
surface. This will help a lot but for now we are fine to do this. The last step here 
is to clean up the sprite array. */