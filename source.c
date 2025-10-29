#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#define TETROMINO_BLOCK_SIZE 20

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

int width = 700, height = 550;

// Blocks
int bWidthMin, bWidthMax, bHeightMin, bHeightMax; 
int randBlock = 0, rotationI = 0;
bool falling = true, winning = true;

TTF_Font* globalFont;

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

void rotateTetrominoCCW(tetromino *t);
void rotateTetrominoCW(tetromino *t);

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
        {{1, 0, 0, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Right L
        {{0, 0, 1, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0},
         {0, 0, 0, 0}},
        //Square
        {{1, 1, 0, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0},
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
        tetArray[count].x = 4;
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

    SDL_SetAppMetadata("Play Tetis!", "0.3.0", "com.github.SDL-Tetris.LKerr42");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize ttf: %s", SDL_GetError());
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

    globalFont = TTF_OpenFont("assets\\VT323-Regular.ttf", 24);
    if (globalFont == NULL) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

bool canMove(tetromino *t, setBlocks filledBlocks[22][12], int dx, int dy) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            blockStruct *b = &t->blocks[row][col];
            if (b->active) {
                int newX = t->x + col + dx;
                int newY = t->y + row + dy;

                // Check walls (assuming 0..11 width and 0..21 height)
                if (newX < 0 || newX >= 11 || newY >= 21) {
                    return false;
                }

                // Check collisions with placed blocks
                if (filledBlocks[newY][newX].v == true) {
                    return false;
                }
            }
        }
    }
    return true;
}

