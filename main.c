#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define TETROMINO_BLOCK_SIZE 20
#define KEYBOARD_PIXEL_SIZE  10
#define CLEAR     0 
#define HOLYMOLY  1
#define LAND      2
#define MOVELEFT  3
#define MOVERIGHT 4
#define OPEN      5
#define SPINCCW   6
#define SPINCW    7
#define SWITCH    8

#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define min(X, Y) ((X) < (Y) ? (X) : (Y))

/* We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

SDL_Texture *staticText;

int width = 1000, height = 750;

// Blocks
int bWidthMin, bWidthMax, bHeightMin, bHeightMax; 
int currentBlock = 0, score = 0;
float textW, textH;

int nextBlocks[4];

bool winning = false, keyboardCard = false, titleCard = true, firstRun = true;
bool showWireframe = true;
char scoreString[7] = "0000000";

TTF_Font* globalFont;
TTF_Font* globalFontS;

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
    blockStruct titleBlocks[5][4];
    int r;
    int g;
    int b;
    int x;
    int y;
} tetromino;
tetromino tetArray[7];
tetromino titleTetroes[6];
tetromino currentTet;
tetromino wireframeTet;

typedef struct {
    int r;
    int g;
    int b;
} colours;
colours colour[7];

typedef struct {
    char source[32];
    Uint8 *audio_buff;
    Uint32 audio_len;
    Uint32 cursor;
    bool playing;
    SDL_AudioStream *stream;
} sound;
sound sfx[9];
sound mainTheme;

typedef struct {
    SDL_Texture *tex;
    SDL_FRect dest;
} textTexture;

void rotateTetrominoCCW(tetromino *t);
void rotateTetrominoCW(tetromino *t);
void setupStaticText();

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

void setColourDef(colours *data, int R, int G, int B) {
    data -> r = R;
    data -> g = G;
    data -> b = B;
}

void prependChar(char *str, char c) {
    size_t len = strlen(str);

    memmove(str + 1, str, len + 1);

    str[0] = c;
}

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

void setupTetrominos() {
    setTetColour(&tetArray[0], 0, 255, 255); //Long boy
    setTetColour(&tetArray[1], 0, 0, 255);   //Left L
    setTetColour(&tetArray[2], 255, 160, 0); //Right L
    setTetColour(&tetArray[3], 255, 255, 0); //Square 
    setTetColour(&tetArray[4], 0, 255, 0);   //Right squiggle
    setTetColour(&tetArray[5], 150, 0, 255); //T boy
    setTetColour(&tetArray[6], 255, 0, 0);   //Left squiggle

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

void setupTitleBlocks() {
    int letters[6][5][4] = {
        //T
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        //E
        {{1, 1, 1, 0}, 
         {1, 0, 0, 0},
         {1, 1, 0, 0},
         {1, 0, 0, 0},
         {1, 1, 1, 0}},
        //T
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        //R
        {{1, 1, 1, 0}, 
         {1, 0, 0, 1},
         {1, 1, 1, 0},
         {1, 0, 1, 0},
         {1, 0, 0, 1}},
        //I
        {{1, 1, 1, 0}, 
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {1, 1, 1, 0}},
        //S
        {{0, 1, 1, 1}, 
         {1, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 1},
         {1, 1, 1, 0}},
    };

    for (int count = 0; count < 6; count++) {
        setTetColour(&titleTetroes[count], 0, 0, 255);

        for (int Y = 0; Y < 5; Y++) {
            for (int X = 0; X < 4; X++) {
                titleTetroes[count].titleBlocks[Y][X].x = X;
                titleTetroes[count].titleBlocks[Y][X].y = Y;

                if (letters[count][Y][X] == 1) {
                    titleTetroes[count].titleBlocks[Y][X].active = true;
                    setBlockColour(&titleTetroes[count].titleBlocks[Y][X], titleTetroes[count].r, titleTetroes[count].g, titleTetroes[count].b);
                } else {
                    titleTetroes[count].titleBlocks[Y][X].active = false;
                }
                

            }
        }
        titleTetroes[count].x = (TETROMINO_BLOCK_SIZE << 2) * count;
        if (count == 4 || count == 5) titleTetroes[count].x += TETROMINO_BLOCK_SIZE;
        titleTetroes[count].y = 0;
    }
}

SDL_Texture *boardTexture;
SDL_Texture *nextTexture;
SDL_Surface *icon;

SDL_Texture *keyboard;
SDL_Surface *keyboardSurface;
SDL_FRect keyRect;

SDL_Texture *backgroundKeyboard;
SDL_Texture *keyboardText;
SDL_FRect keyboardTextRect;

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
                SDL_SetRenderDrawColor(renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(renderer, &pixelRect);
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

    SDL_SetRenderTarget(renderer, texture);

    SDL_FRect pixelRect;
    pixelRect.w = pixelRect.h = 1;
    for (i = minY; i < maxY; i++) {
        cY++;
        for (j = minX; j < maxX; j++) { 
            cX++;
            if (i > minY+3 && i < maxY-3 && j > minX+3 && j < maxX-3) {
                if (!isWireframe) {
                    SDL_SetRenderDrawColor(renderer, cR, cG, cB, SDL_ALPHA_OPAQUE); 
                } else {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
                }
            } else if ((-cX+rect.w) - cY < -1) {
                SDL_SetRenderDrawColor(renderer, bR, bG, bB, SDL_ALPHA_OPAQUE); 
            } else {
                SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
            }
            pixelRect.y = i;
            pixelRect.x = j;
            SDL_RenderFillRect(renderer, &pixelRect);
        }
        cX = 0;
    }

    SDL_SetRenderTarget(renderer, NULL);
}

void buildBoardTexture() {
    SDL_SetRenderTarget(renderer, boardTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

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
    if (showWireframe) {
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

    SDL_SetRenderTarget(renderer, NULL);
}

void renderBoard() {
    SDL_FRect displayRect = {
        bWidthMin*TETROMINO_BLOCK_SIZE,
        bHeightMin*TETROMINO_BLOCK_SIZE,
        12*TETROMINO_BLOCK_SIZE,
        22*TETROMINO_BLOCK_SIZE
    };

    SDL_RenderTexture(renderer, boardTexture, NULL, &displayRect);
}

void updateNextBlocks() {
    SDL_SetRenderTarget(renderer, nextTexture);

    // Clear previous contents
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int posX, posY;

    //display next blocks
    for (int block = 0; block < 4; block++) {
        for (int d = 0; d < 4; d++) {
            for (int e = 0; e < 4; e++) {
                if (shapes[nextBlocks[block]][d][e] == 1) {
                    posX = e * TETROMINO_BLOCK_SIZE;
                    posY = d * TETROMINO_BLOCK_SIZE + ((TETROMINO_BLOCK_SIZE*3) * block);
                    SDL_FRect Nrect = {
                        posX,
                        posY,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlockToTexture(Nrect, tetArray[nextBlocks[block]].r, tetArray[nextBlocks[block]].g, tetArray[nextBlocks[block]].b, nextTexture, false);
                }
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL); // back to screen
}

//audio header
void displayNextBlocks() {
    SDL_FRect displayRect = {
        (bWidthMax * TETROMINO_BLOCK_SIZE) + 60,
        (bHeightMin * TETROMINO_BLOCK_SIZE) + 150,
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    };

    SDL_RenderTexture(renderer, nextTexture, NULL, &displayRect);
}

void startSound(sound *s) {
    s -> playing = true;
    SDL_ClearAudioStream(s->stream);
}

void initaliseAudioFile(sound *wavFile, char path[]) {
    SDL_AudioSpec spec;

    strcpy(wavFile -> source, path);
    wavFile -> audio_buff = NULL;
    wavFile -> audio_len = 0;
    wavFile -> playing = false;
    wavFile -> cursor = 0;

    if (!SDL_LoadWAV(path, &spec, &wavFile->audio_buff, & wavFile->audio_len)) {
        SDL_Log("Couldn't load .wav file: %s", SDL_GetError());
    }

    wavFile -> stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!wavFile -> stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
    }
    SDL_ResumeAudioStreamDevice(wavFile -> stream);
}

void playWAV(sound *wavFile, bool loop) {
    if (!loop) {
        if (!wavFile->playing) return;

        int queued = SDL_GetAudioStreamQueued(wavFile -> stream);

        Uint32 remaining = wavFile->audio_len - wavFile->cursor;

        if (queued < 48000) {
            if (remaining > 0) {
                SDL_PutAudioStreamData(
                    wavFile -> stream,
                    wavFile->audio_buff + wavFile->cursor,
                    remaining
                );
                wavFile->cursor += remaining;
            }
        }

        if (wavFile->cursor >= wavFile->audio_len && queued == 0) {
            wavFile->playing = false;
            wavFile->cursor = 0;
        }
    } else {
        if (SDL_GetAudioStreamQueued(wavFile -> stream) < ((int) wavFile -> audio_len)) {
            SDL_PutAudioStreamData(wavFile -> stream, wavFile -> audio_buff, (int) wavFile -> audio_len);
        }
    }
}

void restartMainTheme() {
    SDL_ClearAudioStream(mainTheme.stream);
}

// end audio header

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

float keyW, keyH;
int halfTitleWidth, halfTitleHeight;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    // -- setup --
    bWidthMin = ((width / TETROMINO_BLOCK_SIZE) >> 1) - 5;
    bWidthMax = ((width / TETROMINO_BLOCK_SIZE) >> 1) + 5;

    bHeightMin = ((height / TETROMINO_BLOCK_SIZE) >> 1) - 10;
    bHeightMax = ((height / TETROMINO_BLOCK_SIZE) >> 1) + 10;

    halfTitleWidth = (width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
    halfTitleHeight = ((height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

    setupTetrominos();
    setupTitleBlocks();

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
        width, height, 
        SDL_WINDOW_RESIZABLE,
        &window, &renderer
    );
    if (!wind) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // -- icon --
    icon = IMG_Load("assets/iconT.ico");
    SDL_SetWindowIcon(window, icon);
    SDL_DestroySurface(icon);

    // -- keyboard image --
    keyboardSurface = IMG_Load("assets/keyboard.png");
    if (keyboardSurface == NULL) {
        SDL_Log("Failed to load keyboardSurface: %s", SDL_GetError());
    }
    keyboard = SDL_CreateTextureFromSurface(renderer, keyboardSurface);
    if (keyboard == NULL) {
        SDL_Log("Failed to load keyboard: %s", SDL_GetError());
    }

    SDL_GetTextureSize(keyboard, &keyW, &keyH);

    int intKeyH = (int)keyH, intKeyW = (int)keyW;

    int keyX = (width >> 1) - (intKeyW >> 1);
    int keyY = (height >> 1) - (intKeyH >> 1);

    SDL_FRect temp = {keyX, keyY-5, intKeyW, intKeyH};
    keyRect = temp;

    // -- fonts -- 
    globalFont = TTF_OpenFont("assets\\NES.ttf", 25);
    if (globalFont == NULL) {
        SDL_Log("Failed to load font globalFont: %s", SDL_GetError());
    }

    globalFontS = TTF_OpenFont("assets\\NES.ttf", 17);
    if (globalFont == NULL) {
        SDL_Log("Failed to load font globalFontS: %s", SDL_GetError());
    }

    setupStaticText();

    // -- texture setup --
    boardTexture = SDL_CreateTexture(
        renderer,
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
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        TETROMINO_BLOCK_SIZE << 2,
        (TETROMINO_BLOCK_SIZE << 3) + (TETROMINO_BLOCK_SIZE << 2)
    );
    if (!nextTexture) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    backgroundKeyboard  = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        intKeyW, intKeyH
    );
    if (!backgroundKeyboard) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    keyboardText  = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        intKeyW, 100
    );
    if (!backgroundKeyboard) {
        printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    }

    SDL_FRect temp2 = {keyX, keyY+intKeyH+5, intKeyW, 100};
    keyboardTextRect = temp2;

    // -- next blocks -- 
    for (int k = 0; k < 4; k++) {
        nextBlocks[k] = SDL_rand(7);
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
    if (currentBlock == 0) {
        N = 4;
    } else if (currentBlock == 3) {
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
    if (currentBlock == 0) {
        N = 4;
    } else if (currentBlock == 3) {
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

int heldtet = -1;

void resetGame() {
    resetBoard();
    for (int i = 0; i < 7; i++) {
        scoreString[i] = '0';
        if (i < 4) {
            nextBlocks[i] = SDL_rand(7);
        }
    }
    score = 0;
    heldtet = -1;
    currentBlock = 0;
    currentTet.x = 4;
    currentTet.y = 1;
    winning = true;
    firstRun = true;
}

void runWireframes(tetromino *copyTet) {
    if (showWireframe) {
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

void addKeyboardRects(int x, int y, int w, int h, bool down) {
    SDL_SetRenderTarget(renderer, backgroundKeyboard);

    SDL_FRect rect = {
        x * KEYBOARD_PIXEL_SIZE,
        y * KEYBOARD_PIXEL_SIZE, //x and y
        w * KEYBOARD_PIXEL_SIZE, 
        h * KEYBOARD_PIXEL_SIZE, //w and h
    };

    if (down) {
        SDL_SetRenderDrawColor(renderer, 255, 53, 38, SDL_ALPHA_OPAQUE); 
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
    }

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderTarget(renderer, NULL);
}

Uint8 amountPressedDown = 0;

void handleKeyboardInput(SDL_Scancode code) {
    switch (code) {
        case SDL_SCANCODE_LEFT: {
            if (canMove(&currentTet, -1, 0)) {
                startSound(&sfx[MOVELEFT]);
                currentTet.x -= 1;
                runWireframes(&currentTet);
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (canMove(&currentTet, 1, 0)) {
                startSound(&sfx[MOVERIGHT]);
                currentTet.x += 1;
                runWireframes(&currentTet);
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (canMove(&currentTet, 0, 1)) {
                currentTet.y += 1;
            }
            break;
        }
        case SDL_SCANCODE_D: {
            rotateTetrominoCW(&currentTet);
            runWireframes(&currentTet);
            break;
        }
        case SDL_SCANCODE_A: {
            rotateTetrominoCCW(&currentTet);
            runWireframes(&currentTet);
            break;
        }
        case SDL_SCANCODE_R: {
            if (winning) {
                restartMainTheme();
                runWireframes(&currentTet);
                resetGame();
                startSound(&sfx[OPEN]);
            }
            break;
        }
        case SDL_SCANCODE_H: {
            startSound(&sfx[HOLYMOLY]);
            break;
        }
        case SDL_SCANCODE_SPACE: {
            if (titleCard) {
                restartMainTheme();
                startSound(&sfx[OPEN]);
                titleCard = false;
                keyboardCard = true;
                break;
            } else if (keyboardCard) {
                restartMainTheme();
                startSound(&sfx[OPEN]);
                keyboardCard = false;
                winning = true;
                break;
            } else if (winning) {
                startSound(&sfx[SWITCH]);
                //swap values
                int temp = heldtet;
                heldtet = currentBlock;
                currentBlock = temp;

                if (currentBlock == -1) { //if starting
                    currentBlock = nextBlocks[0];
                    for (int n = 0; n < 3; n++) {
                        nextBlocks[n] = nextBlocks[n+1];
                    }
                    nextBlocks[3] = (int)SDL_rand(7); 
                }
                currentTet = tetArray[currentBlock];
                currentTet.x = 4;
                currentTet.y = 1;
                updateNextBlocks();
                runWireframes(&currentTet);
                break;
            } else {
                startSound(&sfx[OPEN]);
                restartMainTheme();
                resetGame();
                break;
            }
            break;
        }
        case SDL_SCANCODE_LALT: {
            showWireframe = !showWireframe;
            runWireframes(&currentTet);
            buildBoardTexture();
            break;
        }
        case SDL_SCANCODE_RALT: {
            showWireframe = !showWireframe;
            runWireframes(&currentTet);
            buildBoardTexture();
            break;
        }
    }
}

Uint8 amountPressed = 0;

textTexture textArray[300];

void writeToKeyboardText(char mainStr[], char subStr[], bool write, SDL_Scancode scancode) {
    SDL_SetRenderTarget(renderer, keyboardText);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (write) {
        //create surface from string and draw it to the main texture at the centre Y
        if (textArray[scancode].tex == NULL) {
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface *textSurface = TTF_RenderText_Blended(globalFont, mainStr, strlen(mainStr), textColor);
            if (!textSurface) {
                SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
                SDL_SetRenderTarget(renderer, NULL);
                return;
            }
            if (subStr[0] != '\0') {
                SDL_Surface *subTextSurface = TTF_RenderText_Blended(globalFontS, subStr, strlen(subStr), textColor);
                SDL_Surface *combined = SDL_CreateSurface(
                    max(subTextSurface->w, textSurface->w), 
                    textSurface->h + subTextSurface->h + 10, 
                    SDL_PIXELFORMAT_RGBA32
                );

                SDL_Rect mainDest = {(combined->w - textSurface->w) / 2, 0, textSurface->w, textSurface->h};
                SDL_Rect subDest = {(combined->w - subTextSurface->w) / 2, textSurface->h + 10, subTextSurface->w, subTextSurface->h};

                SDL_BlitSurface(textSurface, NULL, combined, &mainDest);
                SDL_BlitSurface(subTextSurface, NULL, combined, &subDest);

                textArray[scancode].dest.w = combined->w;
                textArray[scancode].dest.h = combined->h;
                textArray[scancode].dest.x = (840 - textArray[scancode].dest.w) / 2;
                textArray[scancode].dest.y = 0;

                textArray[scancode].tex = SDL_CreateTextureFromSurface(renderer, combined);

                if (!textArray[scancode].tex) {
                    SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
                    SDL_SetRenderTarget(renderer, NULL);
                    return;
                }
            } else {
                textArray[scancode].dest.w = textSurface->w;
                textArray[scancode].dest.h = textSurface->h;
                textArray[scancode].dest.x = (840 - textArray[scancode].dest.w) / 2;
                textArray[scancode].dest.y = 0;
                
                textArray[scancode].tex = SDL_CreateTextureFromSurface(renderer, textSurface);

                if (!textArray[scancode].tex) {
                    SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
                    SDL_SetRenderTarget(renderer, NULL);
                    return;
                }
            }
            
            SDL_RenderTexture(renderer, textArray[scancode].tex, NULL, &textArray[scancode].dest);
            SDL_DestroySurface(textSurface); 
        } else {
            SDL_RenderTexture(renderer, textArray[scancode].tex, NULL, &textArray[scancode].dest);
        }

    }

    SDL_SetRenderTarget(renderer, NULL);
}

void handleInputKeyboardCard(SDL_Scancode code, bool pressing) {
    switch (code) {
        // -- letters --
        case SDL_SCANCODE_Q: {
            writeToKeyboardText("-Q-", "", pressing, SDL_SCANCODE_Q);
            addKeyboardRects(7, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_W: {
            writeToKeyboardText("-W-", "", pressing, SDL_SCANCODE_W);
            addKeyboardRects(13, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_E: {
            writeToKeyboardText("-E-", "", pressing, SDL_SCANCODE_E);
            addKeyboardRects(19, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_R: {
            writeToKeyboardText("-R-", "", pressing, SDL_SCANCODE_R);
            addKeyboardRects(25, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_T: {
            writeToKeyboardText("-T-", "", pressing, SDL_SCANCODE_T);
            addKeyboardRects(31, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_Y: {
            writeToKeyboardText("-Y-", "", pressing, SDL_SCANCODE_Y);
            addKeyboardRects(37, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_U: {
            writeToKeyboardText("-U-", "", pressing, SDL_SCANCODE_U);
            addKeyboardRects(43, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_I: {
            writeToKeyboardText("-I-", "", pressing, SDL_SCANCODE_I);
            addKeyboardRects(49, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_O: {
            writeToKeyboardText("-O-", "", pressing, SDL_SCANCODE_O);
            addKeyboardRects(55, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_P: { //TODO: add pause function
            writeToKeyboardText("-P-", "", pressing, SDL_SCANCODE_P);
            addKeyboardRects(61, 9, 5, 7, pressing);
            break;
        }

        case SDL_SCANCODE_A: {
            writeToKeyboardText("-A-", "Tap to spin block counter-clockwise", pressing, SDL_SCANCODE_A);
            addKeyboardRects(9, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_S: {
            writeToKeyboardText("-S-", "", pressing, SDL_SCANCODE_S);
            addKeyboardRects(15, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_D: {
            writeToKeyboardText("-D-", "Tap to spin block clockwise", pressing, SDL_SCANCODE_D);
            addKeyboardRects(21, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_F: {
            writeToKeyboardText("-F-", "", pressing, SDL_SCANCODE_F);
            addKeyboardRects(27, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_G: {
            writeToKeyboardText("-G-", "", pressing, SDL_SCANCODE_G);
            addKeyboardRects(33, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_H: {
            writeToKeyboardText("-H-", "Its a surprise :)", pressing, SDL_SCANCODE_H);
            addKeyboardRects(39, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_J: {
            writeToKeyboardText("-J-", "", pressing, SDL_SCANCODE_J);
            addKeyboardRects(45, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_K: {
            writeToKeyboardText("-K-", "", pressing, SDL_SCANCODE_K);
            addKeyboardRects(51, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_L: {
            writeToKeyboardText("-L-", "", pressing, SDL_SCANCODE_L);
            addKeyboardRects(57, 15, 5, 7, pressing);
            break;
        }

        case SDL_SCANCODE_Z: {
            writeToKeyboardText("-Z-", "", pressing, SDL_SCANCODE_Z);
            addKeyboardRects(12, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_X: {
            writeToKeyboardText("-X-", "", pressing, SDL_SCANCODE_X);
            addKeyboardRects(18, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_C: {
            writeToKeyboardText("-C-", "", pressing, SDL_SCANCODE_C);
            addKeyboardRects(24, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_V: {
            writeToKeyboardText("-V-", "", pressing, SDL_SCANCODE_V);
            addKeyboardRects(30, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_B: {
            writeToKeyboardText("-B-", "", pressing, SDL_SCANCODE_B);
            addKeyboardRects(36, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_N: {
            writeToKeyboardText("-N-", "", pressing, SDL_SCANCODE_N);
            addKeyboardRects(42, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_M: {
            writeToKeyboardText("-M-", "", pressing, SDL_SCANCODE_M);
            addKeyboardRects(48, 21, 5, 7, pressing);
            break;
        }

        // -- numbers --
        case SDL_SCANCODE_1: {  
            writeToKeyboardText("-1-", "", pressing, SDL_SCANCODE_1);
            addKeyboardRects(5, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_2: {
            writeToKeyboardText("-2-", "", pressing, SDL_SCANCODE_2);
            addKeyboardRects(11, 3, 5, 7, pressing);
            break;  
        }
        case SDL_SCANCODE_3: {
            writeToKeyboardText("-3-", "", pressing, SDL_SCANCODE_3);
            addKeyboardRects(17, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_4: {
            writeToKeyboardText("-4-", "", pressing, SDL_SCANCODE_4);
            addKeyboardRects(23, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_5: {
            writeToKeyboardText("-5-", "", pressing, SDL_SCANCODE_5);
            addKeyboardRects(29, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_6: {
            writeToKeyboardText("-6-", "", pressing, SDL_SCANCODE_6);
            addKeyboardRects(35, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_7: {
            writeToKeyboardText("-7-", "", pressing, SDL_SCANCODE_7);            
            addKeyboardRects(41, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_8: {
            writeToKeyboardText("-8-", "", pressing, SDL_SCANCODE_8);
            addKeyboardRects(47, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_9: {
            writeToKeyboardText("-9-", "", pressing, SDL_SCANCODE_9);
            addKeyboardRects(53, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_0: {
            writeToKeyboardText("-0-", "", pressing, SDL_SCANCODE_0);
            addKeyboardRects(59, 3, 5, 7, pressing);
            break;
        }
        
        // -- top row --
        case SDL_SCANCODE_ESCAPE: {
            writeToKeyboardText("-Esc-", "", pressing, SDL_SCANCODE_ESCAPE);
            addKeyboardRects(1, 1, 6, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F1: {
            writeToKeyboardText("-F1-", "", pressing, SDL_SCANCODE_F1);
            addKeyboardRects(8, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F2: {
            writeToKeyboardText("-F2-", "", pressing, SDL_SCANCODE_F2);
            addKeyboardRects(13, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F3: {
            writeToKeyboardText("-F3-", "", pressing, SDL_SCANCODE_F3);
            addKeyboardRects(19, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F4: {
            writeToKeyboardText("-F4-", "", pressing, SDL_SCANCODE_F4);
            addKeyboardRects(24, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F5: {
            writeToKeyboardText("-F5-", "", pressing, SDL_SCANCODE_F5);
            addKeyboardRects(30, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F6: {
            writeToKeyboardText("-F6-", "", pressing, SDL_SCANCODE_F6);
            addKeyboardRects(35, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F7: {
            writeToKeyboardText("-F7-", "", pressing, SDL_SCANCODE_F7);
            addKeyboardRects(41, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F8: {
            writeToKeyboardText("-F8-", "", pressing, SDL_SCANCODE_F8);
            addKeyboardRects(46, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F9: {
            writeToKeyboardText("-F9-", "", pressing, SDL_SCANCODE_F9);
            addKeyboardRects(52, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F10: {
            writeToKeyboardText("-F10-", "", pressing, SDL_SCANCODE_F10);
            addKeyboardRects(57, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F11: {
            writeToKeyboardText("-F11-", "", pressing, SDL_SCANCODE_F11);
            addKeyboardRects(63, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F12: {
            writeToKeyboardText("-F12-", "", pressing, SDL_SCANCODE_F12);
            addKeyboardRects(68, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_DELETE: {
            writeToKeyboardText("-Del-", "", pressing, SDL_SCANCODE_DELETE);
            addKeyboardRects(79, 1, 6, 3, pressing);
            break;
        }

        // --  bottom row --
        case SDL_SCANCODE_LCTRL: {
            writeToKeyboardText("-L Ctrl-", "", pressing, SDL_SCANCODE_LCTRL);
            addKeyboardRects(1, 27, 6, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LGUI: {
            writeToKeyboardText("-Wnd-", "", pressing, SDL_SCANCODE_LGUI);
            addKeyboardRects(14, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LALT: {
            writeToKeyboardText("-L Alt-", "Tap to show/hide wireframes", pressing, SDL_SCANCODE_LALT);
            addKeyboardRects(20, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_SPACE: {
            writeToKeyboardText("-Space-", "Tap to swap blocks, double tap to start game", pressing, SDL_SCANCODE_SPACE);
            addKeyboardRects(26, 27, 29, 6, pressing);

            if (!pressing) {
                amountPressed += 1;
            }
            break;
        }
        case SDL_SCANCODE_RALT: {
            writeToKeyboardText("-R Alt-", "Tap to show/hide wireframes", pressing, SDL_SCANCODE_RALT);
            addKeyboardRects(56, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_APPLICATION: {
            writeToKeyboardText("-App-", "", pressing, SDL_SCANCODE_APPLICATION);
            addKeyboardRects(62, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LEFT: {
            writeToKeyboardText("-Left-", "Tap to move block left", pressing, SDL_SCANCODE_LEFT);
            addKeyboardRects(68, 28, 6, 5, pressing);
            break;
        }
        case SDL_SCANCODE_UP: {
            writeToKeyboardText("-Up-", "", pressing, SDL_SCANCODE_UP);
            addKeyboardRects(74, 27, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_DOWN: {
            writeToKeyboardText("-Down-", "Tap to move block down, double tap for auto place", pressing, SDL_SCANCODE_DOWN);
            addKeyboardRects(74, 30, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            writeToKeyboardText("-Right-", "Tap to move block right", pressing, SDL_SCANCODE_RIGHT);
            addKeyboardRects(79, 28, 6, 5, pressing);
            break;
        }

        // -- left side --
        case SDL_SCANCODE_GRAVE: {
            writeToKeyboardText("-Grave-", "", pressing, SDL_SCANCODE_GRAVE);
            addKeyboardRects(1, 4, 3, 5, pressing);
            break;
        }
        case SDL_SCANCODE_TAB: {
             writeToKeyboardText("-Tab-", "", pressing, SDL_SCANCODE_TAB);
            addKeyboardRects(1, 9, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_CAPSLOCK: {
             writeToKeyboardText("-Caps-", "", pressing, SDL_SCANCODE_CAPSLOCK);
            addKeyboardRects(1, 15, 7, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LSHIFT: {
             writeToKeyboardText("-L Shift-", "", pressing, SDL_SCANCODE_LSHIFT);
            addKeyboardRects(1, 21, 10, 7, pressing);
            break;
        }

        // -- right side --
        case SDL_SCANCODE_MINUS: {
            writeToKeyboardText("-Minus-", "", pressing, SDL_SCANCODE_MINUS);
            addKeyboardRects(65, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_EQUALS: {
            writeToKeyboardText("-Equals-", "", pressing, SDL_SCANCODE_EQUALS);
            addKeyboardRects(71, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_BACKSPACE: {
            writeToKeyboardText("-Backspace-", "", pressing, SDL_SCANCODE_BACKSPACE);
            addKeyboardRects(77, 3, 8, 7, pressing);
            break;
        }

        case SDL_SCANCODE_LEFTBRACKET: { //six-seven???
            writeToKeyboardText("-L Bracket-", "", pressing, SDL_SCANCODE_LEFTBRACKET);
            addKeyboardRects(67, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RIGHTBRACKET: {
            writeToKeyboardText("-R Bracket-", "", pressing, SDL_SCANCODE_RIGHTBRACKET);
            addKeyboardRects(73, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_BACKSLASH: {
            writeToKeyboardText("-Backslash-", "", pressing, SDL_SCANCODE_BACKSLASH);
            addKeyboardRects(79, 9, 6, 7, pressing);
            break;
        }

        case SDL_SCANCODE_SEMICOLON: {
            writeToKeyboardText("-Semicolon-", "", pressing, SDL_SCANCODE_SEMICOLON);
            addKeyboardRects(63, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_APOSTROPHE: { //six-nine???
            writeToKeyboardText("-Apostrophe-", "", pressing, SDL_SCANCODE_APOSTROPHE);
            addKeyboardRects(69, 15, 6, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RETURN: {
            writeToKeyboardText("-Enter-", "", pressing, SDL_SCANCODE_RETURN);
            addKeyboardRects(76, 15, 9, 7, pressing);
            break;
        }
    
        case SDL_SCANCODE_COMMA: {
            writeToKeyboardText("-Comma-", "", pressing, SDL_SCANCODE_COMMA);
            addKeyboardRects(54, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_PERIOD: {
            writeToKeyboardText("-Period-", "", pressing, SDL_SCANCODE_PERIOD);
            addKeyboardRects(60, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_SLASH: {
            writeToKeyboardText("-Slash-", "", pressing, SDL_SCANCODE_SLASH);
            addKeyboardRects(66, 21, 6, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RSHIFT: {
            writeToKeyboardText("-R Shift-", "", pressing, SDL_SCANCODE_RSHIFT);
            addKeyboardRects(73, 21, 12, 7, pressing);
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
            if (keyboardCard) {
                handleInputKeyboardCard(event->key.scancode, true);
            } else {
                handleKeyboardInput(event->key.scancode);
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            if (keyboardCard) {
                handleInputKeyboardCard(event->key.scancode, false);
            } 
            if (event->key.scancode == SDL_SCANCODE_DOWN) {
                amountPressedDown++;
            }
        }
        case SDL_EVENT_WINDOW_RESIZED: {
            SDL_GetWindowSize(window, &width, &height);
            bWidthMin = ((width / TETROMINO_BLOCK_SIZE) >> 1) - 5;
            bWidthMax = ((width / TETROMINO_BLOCK_SIZE) >> 1) + 5;

            bHeightMin = ((height / TETROMINO_BLOCK_SIZE) >> 1) - 10;
            bHeightMax = ((height / TETROMINO_BLOCK_SIZE) >> 1) + 10;
            setupStaticText();

            keyRect.x = (width >> 1) - ((int)keyW/2);
            keyRect.y = (height >> 1) - ((int)keyH/2) - 5;

            keyboardTextRect.x = keyRect.x;
            keyboardTextRect.y = keyRect.y + keyRect.h + 10;

            halfTitleWidth = (width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
            halfTitleHeight = ((height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));
            break;
        }
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void moveBoardDown(int remove) {
    //start at the index and set each to the one above
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

    //set if set to centre
    if (x == -1) {
        x = (width >> 1) - (textW/2);
    }
    if (y == -1) {
        y = (height >> 1) - (textH/2);
    }

    SDL_FRect textRect = {x, y, textW, textH};
    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

bool displayShift = true;

void setupStaticText() {
    //board sizes 
    int widMinT = bWidthMin*TETROMINO_BLOCK_SIZE, widMaxT = bWidthMax*TETROMINO_BLOCK_SIZE;
    int higMinT = bHeightMin*TETROMINO_BLOCK_SIZE, higMaxT = bHeightMax*TETROMINO_BLOCK_SIZE;

    // Render text to a surface
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Blended(globalFont, "Tetris Time!", 12, textColor);
    SDL_Surface* surface2  = TTF_RenderText_Blended(globalFont, "-Held-", 6, textColor);
    SDL_Surface* surface3  = TTF_RenderText_Blended(globalFont, "-Score-", 7, textColor);
    SDL_Surface* surface4  = TTF_RenderText_Blended(globalFont, "-Next Blocks-", 13, textColor);
    if (!surface1 || !surface2 || !surface3) {
        SDL_Log("TTF_RenderText_Blended Error: %s", SDL_GetError());
        return;
    }

    int textHeight = surface1->h;

    SDL_Surface *combined = SDL_CreateSurface(widMaxT+60+surface4->w, higMinT+(textHeight*3)+60, SDL_PIXELFORMAT_RGBA32);

    //setup destination rects
    SDL_Rect dest1 = {(width >> 1) - (surface1->w >> 1) + 15, higMinT>>1, surface1->w, textHeight};
    SDL_Rect dest2 = {widMinT-180, higMinT+20, surface2->w, textHeight};
    SDL_Rect dest3 = {widMaxT+60, higMinT+20, surface3->w, textHeight};
    SDL_Rect dest4 = {widMaxT+60, higMinT+(textHeight << 1)+60, surface4->w, textHeight};

    //copy to combined
    SDL_BlitSurface(surface1, NULL, combined, &dest1);
    SDL_BlitSurface(surface2, NULL, combined, &dest2);
    SDL_BlitSurface(surface3, NULL, combined, &dest3);
    SDL_BlitSurface(surface4, NULL, combined, &dest4);

    // Convert surface to texture
    staticText = SDL_CreateTextureFromSurface(renderer, combined);
    if (!staticText) {
        SDL_Log("SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return;
    }

    SDL_GetTextureSize(staticText, &textW, &textH);

    //destroy
    SDL_DestroySurface(surface1);
    SDL_DestroySurface(surface2);
    SDL_DestroySurface(surface3);
    SDL_DestroySurface(surface4);
    SDL_DestroySurface(combined);
}

void displayStaticTexture() {
    // Get text dimensions
    SDL_FRect textRect = {0, 0, textW, textH};
    SDL_RenderTexture(renderer, staticText, NULL, &textRect);
}

int posX = 0, posY = 0;
int currentMove = 0, currentColour = 1;
char* finalScore = NULL;

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    Uint64 now = SDL_GetTicks();
    static Uint64 lastFallTime = 0, lastChange = 0, lastPress = 0, lastPressDown = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int ii = 0; ii < SDL_arraysize(sfx); ii++) {
        if (sfx[ii].playing == true) {
            playWAV(&sfx[ii], false);
        }
    }
    playWAV(&mainTheme, true);

    if (titleCard) {
        //change through colours
        if (now - lastChange >= 500) {
            if (currentMove == 6) {
                currentMove = 0;
                currentColour++;
                if (currentColour == 7) currentColour = 0;
            }

            titleTetroes[currentMove].r = colour[currentColour].r;
            titleTetroes[currentMove].g = colour[currentColour].g;
            titleTetroes[currentMove].b = colour[currentColour].b;

            currentMove++;
            lastChange = now;
        }
        //text
        displayText("Tribute by LKerr42", -1, (height >> 1) + (TETROMINO_BLOCK_SIZE << 1), globalFont, 255, 255, 255);
        displayText("Press Space to start", -1, 10, globalFont, 255, 255, 255);

        //display blocks
        for (int countB = 0; countB < 6; countB++) {
            for (int Y = 0; Y < 5; Y++) {
                for (int X = 0; X < 4; X++) {
                    if (titleTetroes[countB].titleBlocks[Y][X].active == true) {
                        posX = titleTetroes[countB].x + (X * TETROMINO_BLOCK_SIZE) + halfTitleWidth;
                        posY = titleTetroes[countB].y + (Y* TETROMINO_BLOCK_SIZE) + halfTitleHeight;

                        SDL_FRect Trect = {
                            posX,
                            posY - (TETROMINO_BLOCK_SIZE << 1),
                            TETROMINO_BLOCK_SIZE,
                            TETROMINO_BLOCK_SIZE
                        };
                        displayBlock(Trect, titleTetroes[countB].r, titleTetroes[countB].g, titleTetroes[countB].b);
                    }
                }
            }
        }
    } else if (keyboardCard) {
        SDL_RenderTexture(renderer, keyboardText, NULL, &keyboardTextRect);
        SDL_RenderTexture(renderer, backgroundKeyboard, NULL, &keyRect);
        SDL_RenderTexture(renderer, keyboard, NULL, &keyRect);
        if (amountPressed == 1) {
            lastPress++;
        } else if (amountPressed == 2) {
            restartMainTheme();
            startSound(&sfx[OPEN]);
            keyboardCard = false;
            winning = true;
            amountPressed = 0;
        }

        if (lastPress >= 500) {
            amountPressed = 0;
            lastPress = 0;
        }
    } else if (winning) { 
        displayStaticTexture();
        //For some reason the firstBlocks int array corrupts between the start of this and the end of space being pressed
        if (firstRun == true) {
            updateNextBlocks(); 
            firstRun = false;
            currentTet = tetArray[currentBlock];
            runWireframes(&currentTet);
        }
        displayText(scoreString, (bWidthMax*TETROMINO_BLOCK_SIZE)+60, (bHeightMin*TETROMINO_BLOCK_SIZE)+49, globalFont, 255, 255, 255);
        int countBlocks = 0, linesCleared = 0;
        char incompleteScore[7];

        if (amountPressedDown == 1) {
            lastPressDown++;
        } else if (amountPressedDown == 2) {
            while (true) {
                if (canMove(&currentTet, 0, 1)) {
                    currentTet.y += 1;
                } else {
                    break;
                }
            }
            amountPressedDown = 0;
            lastPressDown = 0;
        }

        if (lastPressDown >= 500) {
            amountPressedDown = 0;
            lastPressDown = 0;
        }

        if (now - lastFallTime >= 1000) {
            //check if any are above a set block
            if (canMove(&currentTet, 0, 1)) {
                currentTet.y += 1;
            } else {
                startSound(&sfx[LAND]);
                //set block
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        if (currentTet.blocks[k][l].active == true) {
                            posX = currentTet.x+currentTet.blocks[k][l].x;
                            posY = currentTet.y+currentTet.blocks[k][l].y;

                            filledBlocks[posY][posX].v = true; 
                            filledBlocks[posY][posX].r = currentTet.blocks[k][l].r; 
                            filledBlocks[posY][posX].g = currentTet.blocks[k][l].g;
                            filledBlocks[posY][posX].b = currentTet.blocks[k][l].b;
                        }
                    }
                }

                //reset block
                currentBlock = nextBlocks[0];
                for (int n = 0; n < 3; n++) {
                    nextBlocks[n] = nextBlocks[n+1];
                }
                nextBlocks[3] = (int)SDL_rand(7); 
                currentTet = tetArray[currentBlock];
                currentTet.x = 4;
                currentTet.y = 1;
                
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

                runWireframes(&currentTet);

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
                        winning = false;
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
                if (currentTet.blocks[y][x].active) {
                    posX = currentTet.x + x;
                    posY = currentTet.y + y;
                    SDL_FRect Brect = {
                        (posX + bWidthMin) * TETROMINO_BLOCK_SIZE,
                        (posY + bHeightMin) * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlock(Brect, currentTet.r, currentTet.g, currentTet.b);
                }
            }
        }

        int baseX = (bWidthMin*TETROMINO_BLOCK_SIZE)-180 + (TETROMINO_BLOCK_SIZE << 1);
        int baseY = (bHeightMin*TETROMINO_BLOCK_SIZE)+70;

        //display held tetromino
        if (heldtet != -1) {
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    if (shapes[heldtet][k][l] == 1) {
                        SDL_FRect Hrect = {
                            l * TETROMINO_BLOCK_SIZE + baseX,
                            k * TETROMINO_BLOCK_SIZE + baseY,
                            TETROMINO_BLOCK_SIZE,
                            TETROMINO_BLOCK_SIZE
                        };
                        displayBlock(Hrect, tetArray[heldtet].r, tetArray[heldtet].g, tetArray[heldtet].b);
                    }
                }
            } 
        }

    } else if (titleCard == false) { //lose card
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
        SDL_RenderClear(renderer);
        bool displayed = false;
        displayText("You Lost!", -1, -1, globalFont, 255, 255, 255);
        displayText(finalScore, -1, (height >> 1) + 50, globalFontS, 255, 255, 255);
        displayText("Press space to start again", -1, (height >> 1) + 100, globalFontS, 255, 255, 255);
    }

    SDL_RenderPresent(renderer); // render everything

    return SDL_APP_CONTINUE;  // carry on with the program!
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(staticText);
    SDL_DestroyTexture(keyboard);
    SDL_DestroyTexture(keyboardText);
    TTF_CloseFont(globalFont);
    TTF_CloseFont(globalFontS);
    TTF_Quit();
    for (int i = 0; i < SDL_arraysize(sfx); i++) {
        if (sfx[i].stream) {
            SDL_DestroyAudioStream(sfx[i].stream);
        }
        SDL_free(sfx[i].audio_buff);
    }
    SDL_DestroyAudioStream(mainTheme.stream);
    SDL_free(mainTheme.audio_buff);
}