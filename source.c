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

SDL_Texture *staticText;

int width = 1000, height = 750;

// Blocks
int bWidthMin, bWidthMax, bHeightMin, bHeightMax; 
int currentBlock = 0, score = 0;
float textW, textH;

bool falling = true, winning = false, titleCard = true;
char scoreString[7] = "0000000";

TTF_Font* globalFont;
TTF_Font* globalFontS;

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
    blockStruct titleBlocks[5][4];
    int r;
    int g;
    int b;
    int x;
    int y;
} tetromino;
tetromino tetArray[7];
tetromino titleTetroes[6];

typedef struct {
    int r;
    int g;
    int b;
} colours;
colours colour[7];

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

void setupStaticText();

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

SDL_Texture* boardTexture;

void displayBlock(SDL_FRect rect, int r, int g, int b, bool drawTexture) {
    int i, j, cX = 0, cY = 0;
    int minX = rect.x, maxX = rect.w+rect.x, minY = rect.y, maxY = rect.h+rect.y;
    
    int cR = SDL_clamp(r-45, 0, 255);
    int cG = SDL_clamp(g-45, 0, 255);
    int cB = SDL_clamp(b-45, 0, 255);

    int bR = SDL_clamp(r-90, 0, 255);
    int bG = SDL_clamp(g-90, 0, 255);
    int bB = SDL_clamp(b-90, 0, 255);

    if (drawTexture) {
        SDL_SetRenderTarget(renderer, boardTexture);
    }

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

    if (drawTexture) {
        SDL_SetRenderTarget(renderer, NULL);
    }
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

                displayBlock(rect,
                             filledBlocks[i][j].r,
                             filledBlocks[i][j].g,
                             filledBlocks[i][j].b,
                             true);   // draw to texture
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL); // back to screen
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

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    bWidthMin = ((width / TETROMINO_BLOCK_SIZE) >> 1) - 5;
    bWidthMax = ((width / TETROMINO_BLOCK_SIZE) >> 1) + 5;

    bHeightMin = ((height / TETROMINO_BLOCK_SIZE) >> 1) - 10;
    bHeightMax = ((height / TETROMINO_BLOCK_SIZE) >> 1) + 10;

    setupTetrominos();
    setupTitleBlocks();

    setColourDef(&colour[0], 0, 0, 255); //blue
    setColourDef(&colour[1], 0, 255, 255); //turquoise
    setColourDef(&colour[2], 0, 255, 0); //green
    setColourDef(&colour[3], 255, 255, 0); //yellow
    setColourDef(&colour[4], 255, 160, 0); //orange
    setColourDef(&colour[5], 255, 0, 0); //red
    setColourDef(&colour[6], 150, 0, 255); //purple

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

    SDL_SetAppMetadata("Play Tetis!", "0.5.0", "com/LKerr42/SDL-Tetris.github");

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

    globalFont = TTF_OpenFont("assets\\NES.ttf", 25);
    if (globalFont == NULL) {
        SDL_Log("Failed to load font globalFont: %s", SDL_GetError());
    }

    globalFontS = TTF_OpenFont("assets\\NES.ttf", 17);
    if (globalFont == NULL) {
        SDL_Log("Failed to load font globalFontS: %s", SDL_GetError());
    }

    setupStaticText();

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

void rotateTetrominoCW(tetromino *t) {
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
            rotateTetrominoCCW(t);
        }
    }
}

void rotateTetrominoCCW(tetromino *t) {
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
            rotateTetrominoCW(t);
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
    renderBoard();
}

int heldtet = -1;

void resetGame() {
    resetBoard();
    for (int i = 0; i < 7; i++) {
        scoreString[i] = '0';
    }
    score = 0;
    heldtet = -1;
    currentBlock = 0;
    tetArray[currentBlock].x = 4;
    tetArray[currentBlock].y = 1;
    winning = true;
}

