#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include "include/app.h"
#include "include/audio.h"
#include "include/text.h"
#include "include/keyboard.h"
#include "include/tetromino.h"
#include "include/renderer.h"
#include "include/game_control.h"

// Context for the whole app
appContext app;

//file specific globals
int currentMove = 0, currentColour = 1;
int halfTitleWidth, halfTitleHeight, posX = 0, posY = 0;
float keyW, keyH;

char *finalScore = NULL;
char fileNames[9][16] = { //TODO: Add sounds: lose, main theme B
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

SDL_Surface *icon;
SDL_Surface *keyboardSurface;

SDL_FRect keyRect;
SDL_FRect keyboardTextRect;
SDL_FRect pausedBackground;

colours colour[7];

// -- SDL interface functions --

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // -- app setup --
    app.width = 1000;
    app.height = 750;
    app.score = 0;
    app.titleCard = true;
    app.showWireframe = true;
    app.firstRun = true;
    app.heldtet = -1;
    app.amountPressed = -1;
    clearLinesStruct(&app);

    SDL_FRect temp4 = {0, 0, app.width, app.height};
    pausedBackground = temp4;

    // -- width and heights --
    app.bWidthMin = (app.width >> 1) - (6*TETROMINO_BLOCK_SIZE); // - 5
    app.bWidthMax = (app.width >> 1) + (6*TETROMINO_BLOCK_SIZE); // + 5

    app.bHeightMin = (app.height >> 1) - (11*TETROMINO_BLOCK_SIZE); // - 10
    app.bHeightMax = (app.height >> 1) + (11*TETROMINO_BLOCK_SIZE);; // + 10

    halfTitleWidth = (app.width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
    halfTitleHeight = ((app.height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

    // -- setup tetrominoes --
    setupTetrominos(&app);
    setupTitleBlocks(&app);

    // -- colours --
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
                app.filledBlocks[i][j].v = true;
                app.filledBlocks[i][j].r = app.filledBlocks[i][j].g = app.filledBlocks[i][j].b = 125;
            } else {
                app.filledBlocks[i][j].v = false;
            }
        }
    }

    // -- SDL init --
    SDL_SetAppMetadata("Play Tetis!", "1.1.4", "com/LKerr42/SDL-Tetris.github");

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
    app.keyboard = SDL_CreateTextureFromSurface(app.renderer, keyboardSurface);
    if (app.keyboard == NULL) {
        SDL_Log("Failed to load keyboard: %s", SDL_GetError());
    }

    SDL_GetTextureSize(app.keyboard, &keyW, &keyH);

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
    app.boardTexture = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        12*TETROMINO_BLOCK_SIZE,
        22*TETROMINO_BLOCK_SIZE
    );
    if (!app.boardTexture) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    buildBoardTexture(&app);

    app.nextTexture = SDL_CreateTexture(
        app.renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    );
    if (!app.nextTexture) {
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

    updateNextBlocks(&app);

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
            app.bWidthMin = (app.width >> 1) - (6*TETROMINO_BLOCK_SIZE); // - 5
            app.bWidthMax = (app.width >> 1) + (6*TETROMINO_BLOCK_SIZE); // + 5

            app.bHeightMin = (app.height >> 1) - (11*TETROMINO_BLOCK_SIZE); // - 10
            app.bHeightMax = (app.height >> 1) + (11*TETROMINO_BLOCK_SIZE);; // + 10
            setupStaticText(&app);

            keyRect.x = (app.width >> 1) - ((int)keyW/2);
            keyRect.y = (app.height >> 1) - ((int)keyH/2) - 5;

            keyboardTextRect.x = keyRect.x;
            keyboardTextRect.y = keyRect.y + keyRect.h + 10;

            halfTitleWidth = (app.width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
            halfTitleHeight = ((app.height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

            SDL_FRect temp2 = {0, 0, app.width, app.height};
            pausedBackground = temp2;

            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
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

    updateLineClear(&app, now);

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
                        displayBlock(&app, Trect, app.titleTetroes[countB]->r, app.titleTetroes[countB]->g, app.titleTetroes[countB]->b);
                    }
                }
            }
        }
    } else if (app.keyboardCard) {
        SDL_RenderTexture(app.renderer, app.keyboardText, NULL, &keyboardTextRect);
        SDL_RenderTexture(app.renderer, app.backgroundKeyboard, NULL, &keyRect);
        SDL_RenderTexture(app.renderer, app.keyboard, NULL, &keyRect);
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
        if (app.firstRun == true) {
            updateNextBlocks(&app); 
            app.firstRun = false;
            *app.currentTet = *app.tetArray[app.currentBlock];
            runWireframes(&app, app.currentTet);
            SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);

            sprintf(app.scoreString, "%s", "0000000");

        }
        displayText(&app, app.scoreString, (app.bWidthMax+20), (app.bHeightMin+49), app.globalFont, 255, 255, 255);
        int countBlocks = 0, linesCleared = 0;
        char incompleteScore[7];

        if (app.amountPressedDown == 1) {
            lastPressDown++;
        } else if (app.amountPressedDown == 2) {
            while (true) { 
                if (canMove(&app, app.currentTet, 0, 1)) {
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
            if (canMove(&app, app.currentTet, 0, 1) && !app.paused) {
                app.currentTet->y += 1;
            } else if (!app.paused) {
                startSound(&sfx[LAND]);
        
                //set block
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        if (app.currentTet->blocks[k][l].active == true) {
                            posX = app.currentTet->x+app.currentTet->blocks[k][l].x;
                            posY = app.currentTet->y+app.currentTet->blocks[k][l].y;

                            app.filledBlocks[posY][posX].v = true; 
                            app.filledBlocks[posY][posX].r = app.currentTet->blocks[k][l].r; 
                            app.filledBlocks[posY][posX].g = app.currentTet->blocks[k][l].g;
                            app.filledBlocks[posY][posX].b = app.currentTet->blocks[k][l].b;
                        }
                    }
                }
            
                //lose condition
                for (int m = 1; m < 11; m++) {
                    if (app.filledBlocks[1][m].v == true) {
                        app.winning = false;
                        app.loseCard = true;
                        SDL_asprintf(&finalScore, "Final Score: %s", app.scoreString);
                        break;
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
                
                updateNextBlocks(&app);

                //check if row filled
                for (int l = 1; l <= 20; l++) {
                    countBlocks = 0;
                    for (int k = 1; k <= 10 ; k++) {
                        if (app.filledBlocks[l][k].v == true) {
                            countBlocks++;
                        }
                    }
                    if (countBlocks == 10) {
                        pushBackToLinesArray(&app, l);
                        linesCleared++;
                    }
                }

                for (int check = 1; check < 5; check++) {
                    printf("%d, ", app.clearInst.rows[check]);
                }
                printf("\n");

                //update score
                if (linesCleared > 0) {
                    startLineClear(&app);

                    startSound(&sfx[CLEAR]);
                    switch (linesCleared) {
                        case 1:
                            app.score += 40;
                            break;
                        case 2:
                            app.score += 100;
                            break;
                        case 3:
                            app.score += 300;
                            break;
                        case 4:
                            app.score += 1200;
                            break;
                    }

                    //update string
                    sprintf(incompleteScore, "%d", app.score);
                    while (strlen(incompleteScore) != 7) {
                        prependChar(incompleteScore, '0');
                    }
                    strcpy(app.scoreString, incompleteScore);

                    app.clearInst.amountLines = linesCleared;
                }

                runWireframes(&app, app.currentTet);
                buildBoardTexture(&app);
            }

            lastFallTime = now;
        }
        
        displayNextBlocks(&app);
        renderBoard(&app);

        //display falling tetromino
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (app.currentTet->blocks[y][x].active) {
                    posX = (app.currentTet->x + x) * TETROMINO_BLOCK_SIZE;
                    posY = (app.currentTet->y + y) * TETROMINO_BLOCK_SIZE;
                    SDL_FRect Brect = {
                        (app.bWidthMin + posX),
                        (app.bHeightMin + posY),
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlock(&app, Brect, app.currentTet->r, app.currentTet->g, app.currentTet->b);
                }
            }
        }

        int baseX = (app.bWidthMin-180) + (TETROMINO_BLOCK_SIZE << 1);
        int baseY = (app.bHeightMin+70);

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
                        displayBlock(&app, Hrect, app.tetArray[app.heldtet]->r, app.tetArray[app.heldtet]->g, app.tetArray[app.heldtet]->b);
                    }
                }
            } 
        }

        if (app.paused && app.userPause) {
            SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128);
            SDL_RenderFillRect(app.renderer, &pausedBackground);
            displayText(&app, "-Paused-", -1, -1, app.globalFontL, 255, 255, 255);
        } 
    } else if (app.loseCard) {
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
    closeApp(&app);
}