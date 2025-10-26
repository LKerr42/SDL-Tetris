#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdlib.h>

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define TETROMINO_BLOCK_SIZE 20

int width = 640, height = 480;

// Blocks
int bWidthMin, bWidthMax, bHeightMin, bHeightMax; 

int randBlock = 0;
bool falling = true;

// Idea: each block in a Tetromino is a different "object" within this struct, 
// each with a different starting position and all with one colour.
// Then they could be placed in the array with one loop
typedef struct {
    int x;
    int y;
    int r;
    int g;
    int b;
    bool active;
} blockStruct;
blockStruct block;

typedef struct {
    bool v;
    int r;
    int g;
    int b;
} setBlocks;
setBlocks filledBlocks[22][12];

typedef struct {
    blockStruct blocks[4][4];
    int r;
    int g;
    int b;
    int x;
    int y;
} tetromino;
tetromino tetArray[7];

void setBlockColour(blockStruct *block, int R, int G, int B) {
    block -> r = R;
    block -> g = G;
    block -> b = B;
}

void setTetColour(tetromino *object, int R, int G, int B) {
    object -> r = R;
    object -> g = G;
    object -> b = B;
}

void setupTetrominos() {
    int shapes[7][4][4] = {
        //Long boy
        {{0, 0, 0, 0}, 
         {1, 1, 1, 1},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Left L
        {{0, 0, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 1, 1},
         {0, 0, 0, 0}},
        //Right L
        {{0, 0, 0, 0},
         {0, 0, 0, 1},
         {1, 1, 1, 1},
         {0, 0, 0, 0}},
        //Square
        {{0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        //Right squiggle
        {{0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //T boy
        {{0, 1, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Left squiggle
        {{1, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
    };
    setTetColour(&tetArray[0], 0, 255, 255); //Long boy
    setTetColour(&tetArray[1], 0, 0, 255);   //Left L
    setTetColour(&tetArray[2], 255, 160, 0); //Right L
    setTetColour(&tetArray[3], 255, 255, 0); //Square 
    setTetColour(&tetArray[4], 0, 255, 0);   //Right squiggle
    setTetColour(&tetArray[5], 150, 0, 255); //T boy
    setTetColour(&tetArray[6], 255, 0, 0);   //Right squiggle

    for (int count = 0; count < 7; count++) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                tetArray[count].blocks[y][x].x = x;
                tetArray[count].blocks[y][x].y = y;

                if (shapes[count][y][x] == 1) {
                    tetArray[count].blocks[y][x].active = true;
                    setBlockColour(&tetArray[count].blocks[y][x], tetArray[count].r, tetArray[count].g, tetArray[count].b);
                } else {
                    tetArray[count].blocks[y][x].active = false;
                }
            }
        }
        tetArray[count].x = 5;
        tetArray[count].y = 1;
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    bWidthMin = ((width / TETROMINO_BLOCK_SIZE) / 2) - 5;
    bWidthMax = ((width / TETROMINO_BLOCK_SIZE) / 2) + 5;

    bHeightMin = ((height / TETROMINO_BLOCK_SIZE) / 2) - 10;
    bHeightMax = ((height / TETROMINO_BLOCK_SIZE) / 2) + 10;

    setupTetrominos();

    setBlockColour(&block, 0, 0, 255);

    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 12; j++) {
            if (i == 0 || i == 21 || j == 0 || j == 11) {
                filledBlocks[i][j].v = true;
                filledBlocks[i][j].r = filledBlocks[i][j].g = filledBlocks[i][j].b = 80;
            }
            //printf("%d ", filledBlocks[i][j]);
        }
        //printf("\n");
    }

    SDL_SetAppMetadata("Play Tetis!", "0.1.3", "com.github.SDL-Tetris.LKerr42");

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
            if (filledBlocks[tetArray[randBlock].y][tetArray[randBlock].x-1].v == false && falling) {
                tetArray[randBlock].x = tetArray[randBlock].x - 1;
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (filledBlocks[tetArray[randBlock].y][tetArray[randBlock].x+1].v == false && falling) {
                tetArray[randBlock].x = tetArray[randBlock].x + 1;
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (filledBlocks[tetArray[randBlock].y+1][tetArray[randBlock].x].v == false && falling) {
                tetArray[randBlock].y = tetArray[randBlock].y + 1;
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
            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/*void randomiseBlockColour(blockStruct *block) {
    int rand = SDL_rand(7);
    switch(rand) {
        case 0: // long boy
            setBlockColour(block, 0, 255, 255);
            break;
        case 1: // left L boy
            setBlockColour(block, 0, 0, 255);
            break;
        case 2: // right L boy
            setBlockColour(block, 255, 160, 0);
            break;
        case 3: // square boy
            setBlockColour(block, 255, 255, 0);
            break;
        case 4: // right squiggly boy
            setBlockColour(block, 0, 255, 0);
            break;
        case 5: // T boy
            setBlockColour(block, 150, 0, 255);
            break;
        case 6: // left squiggly boy
            setBlockColour(block, 250, 0, 0);
            break;
    }
}*/

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_GetWindowSize(window, &width, &height);
    SDL_FRect rects[20];

    bWidthMin = ((width / TETROMINO_BLOCK_SIZE) / 2) - 5;
    bWidthMax = ((width / TETROMINO_BLOCK_SIZE) / 2) + 5;

    bHeightMin = ((height / TETROMINO_BLOCK_SIZE) / 2) - 10;
    bHeightMax = ((height / TETROMINO_BLOCK_SIZE) / 2) + 10;


    Uint64 now = SDL_GetTicks();
    static Uint64 lastFallTime = 0;

    if (now - lastFallTime >= 1000) {
        if (filledBlocks[tetArray[randBlock].y+1][tetArray[randBlock].x].v == true) {
            falling = false;
            printf("Stopped Falling: at %d above %d\n", tetArray[randBlock].y, tetArray[randBlock].y+1);
            //set block
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    if (tetArray[randBlock].blocks[k][l].active == true) {
                        filledBlocks[tetArray[randBlock].blocks[k][l].y][tetArray[randBlock].blocks[k][l].x].v = true; 
                        filledBlocks[tetArray[randBlock].blocks[k][l].y][tetArray[randBlock].blocks[k][l].x].r = tetArray[randBlock].blocks[k][l].r; 
                        filledBlocks[tetArray[randBlock].blocks[k][l].y][tetArray[randBlock].blocks[k][l].x].g = tetArray[randBlock].blocks[k][l].g;
                        filledBlocks[tetArray[randBlock].blocks[k][l].y][tetArray[randBlock].blocks[k][l].x].b = tetArray[randBlock].blocks[k][l].b;
                    }
                }
            }

            //reset block
            randBlock = SDL_rand(7);
            falling = true;
            tetArray[randBlock].x = 5;
            tetArray[randBlock].y = 1;
        } else {
            printf("Not Stopped Falling: at %d above %d\n", tetArray[randBlock].y, tetArray[randBlock].y+1);     
            tetArray[randBlock].y += 1;
            printf("Fell to %d at %llu\n", tetArray[randBlock].y, now);   
        }

        lastFallTime = now;
    }

    //Render
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
    SDL_RenderClear(renderer);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (tetArray[randBlock].blocks[y][x].active) {
                SDL_FRect Brect = {
                    (tetArray[randBlock].blocks[y][x].x + bWidthMin + tetArray[randBlock].x) * TETROMINO_BLOCK_SIZE,
                    (tetArray[randBlock].blocks[y][x].y + bHeightMin + tetArray[randBlock].y) * TETROMINO_BLOCK_SIZE,
                    TETROMINO_BLOCK_SIZE,
                    TETROMINO_BLOCK_SIZE
                };
                SDL_SetRenderDrawColor(renderer, tetArray[randBlock].r, tetArray[randBlock].g, tetArray[randBlock].b, SDL_ALPHA_OPAQUE); 
                SDL_RenderFillRect(renderer, &Brect);
            }
        }
    }

    /*SDL_SetRenderDrawColor(renderer, block.r, block.g, block.b, SDL_ALPHA_OPAQUE);  // full alpha
    rects[0].x = (block.x + bWidthMin) * TETROMINO_BLOCK_SIZE;
    rects[0].y = (block.y + bHeightMin) * TETROMINO_BLOCK_SIZE;
    rects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;*/

    SDL_RenderFillRect(renderer, &rects[0]); // render rectangle

    //int c = 0;
    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 12; j++) {
            if (filledBlocks[i][j].v == true) {
                rects[0].x = (j + bWidthMin) * TETROMINO_BLOCK_SIZE;
                rects[0].y = (i + bHeightMin) * TETROMINO_BLOCK_SIZE;
                rects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;
                SDL_SetRenderDrawColor(renderer, filledBlocks[i][j].r, filledBlocks[i][j].g, filledBlocks[i][j].b, SDL_ALPHA_OPAQUE);
                SDL_RenderFillRect(renderer, &rects[0]);
                //c++;
            }
        }
    }

    /*setRects[0].x = (bWidthMin-1) * TETROMINO_BLOCK_SIZE;
    setRects[0].y = (bHeightMin-1) * TETROMINO_BLOCK_SIZE;
    setRects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;*/

    //SDL_SetRenderDrawColor(renderer, 80, 80, 80, SDL_ALPHA_OPAQUE);  // grey, full alpha
    //SDL_RenderFillRects(renderer, setRects, SDL_arraysize(setRects));

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

