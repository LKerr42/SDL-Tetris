#include "include/tetromino.h"

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
