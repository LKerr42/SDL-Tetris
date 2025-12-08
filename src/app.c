#include <string.h>
#include "include/app.h"
#include "include/tetromino.h"

void prependChar(char *str, char c) {
    size_t len = strlen(str);

    memmove(str + 1, str, len + 1);

    str[0] = c;
}