void handleKeyboardInput(SDL_Scancode event) {
    int indx, furth;
    switch (event) {
        case SDL_SCANCODE_LEFT: {
            if (canMove(&tetArray[currentBlock], -1, 0)) {
                tetArray[currentBlock].x -= 1;
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (canMove(&tetArray[currentBlock], 1, 0)) {
                tetArray[currentBlock].x += 1;
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (canMove(&tetArray[currentBlock], 0, 1)) {
                tetArray[currentBlock].y += 1;
            } else {
                // The block can’t move down anymore → it has landed
                falling = false;
                // Copy its active cells into filledBlocks
            }
            break;
        }
        case SDL_SCANCODE_D: {
            rotateTetrominoCW(&tetArray[currentBlock]);
            break;
        }
        case SDL_SCANCODE_A: {
            rotateTetrominoCCW(&tetArray[currentBlock]);
            break;
        }
        case SDL_SCANCODE_R: {
            if (winning) {
                resetGame();
            }
            break;
        }
        case SDL_SCANCODE_SPACE: {
            printf("Space pressed\n");
            if (winning) {
                //swap values
                int temp = heldtet;
                heldtet = currentBlock;
                currentBlock = temp;

                if (currentBlock == -1) { //if starting
                    currentBlock = SDL_rand(7);
                }
                tetArray[currentBlock].x = 4;
                tetArray[currentBlock].y = 1;
            } else {
                resetGame();
            }

            if (titleCard) {
                titleCard = false;
                winning = true;
                printf("heldtet before: %d", heldtet);
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
            bWidthMin = ((width / TETROMINO_BLOCK_SIZE) >> 1) - 5;
            bWidthMax = ((width / TETROMINO_BLOCK_SIZE) >> 1) + 5;

            bHeightMin = ((height / TETROMINO_BLOCK_SIZE) >> 1) - 10;
            bHeightMax = ((height / TETROMINO_BLOCK_SIZE) >> 1) + 10;
            setupStaticText();
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

bool toStop = false;
int posX = 0, posY = 0;
int currentMove = 0, currentColour = 1;
/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_FRect rects[20];

    Uint64 now = SDL_GetTicks();
    static Uint64 lastFallTime = 0, lastChange = 0;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    int halfTitleWidth = (width >> 1) - (25 * TETROMINO_BLOCK_SIZE >> 1);
    int halfTitleHeight = ((height >> 1) - (5 * TETROMINO_BLOCK_SIZE >> 1));

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
                        displayBlock(Trect, titleTetroes[countB].r, titleTetroes[countB].g, titleTetroes[countB].b, false);
                    }
                }
            }
        }
    }

    if (winning) {
        displayStaticTexture();
        displayText(scoreString, (bWidthMax*TETROMINO_BLOCK_SIZE)+60, (bHeightMin*TETROMINO_BLOCK_SIZE)+49, globalFont, 255, 255, 255);
        int countBlocks = 0, linesCleared = 0;
        char incompleteScore[7];

        if (now - lastFallTime >= 1000) {
            //check if any are above a set block
            for (int a = 0; a < 4; a++) {
                for (int b = 0; b < 4; b++) {
                    posX = tetArray[currentBlock].x+tetArray[currentBlock].blocks[a][b].x;
                    posY = tetArray[currentBlock].y+tetArray[currentBlock].blocks[a][b].y;
                    if (filledBlocks[posY+1][posX].v == true && tetArray[currentBlock].blocks[a][b].active == true) {
                        toStop = true;
                        break;
                    }
                }
                if (toStop == true) break;
            }
            if (toStop == true) {
                toStop = false;
                falling = false;
                //set block
                for (int k = 0; k < 4; k++) {
                    for (int l = 0; l < 4; l++) {
                        if (tetArray[currentBlock].blocks[k][l].active == true) {
                            posX = tetArray[currentBlock].x+tetArray[currentBlock].blocks[k][l].x;
                            posY = tetArray[currentBlock].y+tetArray[currentBlock].blocks[k][l].y;

                            filledBlocks[posY][posX].v = true; 
                            filledBlocks[posY][posX].r = tetArray[currentBlock].blocks[k][l].r; 
                            filledBlocks[posY][posX].g = tetArray[currentBlock].blocks[k][l].g;
                            filledBlocks[posY][posX].b = tetArray[currentBlock].blocks[k][l].b;
                        }
                    }
                }

                //reset block
                currentBlock = SDL_rand(7);
                falling = true;
                tetArray[currentBlock].x = 4;
                tetArray[currentBlock].y = 1;

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

                //update score
                if (linesCleared > 0) {
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
                        break;
                    }
                }
            } else {
                //move down
                if (toStop == false) {
                    tetArray[currentBlock].y += 1;
                }  
            }

            lastFallTime = now;
        }

        renderBoard();

        //display falling tetromino
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (tetArray[currentBlock].blocks[y][x].active) {
                    posX = tetArray[currentBlock].x + x;
                    posY = tetArray[currentBlock].y + y;
                    SDL_FRect Brect = {
                        (posX + bWidthMin) * TETROMINO_BLOCK_SIZE,
                        (posY + bHeightMin) * TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE,
                        TETROMINO_BLOCK_SIZE
                    };
                    displayBlock(Brect, tetArray[currentBlock].r, tetArray[currentBlock].g, tetArray[currentBlock].b, false);
                }
            }
        }

        //display held tetromino
        if (heldtet != -1) {
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 4; l++) {
                    if (shapes[heldtet][k][l] == 1) {
                        posX = l;
                        posY = k;
                        SDL_FRect Hrect = {
                            posX * TETROMINO_BLOCK_SIZE + (bWidthMin*TETROMINO_BLOCK_SIZE)-180 + (TETROMINO_BLOCK_SIZE << 1),
                            posY * TETROMINO_BLOCK_SIZE + (bHeightMin*TETROMINO_BLOCK_SIZE)+70,
                            TETROMINO_BLOCK_SIZE,
                            TETROMINO_BLOCK_SIZE
                        };
                        displayBlock(Hrect, tetArray[heldtet].r, tetArray[heldtet].g, tetArray[heldtet].b, false);
                    }
                }
            } 
        }

    } else if (titleCard == false) { //lose card
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black, full alpha
        SDL_RenderClear(renderer);
        bool displayed = false;
        displayText("You Lost!", -1, -1, globalFont, 255, 255, 255);
        displayText("Press space to start again", -1, (height >> 1) + 50, globalFontS, 255, 255, 255);
    }

    SDL_RenderPresent(renderer); // render everything

    return SDL_APP_CONTINUE;  // carry on with the program!
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /* SDL will clean up the window/renderer for us. */
    SDL_DestroyTexture(boardTexture);
    SDL_DestroyTexture(staticText);
    TTF_CloseFont(globalFont);
    TTF_Quit();
}
