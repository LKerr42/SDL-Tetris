#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define TETROMINO_BLOCK_SIZE 20

int width = 640, height = 480;
int gWidthMin, gWidthMax, gHeightMin, gHeightMax; 

typedef struct {
    int x;
    int y;
} blockPos;
blockPos gridPos;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    gWidthMin = ((width / TETROMINO_BLOCK_SIZE) / 2) - 5;
    gWidthMax = ((width / TETROMINO_BLOCK_SIZE) / 2) + 5;

    gHeightMin = ((height / TETROMINO_BLOCK_SIZE) / 2) - 10;
    gHeightMax = ((height / TETROMINO_BLOCK_SIZE) / 2) + 10;

    gridPos.x = (width / TETROMINO_BLOCK_SIZE) / 2;
    gridPos.y = gHeightMin;

    SDL_SetAppMetadata("Play Tetis!", "0.1.2", "com.github.SDL-Tetris.LKerr42");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    bool wind = SDL_CreateWindowAndRenderer(
        "Play Tetris!", 
        width, height, 
        SDL_WINDOW_RESIZABLE,
        &window, &renderer
    );

    if (!wind) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void handleKeyboardInput(SDL_Scancode event) {
    switch (event) {
        case SDL_SCANCODE_LEFT: {
            if (gridPos.x - 1 >= gWidthMin) {
                gridPos.x = gridPos.x - 1;
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (gridPos.x + 1 <= gWidthMax) {
                gridPos.x = gridPos.x + 1;
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (gridPos.y + 1 <= gHeightMax) {
                gridPos.y = gridPos.y + 1;
            } 
            break;
        }
    }
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            handleKeyboardInput(event->key.scancode);
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            SDL_GetWindowSize(window, &width, &height);
            gridPos.x = (width / TETROMINO_BLOCK_SIZE) / 2;
            gridPos.y = (height / TETROMINO_BLOCK_SIZE) / 2;
            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_GetWindowSize(window, &width, &height);
    SDL_FRect rects[300];

    gWidthMin = ((width / TETROMINO_BLOCK_SIZE) / 2) - 5;
    gWidthMax = ((width / TETROMINO_BLOCK_SIZE) / 2) + 5;

    gHeightMin = ((height / TETROMINO_BLOCK_SIZE) / 2) - 10;
    gHeightMax = ((height / TETROMINO_BLOCK_SIZE) / 2) + 10;
    const Uint64 rn = SDL_GetTicks();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  // blue, full alpha
    rects[0].x = gridPos.x * TETROMINO_BLOCK_SIZE;
    rects[0].y = gridPos.y * TETROMINO_BLOCK_SIZE;
    rects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;

    SDL_RenderFillRect(renderer, &rects[0]); // render rectangle

    int rectCount = 0;
    for (int i = gHeightMin-1; i <= gHeightMax+1; i++) {
        for (int j = gWidthMin-1; j <= gWidthMax+1; j++) {
            if (i == gHeightMin-1 || i == gHeightMax+1 || j == gWidthMin-1 || j == gWidthMax+1) {
                rects[rectCount].x = j * TETROMINO_BLOCK_SIZE;
                rects[rectCount].y = i * TETROMINO_BLOCK_SIZE;
                rects[rectCount].w = rects[rectCount].h = TETROMINO_BLOCK_SIZE;
                rectCount++;
            }
        }
    }

    /*rects[0].x = (gWidthMin-1) * TETROMINO_BLOCK_SIZE;
    rects[0].y = (gHeightMin-1) * TETROMINO_BLOCK_SIZE;
    rects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;*/

    SDL_SetRenderDrawColor(renderer, 80, 80, 80, SDL_ALPHA_OPAQUE);  // white, full alpha
    SDL_RenderFillRects(renderer, rects, SDL_arraysize(rects));

    SDL_RenderPresent(renderer); // render everything

    /*
    SDL_FRect rects[16];
    const Uint64 now = SDL_GetTicks();
    int i;

    //we'll have the rectangles grow and shrink over a few seconds.
    const float direction = ((now % 2000) >= 1000) ? 1.0f : -1.0f;
    const float scale = ((float) (((int) (now % 1000)) - 500) / 500.0f) * direction;

    //as you can see from this, rendering draws over whatever was drawn before it.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  //black, full alpha
    SDL_RenderClear(renderer);  //start with a blank canvas.

    // Rectangles are comprised of set of X and Y coordinates, plus width and
    // height. (0, 0) is the top left of the window, and larger numbers go
    // down and to the right. This isn't how geometry works, but this is
    // pretty standard in 2D graphics.

    // Let's draw a single rectangle (square, really).
    rects[0].x = rects[0].y = 100;
    rects[0].w = rects[0].h = 100 + (100 * scale);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);  // red, full alpha
    SDL_RenderRect(renderer, &rects[0]);

    / Now let's draw several rectangles with one function call.
    for (i = 0; i < 3; i++) {
        const float size = (i+1) * 50.0f;
        rects[i].w = rects[i].h = size + (size * scale);
        rects[i].x = (WINDOW_WIDTH - rects[i].w) / 2;  // center it.
        rects[i].y = (WINDOW_HEIGHT - rects[i].h) / 2;  // center it.
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);  // green, full alpha 
    SDL_RenderRects(renderer, rects, 3);  // draw three rectangles at once

    // those were rectangle _outlines_, really. You can also draw _filled_ rectangles! 
    rects[0].x = 400;
    rects[0].y = 50;
    rects[0].w = 100 + (100 * scale);
    rects[0].h = 50 + (50 * scale);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);  // blue, full alpha
    SDL_RenderFillRect(renderer, &rects[0]);

    // ...and also fill a bunch of rectangles at once...
    for (i = 0; i < SDL_arraysize(rects); i++) {
        const float w = (float) (WINDOW_WIDTH / SDL_arraysize(rects));
        const float h = i * 8.0f;
        rects[i].x = i * w;
        rects[i].y = WINDOW_HEIGHT - h;
        rects[i].w = w;
        rects[i].h = h;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  // white, full alpha
    SDL_RenderFillRects(renderer, rects, SDL_arraysize(rects));

    SDL_RenderPresent(renderer);  // put it all on the screen! */

    return SDL_APP_CONTINUE;  // carry on with the program!
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
}

