#include "include/keyboard.h"
#include "include/text.h"
#include "include/app.h"
#include "include/tetromino.h"
#include "include/audio.h"
#include "include/renderer.h"
#include "include/game_control.h"
#include <stdio.h>

void addKeyboardRects(appContext *app, int x, int y, int w, int h, bool down) {
    SDL_SetRenderTarget(app->renderer, app->backgroundKeyboard);

    SDL_FRect rect = {
        x * KEYBOARD_PIXEL_SIZE,
        y * KEYBOARD_PIXEL_SIZE, //x and y
        w * KEYBOARD_PIXEL_SIZE, 
        h * KEYBOARD_PIXEL_SIZE, //w and h
    };

    if (down) {
        SDL_SetRenderDrawColor(app->renderer, 255, 53, 38, SDL_ALPHA_OPAQUE); 
    } else {
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); 
    }

    SDL_RenderFillRect(app->renderer, &rect);

    SDL_SetRenderTarget(app->renderer, NULL);
}

void handleInputKeyboardCard(appContext *app, SDL_Scancode code, bool pressing) {
    switch (code) {
        // -- letters --
        case SDL_SCANCODE_Q: {
            writeToKeyboardText(app, "-Q-", "", pressing, SDL_SCANCODE_Q);
            addKeyboardRects(app, 7, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_W: {
            writeToKeyboardText(app, "-W-", "", pressing, SDL_SCANCODE_W);
            addKeyboardRects(app, 13, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_E: {
            writeToKeyboardText(app, "-E-", "", pressing, SDL_SCANCODE_E);
            addKeyboardRects(app, 19, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_R: {
            writeToKeyboardText(app, "-R-", "Tap to reset the game", pressing, SDL_SCANCODE_R);
            addKeyboardRects(app, 25, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_T: {
            writeToKeyboardText(app, "-T-", "", pressing, SDL_SCANCODE_T);
            addKeyboardRects(app, 31, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_Y: {
            writeToKeyboardText(app, "-Y-", "", pressing, SDL_SCANCODE_Y);
            addKeyboardRects(app, 37, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_U: {
            writeToKeyboardText(app, "-U-", "", pressing, SDL_SCANCODE_U);
            addKeyboardRects(app, 43, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_I: {
            writeToKeyboardText(app, "-I-", "", pressing, SDL_SCANCODE_I);
            addKeyboardRects(app, 49, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_O: {
            writeToKeyboardText(app, "-O-", "", pressing, SDL_SCANCODE_O);
            addKeyboardRects(app, 55, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_P: {
            writeToKeyboardText(app, "-P-", "", pressing, SDL_SCANCODE_P);
            addKeyboardRects(app, 61, 9, 5, 7, pressing);
            break;
        }

        case SDL_SCANCODE_A: {
            writeToKeyboardText(app, "-A-", "Tap to spin block counter-clockwise", pressing, SDL_SCANCODE_A);
            addKeyboardRects(app, 9, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_S: {
            writeToKeyboardText(app, "-S-", "", pressing, SDL_SCANCODE_S);
            addKeyboardRects(app, 15, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_D: {
            writeToKeyboardText(app, "-D-", "Tap to spin block clockwise", pressing, SDL_SCANCODE_D);
            addKeyboardRects(app, 21, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_F: {
            writeToKeyboardText(app, "-F-", "", pressing, SDL_SCANCODE_F);
            addKeyboardRects(app, 27, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_G: {
            writeToKeyboardText(app, "-G-", "", pressing, SDL_SCANCODE_G);
            addKeyboardRects(app, 33, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_H: {
            writeToKeyboardText(app, "-H-", "Its a surprise :)", pressing, SDL_SCANCODE_H);
            addKeyboardRects(app, 39, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_J: {
            writeToKeyboardText(app, "-J-", "", pressing, SDL_SCANCODE_J);
            addKeyboardRects(app, 45, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_K: {
            writeToKeyboardText(app, "-K-", "", pressing, SDL_SCANCODE_K);
            addKeyboardRects(app, 51, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_L: {
            writeToKeyboardText(app, "-L-", "", pressing, SDL_SCANCODE_L);
            addKeyboardRects(app, 57, 15, 5, 7, pressing);
            break;
        }

        case SDL_SCANCODE_Z: {
            writeToKeyboardText(app, "-Z-", "", pressing, SDL_SCANCODE_Z);
            addKeyboardRects(app, 12, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_X: {
            writeToKeyboardText(app, "-X-", "", pressing, SDL_SCANCODE_X);
            addKeyboardRects(app, 18, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_C: {
            writeToKeyboardText(app, "-C-", "", pressing, SDL_SCANCODE_C);
            addKeyboardRects(app, 24, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_V: {
            writeToKeyboardText(app, "-V-", "", pressing, SDL_SCANCODE_V);
            addKeyboardRects(app, 30, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_B: {
            writeToKeyboardText(app, "-B-", "", pressing, SDL_SCANCODE_B);
            addKeyboardRects(app, 36, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_N: {
            writeToKeyboardText(app, "-N-", "", pressing, SDL_SCANCODE_N);
            addKeyboardRects(app, 42, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_M: {
            writeToKeyboardText(app, "-M-", "", pressing, SDL_SCANCODE_M);
            addKeyboardRects(app, 48, 21, 5, 7, pressing);
            break;
        }

        // -- numbers --
        case SDL_SCANCODE_1: {  
            writeToKeyboardText(app, "-1-", "", pressing, SDL_SCANCODE_1);
            addKeyboardRects(app, 5, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_2: {
            writeToKeyboardText(app, "-2-", "", pressing, SDL_SCANCODE_2);
            addKeyboardRects(app, 11, 3, 5, 7, pressing);
            break;  
        }
        case SDL_SCANCODE_3: {
            writeToKeyboardText(app, "-3-", "", pressing, SDL_SCANCODE_3);
            addKeyboardRects(app, 17, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_4: {
            writeToKeyboardText(app, "-4-", "", pressing, SDL_SCANCODE_4);
            addKeyboardRects(app, 23, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_5: {
            writeToKeyboardText(app, "-5-", "", pressing, SDL_SCANCODE_5);
            addKeyboardRects(app, 29, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_6: {
            writeToKeyboardText(app, "-6-", "", pressing, SDL_SCANCODE_6);
            addKeyboardRects(app, 35, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_7: {
            writeToKeyboardText(app, "-7-", "", pressing, SDL_SCANCODE_7);            
            addKeyboardRects(app, 41, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_8: {
            writeToKeyboardText(app, "-8-", "", pressing, SDL_SCANCODE_8);
            addKeyboardRects(app, 47, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_9: {
            writeToKeyboardText(app, "-9-", "", pressing, SDL_SCANCODE_9);
            addKeyboardRects(app, 53, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_0: {
            writeToKeyboardText(app, "-0-", "", pressing, SDL_SCANCODE_0);
            addKeyboardRects(app, 59, 3, 5, 7, pressing);
            break;
        }
        
        // -- top row --
        case SDL_SCANCODE_ESCAPE: {
            writeToKeyboardText(app, "-Esc-", "Tap pause/play game", pressing, SDL_SCANCODE_ESCAPE);
            addKeyboardRects(app, 1, 1, 6, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F1: {
            writeToKeyboardText(app, "-F1-", "Tap to show/hide controls", pressing, SDL_SCANCODE_F1);
            addKeyboardRects(app, 8, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F2: {
            writeToKeyboardText(app, "-F2-", "Tap to show/hide statistics", pressing, SDL_SCANCODE_F2);
            addKeyboardRects(app, 13, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F3: {
            writeToKeyboardText(app, "-F3-", "", pressing, SDL_SCANCODE_F3);
            addKeyboardRects(app, 19, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F4: {
            writeToKeyboardText(app, "-F4-", "", pressing, SDL_SCANCODE_F4);
            addKeyboardRects(app, 24, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F5: {
            writeToKeyboardText(app, "-F5-", "", pressing, SDL_SCANCODE_F5);
            addKeyboardRects(app, 30, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F6: {
            writeToKeyboardText(app, "-F6-", "", pressing, SDL_SCANCODE_F6);
            addKeyboardRects(app, 35, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F7: {
            writeToKeyboardText(app, "-F7-", "", pressing, SDL_SCANCODE_F7);
            addKeyboardRects(app, 41, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F8: {
            writeToKeyboardText(app, "-F8-", "", pressing, SDL_SCANCODE_F8);
            addKeyboardRects(app, 46, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F9: {
            writeToKeyboardText(app, "-F9-", "", pressing, SDL_SCANCODE_F9);
            addKeyboardRects(app, 52, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F10: {
            writeToKeyboardText(app, "-F10-", "", pressing, SDL_SCANCODE_F10);
            addKeyboardRects(app, 57, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F11: {
            writeToKeyboardText(app, "-F11-", "", pressing, SDL_SCANCODE_F11);
            addKeyboardRects(app, 63, 1, 4, 3, pressing);
            break;
        }
        case SDL_SCANCODE_F12: {
            writeToKeyboardText(app, "-F12-", "", pressing, SDL_SCANCODE_F12);
            addKeyboardRects(app, 68, 1, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_DELETE: {
            writeToKeyboardText(app, "-Del-", "Tap to exit app", pressing, SDL_SCANCODE_DELETE);
            addKeyboardRects(app, 79, 1, 6, 3, pressing);
            break;
        }

        // --  bottom row --
        case SDL_SCANCODE_LCTRL: {
            writeToKeyboardText(app, "-L Ctrl-", "", pressing, SDL_SCANCODE_LCTRL);
            addKeyboardRects(app, 1, 27, 6, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LGUI: {
            writeToKeyboardText(app, "-Wnd-", "", pressing, SDL_SCANCODE_LGUI);
            addKeyboardRects(app, 14, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LALT: {
            writeToKeyboardText(app, "-L Alt-", "Tap to show/hide wireframes", pressing, SDL_SCANCODE_LALT);
            addKeyboardRects(app, 20, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_SPACE: {
            writeToKeyboardText(app, "-Space-", "Tap to swap blocks, double tap to start game", pressing, SDL_SCANCODE_SPACE);
            addKeyboardRects(app, 26, 27, 29, 6, pressing);

            if (!pressing) {
                app->amountPressed += 1;
            }
            break;
        }
        case SDL_SCANCODE_RALT: {
            writeToKeyboardText(app, "-R Alt-", "Tap to show/hide wireframes", pressing, SDL_SCANCODE_RALT);
            addKeyboardRects(app, 56, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_APPLICATION: {
            writeToKeyboardText(app, "-App-", "", pressing, SDL_SCANCODE_APPLICATION);
            addKeyboardRects(app, 62, 27, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LEFT: {
            writeToKeyboardText(app, "-Left-", "Tap to move block left", pressing, SDL_SCANCODE_LEFT);
            addKeyboardRects(app, 68, 28, 6, 5, pressing);
            break;
        }
        case SDL_SCANCODE_UP: {
            writeToKeyboardText(app, "-Up-", "", pressing, SDL_SCANCODE_UP);
            addKeyboardRects(app, 74, 27, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_DOWN: {
            writeToKeyboardText(app, "-Down-", "Tap to move block down, double tap for hard drop", pressing, SDL_SCANCODE_DOWN);
            addKeyboardRects(app, 74, 30, 5, 3, pressing);
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            writeToKeyboardText(app, "-Right-", "Tap to move block right", pressing, SDL_SCANCODE_RIGHT);
            addKeyboardRects(app, 79, 28, 6, 5, pressing);
            break;
        }

        // -- left side --
        case SDL_SCANCODE_GRAVE: {
            writeToKeyboardText(app, "-Grave-", "", pressing, SDL_SCANCODE_GRAVE);
            addKeyboardRects(app, 1, 4, 3, 5, pressing);
            break;
        }
        case SDL_SCANCODE_TAB: {
             writeToKeyboardText(app, "-Tab-", "", pressing, SDL_SCANCODE_TAB);
            addKeyboardRects(app, 1, 9, 5, 6, pressing);
            break;
        }
        case SDL_SCANCODE_CAPSLOCK: {
             writeToKeyboardText(app, "-Caps-", "", pressing, SDL_SCANCODE_CAPSLOCK);
            addKeyboardRects(app, 1, 15, 7, 6, pressing);
            break;
        }
        case SDL_SCANCODE_LSHIFT: {
             writeToKeyboardText(app, "-L Shift-", "", pressing, SDL_SCANCODE_LSHIFT);
            addKeyboardRects(app, 1, 21, 10, 7, pressing);
            break;
        }

        // -- right side --
        case SDL_SCANCODE_MINUS: {
            writeToKeyboardText(app, "-Minus-", "", pressing, SDL_SCANCODE_MINUS);
            addKeyboardRects(app, 65, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_EQUALS: {
            writeToKeyboardText(app, "-Equals-", "", pressing, SDL_SCANCODE_EQUALS);
            addKeyboardRects(app, 71, 3, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_BACKSPACE: {
            writeToKeyboardText(app, "-Backspace-", "", pressing, SDL_SCANCODE_BACKSPACE);
            addKeyboardRects(app, 77, 3, 8, 7, pressing);
            break;
        }

        case SDL_SCANCODE_LEFTBRACKET: { //six-seven???
            writeToKeyboardText(app, "-L Bracket-", "", pressing, SDL_SCANCODE_LEFTBRACKET);
            addKeyboardRects(app, 67, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RIGHTBRACKET: {
            writeToKeyboardText(app, "-R Bracket-", "", pressing, SDL_SCANCODE_RIGHTBRACKET);
            addKeyboardRects(app, 73, 9, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_BACKSLASH: {
            writeToKeyboardText(app, "-Backslash-", "", pressing, SDL_SCANCODE_BACKSLASH);
            addKeyboardRects(app, 79, 9, 6, 7, pressing);
            break;
        }

        case SDL_SCANCODE_SEMICOLON: {
            writeToKeyboardText(app, "-Semicolon-", "", pressing, SDL_SCANCODE_SEMICOLON);
            addKeyboardRects(app, 63, 15, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_APOSTROPHE: { //six-nine???
            writeToKeyboardText(app, "-Apostrophe-", "", pressing, SDL_SCANCODE_APOSTROPHE);
            addKeyboardRects(app, 69, 15, 6, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RETURN: {
            writeToKeyboardText(app, "-Enter-", "", pressing, SDL_SCANCODE_RETURN);
            addKeyboardRects(app, 76, 15, 9, 7, pressing);
            break;
        }
    
        case SDL_SCANCODE_COMMA: {
            writeToKeyboardText(app, "-Comma-", "", pressing, SDL_SCANCODE_COMMA);
            addKeyboardRects(app, 54, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_PERIOD: {
            writeToKeyboardText(app, "-Period-", "", pressing, SDL_SCANCODE_PERIOD);
            addKeyboardRects(app, 60, 21, 5, 7, pressing);
            break;
        }
        case SDL_SCANCODE_SLASH: {
            writeToKeyboardText(app, "-Slash-", "", pressing, SDL_SCANCODE_SLASH);
            addKeyboardRects(app, 66, 21, 6, 7, pressing);
            break;
        }
        case SDL_SCANCODE_RSHIFT: {
            writeToKeyboardText(app, "-R Shift-", "", pressing, SDL_SCANCODE_RSHIFT);
            addKeyboardRects(app, 73, 21, 12, 7, pressing);
            break;
        }
    }
}

void handleKeyboardInput(appContext *app, SDL_Scancode code) {
    switch (code) {
        case SDL_SCANCODE_LEFT: {
            if (canMove(app, app->currentTet, -1, 0) && !(app->paused || app->userPause)) {
                startSound(&sfx[MOVELEFT]);
                app->currentTet->x -= 1;
                runWireframes(app, app->currentTet);
            }
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            if (canMove(app, app->currentTet, 1, 0) && !(app->paused || app->userPause)) {
                startSound(&sfx[MOVERIGHT]);
                app->currentTet->x += 1;
                runWireframes(app, app->currentTet);
            }
            break;
        }
        case SDL_SCANCODE_DOWN: {
            if (canMove(app, app->currentTet, 0, 1) && !(app->paused || app->userPause)) {
                app->currentTet->y += 1;
            }
            break;
        }
        case SDL_SCANCODE_D: {
            if (!(app->paused || app->userPause)) {
                rotateTetrominoCW(app, app->currentTet); 
                runWireframes(app, app->currentTet);
            }
            break;
        }
        case SDL_SCANCODE_A: {
            if (!(app->paused || app->userPause)) {
                rotateTetrominoCCW(app, app->currentTet);
                runWireframes(app, app->currentTet);
            }
            break;
        }
        case SDL_SCANCODE_R: {
            if (app->winning && (!(app->paused || app->userPause))) {
                restartMainTheme();
                runWireframes(app, app->currentTet);
                resetGame(app);
                startSound(&sfx[OPEN]);
            }
            break;
        }
        case SDL_SCANCODE_H: {
            startSound(&sfx[HOLYMOLY]);
            break;
        }
        case SDL_SCANCODE_SPACE: {
            if (app->titleCard) {
                restartMainTheme();
                startSound(&sfx[OPEN]);
                app->titleCard = false;
                app->keyboardCard = true;
                break;
            } else if (app->keyboardCard) {
                restartMainTheme();
                startSound(&sfx[OPEN]);
                app->keyboardCard = false;
                app->winning = true;
                break;
            } else if (app->winning && !(app->paused || app->userPause)) {
                startSound(&sfx[SWITCH]);
                //swap values
                int temp = app->heldtet;
                app->heldtet = app->currentBlock;
                app->currentBlock = temp;

                if (app->currentBlock == -1) { //if starting
                    app->currentBlock = app->nextBlocks[0];
                    for (int n = 0; n < 3; n++) {
                        app->nextBlocks[n] = app->nextBlocks[n+1];
                    }
                    app->nextBlocks[3] = (int)SDL_rand(7); 
                }
                *app->currentTet = *app->tetArray[app->currentBlock];
                app->currentTet->x = 4;
                app->currentTet->y = 1;

                updateNextBlocks(app);
                runWireframes(app, app->currentTet);
                break;
            } else if (app->loseCard) {
                startSound(&sfx[OPEN]);
                restartMainTheme();
                resetGame(app);
                break;
            }
            break;
        }
        case SDL_SCANCODE_LALT: {
            if (!(app->paused || app->userPause)) {
                app->showWireframe = !app->showWireframe;
                runWireframes(app, app->currentTet);
                buildBoardTexture(app);
            }
            break;
        }
        case SDL_SCANCODE_RALT: {
            if (!(app->paused || app->userPause)) {
                app->showWireframe = !app->showWireframe;
                runWireframes(app, app->currentTet);
                buildBoardTexture(app);
            }
            break;
        }
        case SDL_SCANCODE_ESCAPE: {
            if (!app->showControls && app->winning) {
                startSound(&sfx[LAND]);
                app->userPause = !app->userPause;
            }
            break;
        }
        case SDL_SCANCODE_F1: {
            if ((app->showControls || !app->userPause) && app->winning) {
                startSound(&sfx[OPEN]);
                if (app->paused && !app->showControls) {
                    app->showControls = !app->showControls;
                    app->userPause = true;
                } else {
                    app->paused = !app->paused;
                    app->showControls = !app->showControls; 
                    app->userPause = false;
                }
            }

            break;
        }
        case SDL_SCANCODE_F2: {
            if ((app->showStats || !app->userPause) && app->winning) {
                startSound(&sfx[OPEN]);
                if (app->paused && !app->showStats) {
                    app->showStats = !app->showStats;
                    app->userPause = true;
                } else {
                    app->paused = !app->paused;
                    app->showStats = !app->showStats; 
                    app->userPause = false;
                }
            }
            if (app->showStats) {
                buildStatsText(app);
            }
            break;
        }
        case SDL_SCANCODE_DELETE: {
            app->closing = true;
            SDL_Event quit_event = {0};
            quit_event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&quit_event);
        }
        case SDL_SCANCODE_C: {
            app->Xmas = !app->Xmas;
            initSnow(app);
            resetTetColours(app);
            updateNextBlocks(app);
        }
    }
}
