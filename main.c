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

/* Defining our variables in this way allows for the w, h, scale, and name to be 
accessed from other points within our code base. Now we need to rewrite void 
game_init(void); and void game_quit(void); to get our screen to display and to c
lean up any memory left when we’re finished. */

void game_init(void);
void game_quit(void);

/* This provides us with a globally defined game object which will be used to 
house all persistent game information. Doing things this way will help cut down 
on function parameters, which can easily get out-of-hand if not kept in check. */

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
    game_init,
    game_quit
};

/* What we are doing here in the initialization step is printing the function 
name to the standard out stream, initializing the SDL library, and setting the 
game to a “running” state. During the SDL initialization the library will build 
and allocate all the objects and variables needed to make a game window, provide 
game audio, timers, events, joystick controls, etc. */

void game_init(void) {
    printf("game_init()\n");

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL error -> %s\n", SDL_GetError());
        exit(1);
    }

    unsigned int w = Game.screen.w;
    unsigned int h = Game.screen.h;
    const char *name = Game.screen.name;

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

/* Here we make the necessary library calls to get a window and renderer. The 
SDL_WINDOWPOS_CENTERED flag will center our window when it pops up for the player. 
It is possible to setup a fullscreen window however if you are using pixel art 
this isn’t recommended for reasons that will become obvious once we write code 
for setting up our sprites. To destroy our renderer and window we’ll code void 
game_quit(void); the following way. */

void game_quit(void) {
    SDL_DestroyRenderer(Game.screen.renderer);
    SDL_DestroyWindow(Game.screen.window);

    Game.screen.window = NULL;
    Game.screen.renderer = NULL;

    SDL_Quit();
}

/* At this point if you are encountering errors it is recommended to wrap the 
library function returns with if-statements to print any errors to stdout as we 
have done in the past. Now we need to modify our main game loop to provide 
continuous rendering as well as exiting functionality. It’s quite interesting to 
note that a window not polling for events (i.e. keypresses, mouse movements, etc.) will show as being non-responsive by the operating system. If on the other hand you are polling for events but aren’t listening for when the window is closed (i.e. red X at top right on windows) then the window will remain open and the only way to close it will require Ctrl+Alt+Delete. Let’s write a new game loop which polls for events and listens for when the window is closed. */

int main(int argc, char **argv) {
    Game.init();

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
        SDL_RenderPresent(Game.screen.renderer);
    }
    Game.quit;
    return (0);
}