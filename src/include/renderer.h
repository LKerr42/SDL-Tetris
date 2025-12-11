#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

struct appContext;

typedef struct setBlocks {
    bool v;
    int r;
    int g;
    int b;
} setBlocks;

void buildBoardTexture(struct appContext *app);

#endif