void rotateTetrominoCW(tetromino *t) {
    int N, Y, X, rx, ry;
    blockStruct temp[4][4];
    if (randBlock == 0) {
        N = 4;
    } else if (randBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

    //printf("Colour: %d, %d, %d\n", t->r, t->g, t->b);

    for (Y = 0; Y < 4; Y++) { //FIX: the active, x and y variables have to be updated aswell
        for (X = 0; X < 4; X++) {
            temp[Y][X].active = false;

            temp[Y][X].x = X;
            temp[Y][X].y = Y;

            temp[Y][X].r = t->r;
            temp[Y][X].g = t->g;
            temp[Y][X].b = t->b;
        }
    }

    for (Y = 0; Y < N; Y++) { 
        for (X = 0; X < N; X++) {
            blockStruct src = t->blocks[Y][X];

            rx = Y;
            ry = 1 - (X - (N - 2));

            temp[ry][rx] = src;

            temp[ry][rx].x = rx;
            temp[ry][rx].y = ry;

            temp[ry][rx].r = t->r;
            temp[ry][rx].g = t->g;
            temp[ry][rx].b = t->b;
        }
    }

    for (Y = 0; Y < 4; Y++) {
        for (X = 0; X < 4; X++) {
            t->blocks[Y][X] = temp[Y][X];
        }
    }

    if (!canMove(t, filledBlocks, 0, 0)) {
        if (canMove(t, filledBlocks, 1, 0)) {
            t->x += 1;
        } else if (canMove(t, filledBlocks, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCCW(t);
        }
    }
}

void rotateTetrominoCCW(tetromino *t) {
    int N, Y, X;
    blockStruct temp[4][4];
    if (randBlock == 0) {
        N = 4;
    } else if (randBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

    //printf("Colour: %d, %d, %d\n", t->r, t->g, t->b);

    for (Y = 0; Y < 4; Y++) { //FIX: the active, x and y variables have to be updated aswell
        for (X = 0; X < 4; X++) {
            temp[Y][X].active = false;

            temp[Y][X].x = X;
            temp[Y][X].y = Y;

            temp[Y][X].r = t->r;
            temp[Y][X].g = t->g;
            temp[Y][X].b = t->b;
        }
    }

    for (Y = 0; Y < N; Y++) { 
        for (X = 0; X < N; X++) {
            blockStruct src = t->blocks[Y][X];

            int rx = 1 - (Y - (N - 2));
            int ry = X;

            temp[ry][rx] = src;

            temp[ry][rx].x = rx;
            temp[ry][rx].y = ry;

            temp[ry][rx].r = t->r;
            temp[ry][rx].g = t->g;
            temp[ry][rx].b = t->b;
        }
    }

    for (Y = 0; Y < 4; Y++) {
        for (X = 0; X < 4; X++) {
            t->blocks[Y][X] = temp[Y][X];
        }
    }


    if (!canMove(t, filledBlocks, 0, 0)) {
        if (canMove(t, filledBlocks, 1, 0)) {
            t->x += 1;
        } else if (canMove(t, filledBlocks, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCW(t);
        }
    }
}

void handleKeyboardInput(SDL_Scancode event) {
    int indx, furth;
    switch (event) {
        case SDL_SCANCODE_LEFT: {
            if (canMove(&tetArray[randBlock], filledBlocks, -1, 0)) {
                tetArray[randBlock].x -= 1;
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (canMove(&tetArray[randBlock], filledBlocks, 1, 0)) {
                tetArray[randBlock].x += 1;
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (canMove(&tetArray[randBlock], filledBlocks, 0, 1)) {
                tetArray[randBlock].y += 1;
            } else {
                // The block can’t move down anymore → it has landed
                falling = false;
                // Copy its active cells into filledBlocks
            }
            break;
        }
        case SDL_SCANCODE_D: {
            printf("Rotating CW\n");
            rotateTetrominoCW(&tetArray[randBlock]);
            break;
        }
        case SDL_SCANCODE_A: {
            printf("Rotating CCW\n");
            rotateTetrominoCCW(&tetArray[randBlock]);
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
            bWidthMin = ((width / TETROMINO_BLOCK_SIZE) / 2) - 5;
            bWidthMax = ((width / TETROMINO_BLOCK_SIZE) / 2) + 5;

            bHeightMin = ((height / TETROMINO_BLOCK_SIZE) / 2) - 10;
            bHeightMax = ((height / TETROMINO_BLOCK_SIZE) / 2) + 10;
            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void moveBoardDown(int remove) {
    for (int i = remove; i >= 2; i--) {
        for (int j = 1; j <= 10; j++) {
            filledBlocks[i][j] = filledBlocks[i-1][j];
        }
    }
}

void displayText(char str[], int x, int y, TTF_Font* font, int r, int g, int b) {
    // Render text to a surface
    SDL_Color textColor = {r, g, b, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, str, strlen(str), textColor);
    if (!textSurface) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    // Convert surface to texture
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface); // free surface memory

    if (!textTexture) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    // Get text dimensions
    float textW, textH;
    SDL_GetTextureSize(textTexture, &textW, &textH);

    SDL_FRect textRect = {x, y, textW, textH};
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

bool toStop = false;
int posX = 0, posY = 0;
/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_FRect rects[20];

    Uint64 now = SDL_GetTicks();
    static Uint64 lastFallTime = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    displayText("Tetris Time!", 0, 0, globalFont, 255, 255, 255);

    if (winning) {
        if (now - lastFallTime >= 1000) {
            for (int a = 0; a < 4; a++) {
                for (int b = 0; b < 4; b++) {
                    posX = tetArray[randBlock].x+tetArray[randBlock].blocks[a][b].x;
                    posY = tetArray[randBlock].y+tetArray[randBlock].blocks[a][b].y;
                    if (filledBlocks[posY+1][posX].v == true && tetArray[randBlock].blocks[a][b].active == true) {
                        toStop = true;
                        break;
                    }
                }
                if (toStop == true) break;
            }
            if (toStop == true) {
                toStop = false;
                falling = false;
                printf("Stopped Falling: at %d above %d\n", tetArray[randBlock].y, tetArray[randBlock].y+1);
                //set block
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        if (tetArray[randBlock].blocks[k][l].active == true) {
                            posX = tetArray[randBlock].x+tetArray[randBlock].blocks[k][l].x;
                            posY = tetArray[randBlock].y+tetArray[randBlock].blocks[k][l].y;

                            filledBlocks[posY][posX].v = true; 
                            filledBlocks[posY][posX].r = tetArray[randBlock].blocks[k][l].r; 
                            filledBlocks[posY][posX].g = tetArray[randBlock].blocks[k][l].g;
                            filledBlocks[posY][posX].b = tetArray[randBlock].blocks[k][l].b;
                        }
                    }
                }

                //reset block
                randBlock = SDL_rand(7);
                falling = true;
                tetArray[randBlock].x = 4;
                tetArray[randBlock].y = 1;

                if (filledBlocks[1][5].v == true) {
                    winning = false;
                }
            } else {
                printf("Not Stopped Falling: at %d above %d\n", tetArray[randBlock].y, tetArray[randBlock].y+1);
                if (toStop == false) {
                    tetArray[randBlock].y += 1;
                }  
                printf("Fell to %d at %llu\n", tetArray[randBlock].y, now);   
            }

            lastFallTime = now;
        }

        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (tetArray[randBlock].blocks[y][x].active) {
                    posX = tetArray[randBlock].x+tetArray[randBlock].blocks[y][x].x;
                    posY = tetArray[randBlock].y+tetArray[randBlock].blocks[y][x].y;
                    SDL_FRect Brect = {
                        (posX + bWidthMin) * TETROMINO_BLOCK_SIZE,
                        (posY + bHeightMin) * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    SDL_SetRenderDrawColor(renderer, tetArray[randBlock].r, tetArray[randBlock].g, tetArray[randBlock].b, SDL_ALPHA_OPAQUE); 
                    SDL_RenderFillRect(renderer, &Brect);
                }
            }
        }

        for (int i = 0; i < 22; i++) {
            for (int j = 0; j < 12; j++) {
                if (filledBlocks[i][j].v == true) {
                    rects[0].x = (j + bWidthMin) * TETROMINO_BLOCK_SIZE;
                    rects[0].y = (i + bHeightMin) * TETROMINO_BLOCK_SIZE;
                    rects[0].w = rects[0].h = TETROMINO_BLOCK_SIZE;
                    SDL_SetRenderDrawColor(renderer, filledBlocks[i][j].r, filledBlocks[i][j].g, filledBlocks[i][j].b, SDL_ALPHA_OPAQUE);
                    SDL_RenderFillRect(renderer, &rects[0]);
                }
            }
        }

        //check if row filled
        int countBlocks = 0;
        for (int l = 1; l <= 20; l++) {
            countBlocks = 0;
            for (int k = 1; k <= 10 ; k++) {
                if (filledBlocks[l][k].v == true) {
                    countBlocks++;
                }
            }
            if (countBlocks == 10) {
                moveBoardDown(l);
            }
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
        SDL_RenderClear(renderer);
        bool displayed = false;
        displayText("You Lost!", (width/2)-10, 10, globalFont, 255, 255, 255);
    }

    SDL_RenderPresent(renderer); // render everything

    return SDL_APP_CONTINUE;  // carry on with the program!
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
    TTF_CloseFont(globalFont);
    TTF_Quit();
}

