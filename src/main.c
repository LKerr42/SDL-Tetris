#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include "include/app.h"
#include "include/audio.h"
#include "include/text.h"
#include "include/keyboard.h"
#include "include/tetromino.h"

//file specific globals
bool firstRun = true;
char scoreString[7] = "0000000", *finalScore = NULL;
int score = 0, currentMove = 0, currentColour = 1;
int halfTitleWidth, halfTitleHeight, posX = 0, posY = 0;
float keyW, keyH;

char fileNames[9][16] = {
    "clear",
    "holyMoly",
    "land",
    "moveLeft",
    "moveRight",
    "open",
    "spinCCW",
    "spinCW",
    "switch"
};

appContext app;

typedef struct {
    bool v;
    int r;
    int g;
    int b;
} setBlocks;
setBlocks filledBlocks[22][12];

tetromino wireframeTet;

SDL_Texture *boardTexture;
SDL_Texture *nextTexture;
SDL_Surface *icon;

SDL_Texture *keyboard;
SDL_Surface *keyboardSurface;
SDL_FRect keyRect;

SDL_FRect keyboardTextRect;
SDL_FRect pausedBackground;
SDL_FRect tempRect;
SDL_FRect boardRect;
SDL_FRect boardHalfRect;

void displayBlock(SDL_FRect rect, int r, int g, int b) {
    int i, j, cX = 0, cY = 0;
    int minX = rect.x, maxX = rect.w+rect.x, minY = rect.y, maxY = rect.h+rect.y;
    
    int cR = SDL_clamp(r-45, 0, 255);
    int cG = SDL_clamp(g-45, 0, 255);
    int cB = SDL_clamp(b-45, 0, 255);

    int bR = SDL_clamp(r-90, 0, 255);
    int bG = SDL_clamp(g-90, 0, 255);
    int bB = SDL_clamp(b-90, 0, 255);

    SDL_FRect pixelRect;
    pixelRect.w = pixelRect.h = 1;
    for (i = minY; i < maxY; i++) {
        cY++;
        for (j = minX; j < maxX; j++) { 
            cX++;
            if (i > minY+3 && i < maxY-3 && j > minX+3 && j < maxX-3) {
                SDL_SetRenderDrawColor(app.renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(app.renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(app.renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(app.renderer, &pixelRect);
        }
        cX = 0;
    }
}

void displayBlockToTexture(SDL_FRect rect, int r, int g, int b, SDL_Texture* texture, bool isWireframe) {
    int i, j, cX = 0, cY = 0;
    int minX = rect.x, maxX = rect.w+rect.x, minY = rect.y, maxY = rect.h+rect.y;
    
    int cR = SDL_clamp(r-45, 0, 255);
    int cG = SDL_clamp(g-45, 0, 255);
    int cB = SDL_clamp(b-45, 0, 255);

    int bR = SDL_clamp(r-90, 0, 255);
    int bG = SDL_clamp(g-90, 0, 255);
    int bB = SDL_clamp(b-90, 0, 255);

    SDL_SetRenderTarget(app.renderer, texture);

    SDL_FRect pixelRect;
    pixelRect.w = pixelRect.h = 1;
    for (i = minY; i < maxY; i++) {
        cY++;
        for (j = minX; j < maxX; j++) { 
            cX++;
            if (i > minY+3 && i < maxY-3 && j > minX+3 && j < maxX-3) {
                if (!isWireframe) {
                    SDL_SetRenderDrawColor(app.renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
                } else {
                    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
                }
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(app.renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(app.renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(app.renderer, &pixelRect);
        }
        cX = 0;
    }

    SDL_SetRenderTarget(app.renderer, NULL);
}

void buildBoardTexture() {
    SDL_SetRenderTarget(app.renderer, boardTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);

    SDL_FRect rect;
    rect.w = rect.h = TETROMINO_BLOCK_SIZE;

    for (int i = 0; i < 22; i++) { 
        for (int j = 0; j < 12; j++) {
            if (filledBlocks[i][j].v == true) {
                rect.x = j * TETROMINO_BLOCK_SIZE;
                rect.y = i * TETROMINO_BLOCK_SIZE;

                displayBlockToTexture(rect, filledBlocks[i][j].r, filledBlocks[i][j].g, filledBlocks[i][j].b, boardTexture, false);
            }
        }
    }
    if (app.showWireframe) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (wireframeTet.blocks[y][x].active) {
                    int posX = wireframeTet.x + x;
                    int posY = wireframeTet.y + y;
                    SDL_FRect Brect = {
                        posX * TETROMINO_BLOCK_SIZE,
                        posY * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlockToTexture(Brect, wireframeTet.r, wireframeTet.g, wireframeTet.b, boardTexture, true);
                }
            }
        }
    }

    SDL_SetRenderTarget(app.renderer, NULL);
}

//TODO: improve efficency by moving rect def to widow resize
void renderBoard() {
    SDL_FRect displayRect = {
        app.bWidthMin*TETROMINO_BLOCK_SIZE,
        app.bHeightMin*TETROMINO_BLOCK_SIZE,
        12*TETROMINO_BLOCK_SIZE,
        22*TETROMINO_BLOCK_SIZE
    };

    SDL_RenderTexture(app.renderer, boardTexture, NULL, &displayRect);
}

void updateNextBlocks() {
    SDL_SetRenderTarget(app.renderer, nextTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);

    int posX, posY;

    //display next blocks
    for (int block = 0; block < 4; block++) {
        for (int d = 0; d < 4; d++) {
            for (int e = 0; e < 4; e++) {
                if (app.tetArray[app.nextBlocks[block]]->blocks[d][e].active == true) {
                    posX = e * TETROMINO_BLOCK_SIZE;
                    posY = d * TETROMINO_BLOCK_SIZE + ((TETROMINO_BLOCK_SIZE*3) * block);
                    SDL_FRect Nrect = {
                        posX,
                        posY,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlockToTexture(Nrect, app.tetArray[app.nextBlocks[block]]->r, app.tetArray[app.nextBlocks[block]]->g, app.tetArray[app.nextBlocks[block]]->b, nextTexture, false);
                }
            }
        }
    }

    SDL_SetRenderTarget(app.renderer, NULL); // back to screen
}

void displayNextBlocks() {
    SDL_FRect displayRect = {
        (app.bWidthMax * TETROMINO_BLOCK_SIZE) + 60,
        (app.bHeightMin * TETROMINO_BLOCK_SIZE) + 150,
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    };

    SDL_RenderTexture(app.renderer, nextTexture, NULL, &displayRect);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // -- setup --
    app.width = 1000;
    app.height = 750;
    app.titleCard = true;
    app.showWireframe = true;
    app.heldtet = -1;

    SDL_FRect temp4 = {0, 0, app.width, app.height};
    pausedBackground = temp4;
    SDL_FRect temp5 = {(app.width >> 1), (app.height >> 1), 1, 1};
    tempRect = temp5;

    app.bWidthMin = ((app.width / TETROMINO_BLOCK_SIZE) >> 1) - 5; // - 5
    app.bWidthMax = ((app.width / TETROMINO_BLOCK_SIZE) >> 1) + 5; // + 5

    app.bHeightMin = ((app.height / TETROMINO_BLOCK_SIZE) >> 1) - 10; // - 10
    app.bHeightMax = ((app.height / TETROMINO_BLOCK_SIZE) >> 1) + 10; // + 10

    halfTitleWidth = (app.width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
    halfTitleHeight = ((app.height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

    SDL_FRect temp6 = {
        app.bWidthMin*TETROMINO_BLOCK_SIZE, 
        app.bWidthMin*TETROMINO_BLOCK_SIZE, 
        app.bWidthMax*TETROMINO_BLOCK_SIZE, 
        app.bHeightMax*TETROMINO_BLOCK_SIZE
    };
    boardRect = temp6;

    SDL_FRect temp7 ={ 
        ((app.width / TETROMINO_BLOCK_SIZE) >> 1),
        ((app.height / TETROMINO_BLOCK_SIZE) >> 1),
        1,
        1
    };
    boardHalfRect = temp7;

    setupTetrominos(&app);
    setupTitleBlocks(&app);

    setColourDef(&colour[0], 0, 0, 255); //blue
    setColourDef(&colour[1], 0, 255, 255); //turquoise
    setColourDef(&colour[2], 0, 255, 0); //green
    setColourDef(&colour[3], 255, 255, 0); //yellow
    setColourDef(&colour[4], 255, 160, 0); //orange
    setColourDef(&colour[5], 255, 0, 0); //red
    setColourDef(&colour[6], 150, 0, 255); //purple

    // -- border -- 
    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 12; j++) {
            if (i == 0 || i == 21 || j == 0 || j == 11) {
                filledBlocks[i][j].v = true;
                filledBlocks[i][j].r = filledBlocks[i][j].g = filledBlocks[i][j].b = 125;
            } else {
                filledBlocks[i][j].v = false;
            }
        }
    }

    // -- init --
    SDL_SetAppMetadata("Play Tetis!", "1.1.0", "com/LKerr42/SDL-Tetris.github");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize ttf: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // -- window --
    bool wind = SDL_CreateWindowAndRenderer(
        "Play Tetris!", 
        app.width, app.height, 
        SDL_WINDOW_RESIZABLE,
        &app.window, &app.renderer
    );
    if (!wind) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // -- icon --
    icon = IMG_Load("assets/iconT.ico");
    SDL_SetWindowIcon(app.window, icon);
    SDL_DestroySurface(icon);

    // -- keyboard image --
    keyboardSurface = IMG_Load("assets/keyboard.png");
    if (keyboardSurface == NULL) {
        SDL_Log("Failed to load keyboardSurface: %s", SDL_GetError());
    }
    keyboard = SDL_CreateTextureFromSurface(app.renderer, keyboardSurface);
    if (keyboard == NULL) {
        SDL_Log("Failed to load keyboard: %s", SDL_GetError());
    }

    SDL_GetTextureSize(keyboard, &keyW, &keyH);

    int intKeyH = (int)keyH, intKeyW = (int)keyW;

    int keyX = (app.width >> 1) - (intKeyW >> 1);
    int keyY = (app.height >> 1) - (intKeyH >> 1);

    SDL_FRect temp = {keyX, keyY-5, intKeyW, intKeyH};
    keyRect = temp;

    // -- fonts -- 
    app.globalFont = TTF_OpenFont("assets\\NES.ttf", 25);
    if (app.globalFont == NULL) {
        SDL_Log("Failed to load font app.globalFont: %s", SDL_GetError());
    }

    app.globalFontS = TTF_OpenFont("assets\\NES.ttf", 17);
    if (app.globalFontS == NULL) {
        SDL_Log("Failed to load font app.globalFontS: %s", SDL_GetError());
    }

    app.globalFontL = TTF_OpenFont("assets\\NES.ttf", 32);
    if (app.globalFontL == NULL) {
        SDL_Log("Failed to load font app.globalFontL: %s", SDL_GetError());
    }

    setupStaticText(&app);

    // -- texture setup --
    boardTexture = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        12*TETROMINO_BLOCK_SIZE,
        22*TETROMINO_BLOCK_SIZE
    );
    if (!boardTexture) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    buildBoardTexture();

    nextTexture = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    );
    if (!nextTexture) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    app.backgroundKeyboard  = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        intKeyW, intKeyH
    );
    if (!app.backgroundKeyboard) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    app.keyboardText = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        intKeyW, 100
    );
    if (!app.keyboardText) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    SDL_FRect temp2 = {keyX, keyY+intKeyH+5, intKeyW, 100};
    keyboardTextRect = temp2;

    // -- next blocks -- 
    for (int k = 0; k < 4; k++) {
        app.nextBlocks[k] = SDL_rand(7);
    }

    updateNextBlocks();

    // -- audio --
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL_Init AUDIO failed: %s", SDL_GetError());
    }

    char *wavPath = NULL;
    for (int i = 0; i < SDL_arraysize(sfx); i++) {
        SDL_asprintf(&wavPath, "%s%s.wav", "assets/audio/", fileNames[i]);
        initaliseAudioFile(&sfx[i], wavPath);
    }
    initaliseAudioFile(&mainTheme, "assets/audio/Tetris.wav");
    SDL_SetAudioStreamGain(mainTheme.stream, 0.5f);
    SDL_SetAudioStreamGain(sfx[LAND].stream, 1.5f);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

bool canMove(tetromino *t, int dx, int dy) {
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

void rotateTetrominoCCW(tetromino *t) {
    startSound(&sfx[SPINCCW]);
    int N, Y, X, rx, ry;
    blockStruct temp[4][4];
    if (app.currentBlock == 0) {
        N = 4;
    } else if (app.currentBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

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

    if (!canMove(t, 0, 0)) {
        if (canMove(t, 1, 0)) {
            t->x += 1;
        } else if (canMove(t, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCW(t);
        }
    }
}

void rotateTetrominoCW(tetromino *t) {
    startSound(&sfx[SPINCW]);
    int N, Y, X;
    blockStruct temp[4][4];
    if (app.currentBlock == 0) {
        N = 4;
    } else if (app.currentBlock == 3) {
        N = 2;
    } else {
        N = 3;
    }

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


    if (!canMove(t, 0, 0)) {
        if (canMove(t, 1, 0)) {
            t->x += 1;
        } else if (canMove(t, -1, 0)) {
            t->x -= 1;
        } else {
            rotateTetrominoCCW(t);
        }
    }
}

void resetBoard() {
    for (int i = 0; i < 22; i++) {
        for (int j = 0; j < 12; j++) {
            if (i == 0 || i == 21 || j == 0 || j == 11) {
                filledBlocks[i][j].v = true;
                filledBlocks[i][j].r = filledBlocks[i][j].g = filledBlocks[i][j].b = 125;
            } else {
                filledBlocks[i][j].v = false;
            }
        }
    }
    buildBoardTexture();
    updateNextBlocks();
    displayNextBlocks();
    renderBoard();
}

void resetGame() {
    resetBoard();
    for (int i = 0; i < 7; i++) {
        scoreString[i] = '0';
        if (i < 4) {
            app.nextBlocks[i] = SDL_rand(7);
        }
    }
    score = 0;
    app.heldtet = -1;
    app.currentBlock = 0;
    app.currentTet->x = 4;
    app.currentTet->y = 1;
    app.winning = true;
    firstRun = true;
}

void runWireframes(tetromino *copyTet) {
    if (app.showWireframe) {
        wireframeTet = *copyTet;
        while (true) {
            if (canMove(&wireframeTet, 0, 1)) {
                wireframeTet.y += 1;
            } else {
                buildBoardTexture();
                break;
            }
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
            if (app.keyboardCard) {
                handleInputKeyboardCard(&app, event->key.scancode, true);
            } else {
                handleKeyboardInput(&app, event->key.scancode);
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            if (app.keyboardCard) {
                handleInputKeyboardCard(&app, event->key.scancode, false);
            } 
            if (event->key.scancode == SDL_SCANCODE_DOWN && app.winning) {
                app.amountPressedDown++;
            }
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            SDL_GetWindowSize(app.window, &app.width, &app.height);
            app.bWidthMin = ((app.width / TETROMINO_BLOCK_SIZE) >> 1) - 6; // - 5
            app.bWidthMax = ((app.width / TETROMINO_BLOCK_SIZE) >> 1) + 6; // + 5

            app.bHeightMin = ((app.height / TETROMINO_BLOCK_SIZE) >> 1) - 11; // - 10
            app.bHeightMax = ((app.height / TETROMINO_BLOCK_SIZE) >> 1) + 11; // + 10
            setupStaticText(&app);

            keyRect.x = (app.width >> 1) - ((int)keyW/2);
            keyRect.y = (app.height >> 1) - ((int)keyH/2) - 5;

            keyboardTextRect.x = keyRect.x;
            keyboardTextRect.y = keyRect.y + keyRect.h + 10;

            halfTitleWidth = (app.width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
            halfTitleHeight = ((app.height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

            SDL_FRect temp2 = {0, 0, app.width, app.height};
            pausedBackground = temp2;

            SDL_FRect temp5 = {(app.width >> 1), (app.height >> 1), 1, 1};
            tempRect = temp5;

            SDL_FRect temp6 = {
                app.bWidthMin*TETROMINO_BLOCK_SIZE, 
                app.bWidthMin*TETROMINO_BLOCK_SIZE, 
                app.bWidthMax*TETROMINO_BLOCK_SIZE, 
                app.bHeightMax*TETROMINO_BLOCK_SIZE
            };
            boardRect = temp6;

            SDL_FRect temp7 ={ 
                ((app.width / TETROMINO_BLOCK_SIZE) >> 1),
                ((app.height / TETROMINO_BLOCK_SIZE) >> 1),
                1,
                1
            };
            boardHalfRect = temp7;
            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void moveBoardDown(int remove) { //TODO: add sweeping animation to this
    //start at the index and set each to the one above
    for (int i = remove; i >= 2; i--) {
        for (int j = 1; j <= 10; j++) {
            filledBlocks[i][j] = filledBlocks[i-1][j];
        }
    }
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    Uint64 now = SDL_GetTicks();
    static Uint64 lastFallTime = 0, lastChange = 0, lastPress = 0, lastPressDown = 0;

    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);

    for (int ii = 0; ii < SDL_arraysize(sfx); ii++) {
        if (sfx[ii].playing == true) {
            playWAV(&sfx[ii], false);
        }
    }
    playWAV(&mainTheme, true);

    if (app.titleCard) {
        //change through colours
        if (now - lastChange >= 500) {
            if (currentMove == 6) {
                currentMove = 0;
                currentColour++;
                if (currentColour == 7) currentColour = 0;
            }

            app.titleTetroes[currentMove]->r = colour[currentColour].r;
            app.titleTetroes[currentMove]->g = colour[currentColour].g;
            app.titleTetroes[currentMove]->b = colour[currentColour].b;

            currentMove++;
            lastChange = now;
        }
        //text
        displayText(&app, "Tribute by LKerr42", -1, (app.height >> 1) + (TETROMINO_BLOCK_SIZE << 1), app.globalFont, 255, 255, 255);
        displayText(&app, "Press Space to start", -1, 10, app.globalFont, 255, 255, 255);

        //display blocks
        for (int countB = 0; countB < 6; countB++) {
            for (int Y = 0; Y < 5; Y++) {
                for (int X = 0; X < 4; X++) {
                    if (app.titleTetroes[countB]->titleBlocks[Y][X].active == true) {
                        posX = app.titleTetroes[countB]->x + (X * TETROMINO_BLOCK_SIZE) + halfTitleWidth;
                        posY = app.titleTetroes[countB]->y + (Y* TETROMINO_BLOCK_SIZE) + halfTitleHeight;

                        SDL_FRect Trect = {
                            posX,
                            posY - (TETROMINO_BLOCK_SIZE << 1),
                            TETROMINO_BLOCK_SIZE,
                            TETROMINO_BLOCK_SIZE
                        };
                        displayBlock(Trect, app.titleTetroes[countB]->r, app.titleTetroes[countB]->g, app.titleTetroes[countB]->b);
                    }
                }
            }
        }
    } else if (app.keyboardCard) {
        SDL_RenderTexture(app.renderer, app.keyboardText, NULL, &keyboardTextRect);
        SDL_RenderTexture(app.renderer, app.backgroundKeyboard, NULL, &keyRect);
        SDL_RenderTexture(app.renderer, keyboard, NULL, &keyRect);
        if (app.amountPressed == 1) {
            lastPress++;
        } else if (app.amountPressed == 2) {
            restartMainTheme();
            startSound(&sfx[OPEN]);
            app.keyboardCard = false;
            app.winning = true;
            app.amountPressed = 0;
        }

        if (lastPress >= 500) {
            app.amountPressed = 0;
            lastPress = 0;
        }
    } else if (app.winning) { 
        displayStaticText(&app);
        //For some reason the firstBlocks int array corrupts between the start of this and the end of space being pressed
        if (firstRun == true) {
            updateNextBlocks(); 
            firstRun = false;
            *app.currentTet = *app.tetArray[app.currentBlock];
            runWireframes(app.currentTet);
        }
        displayText(&app, scoreString, (app.bWidthMax*TETROMINO_BLOCK_SIZE)+60, (app.bHeightMin*TETROMINO_BLOCK_SIZE)+49, app.globalFont, 255, 255, 255);
        int countBlocks = 0, linesCleared = 0;
        char incompleteScore[7];

        if (app.amountPressedDown == 1) {
            lastPressDown++;
        } else if (app.amountPressedDown == 2) {
            while (true) {
                if (canMove(app.currentTet, 0, 1)) {
                    app.currentTet->y += 1;
                } else {
                    break;
                }
            }
            app.amountPressedDown = 0;
            lastPressDown = 0;
        }

        if (lastPressDown >= 500) {
            app.amountPressedDown = 0;
            lastPressDown = 0;
        }

        if (now - lastFallTime >= 1000) {
            //check if any are above a set block
            if (canMove(app.currentTet, 0, 1) && !app.paused) {
                app.currentTet->y += 1;
            } else if (!app.paused) {
                startSound(&sfx[LAND]);
                //set block
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        if (app.currentTet->blocks[k][l].active == true) {
                            posX = app.currentTet->x+app.currentTet->blocks[k][l].x;
                            posY = app.currentTet->y+app.currentTet->blocks[k][l].y;

                            filledBlocks[posY][posX].v = true; 
                            filledBlocks[posY][posX].r = app.currentTet->blocks[k][l].r; 
                            filledBlocks[posY][posX].g = app.currentTet->blocks[k][l].g;
                            filledBlocks[posY][posX].b = app.currentTet->blocks[k][l].b;
                        }
                    }
                }

                //reset block
                app.currentBlock = app.nextBlocks[0];
                for (int n = 0; n < 3; n++) {
                    app.nextBlocks[n] = app.nextBlocks[n+1];
                }
                app.nextBlocks[3] = (int)SDL_rand(7); 
                *app.currentTet = *app.tetArray[app.currentBlock];
                app.currentTet->x = 4;
                app.currentTet->y = 1;
                
                updateNextBlocks();

                //check if row filled
                for (int l = 1; l <= 20; l++) {
                    countBlocks = 0;
                    for (int k = 1; k <= 10 ; k++) {
                        if (filledBlocks[l][k].v == true) {
                            countBlocks++;
                        }
                    }
                    if (countBlocks == 10) {
                        moveBoardDown(l);
                        linesCleared++;
                    }
                }

                runWireframes(app.currentTet);

                //update score
                if (linesCleared > 0) {
                    startSound(&sfx[CLEAR]);
                    switch (linesCleared) {
                        case 1:
                            score += 40;
                            break;
                        case 2:
                            score += 100;
                            break;
                        case 3:
                            score += 300;
                            break;
                        case 4:
                            score += 1200;
                            break;
                    }

                    //update string
                    sprintf(incompleteScore, "%d", score);
                    while (strlen(incompleteScore) != 7) {
                        prependChar(incompleteScore, '0');
                    }
                    strcpy(scoreString, incompleteScore);
                }

                buildBoardTexture();

                //lose condition
                for (int m = 1; m < 11; m++) {
                    if (filledBlocks[1][m].v == true) {
                        app.winning = false;
                        SDL_asprintf(&finalScore, "Final Score: %s", scoreString);
                        break;
                    }
                }
            }

            lastFallTime = now;
        }

        
        displayNextBlocks();
        renderBoard();

        //display falling tetromino
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (app.currentTet->blocks[y][x].active) {
                    posX = app.currentTet->x + x;
                    posY = app.currentTet->y + y;
                    SDL_FRect Brect = {
                        (posX + app.bWidthMin) * TETROMINO_BLOCK_SIZE,
                        (posY + app.bHeightMin) * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlock(Brect, app.currentTet->r, app.currentTet->g, app.currentTet->b);
                }
            }
        }

        int baseX = (app.bWidthMin*TETROMINO_BLOCK_SIZE)-180 + (TETROMINO_BLOCK_SIZE << 1);
        int baseY = (app.bHeightMin*TETROMINO_BLOCK_SIZE)+70;

        //display held tetromino
        if (app.heldtet != -1) {
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    if (app.tetArray[app.heldtet]->blocks[k][l].active == true) {
                        SDL_FRect Hrect = {
                            l * TETROMINO_BLOCK_SIZE + baseX,
                            k * TETROMINO_BLOCK_SIZE + baseY,
                            TETROMINO_BLOCK_SIZE,
                            TETROMINO_BLOCK_SIZE
                        };
                        displayBlock(Hrect, app.tetArray[app.heldtet]->r, app.tetArray[app.heldtet]->g, app.tetArray[app.heldtet]->b);
                    }
                }
            } 
        }

        if (app.paused) {
            SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128);
            SDL_RenderFillRect(app.renderer, &pausedBackground);
            displayText(&app, "-Paused-", -1, -1, app.globalFontL, 255, 255, 255);
        } 

        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(app.renderer, &tempRect);
        SDL_RenderRect(app.renderer, &boardRect);

        SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(app.renderer, &boardHalfRect);
    } else if (app.titleCard == false) { //lose card
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
        SDL_RenderClear(app.renderer);
        bool displayed = false;
        displayText(&app, "You Lost!", -1, -1, app.globalFont, 255, 255, 255);
        displayText(&app, finalScore, -1, (app.height >> 1) + 50, app.globalFontS, 255, 255, 255);
        displayText(&app, "Press space to start again", -1, (app.height >> 1) + 100, app.globalFontS, 255, 255, 255);
    }

    SDL_RenderPresent(app.renderer); // render everything

    return SDL_APP_CONTINUE;  // carry on with the program!
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    SDL_free(app.renderer);
    SDL_free(app.window);

    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(app.staticText);
    SDL_DestroyTexture(keyboard);
    SDL_DestroyTexture(app.keyboardText);
    for (int k = 0; k < 300; k++) {
        if (app.textArray != NULL) {
            SDL_free(app.textArray->tex);
        }
    }

    TTF_CloseFont(app.globalFont);
    TTF_CloseFont(app.globalFontS);
    TTF_Quit();

    for (int i = 0; i < SDL_arraysize(sfx); i++) {
        if (sfx[i].stream) {
            SDL_DestroyAudioStream(sfx[i].stream);
        }
        SDL_free(sfx[i].audio_buff);
    }
    SDL_DestroyAudioStream(mainTheme.stream);
    SDL_free(mainTheme.audio_buff);

    SDL_free(app.currentTet);
    for (int j = 0; j < 7; j++) {
        SDL_free(app.tetArray[j]);
        if (j < 6) {
            SDL_free(app.titleTetroes[j]);
        }
    }
}