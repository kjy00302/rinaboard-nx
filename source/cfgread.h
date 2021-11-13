#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"

int cfgread_color(FILE* cfgfile, const char* name, unsigned char* colorarr);
int cfgread_face(FILE* cfgfile, const char* name, char** byteptr);
