#include "cfgread.h"

static char cfgreadbuf[128];

int cfgread_color(FILE* cfgfile, const char* name,unsigned char* colorarr){
    char format[20] = {0};
    strcat(format, name);
    strcat(format, "=#%s\n");
    if ((fscanf(cfgfile, format, cfgreadbuf) == 0) || (cfgreadbuf[0] == 0) || strlen(cfgreadbuf) != 6){
        return 0;
    }
    char* endptr;
    int color = (int)strtol(cfgreadbuf, &endptr, 16);
    if (*endptr != 0){
        return 0;
    }
    colorarr[0] = (color >> 16 ) & 0xff;
    colorarr[1] = (color >> 8 ) & 0xff;
    colorarr[2] = color & 0xff;
    return 1;
}

int cfgread_face(FILE* cfgfile, const char* name, char** byteptr){
    char format[20] = {0};
    strcat(format, name);
    strcat(format, "=%s\n");
    if ((fscanf(cfgfile, format, cfgreadbuf) == 0) || (cfgreadbuf[0] == 0) || strlen(cfgreadbuf) != 96){
        return 0;
    }
    char* bytearr = malloc(96);
    if (bytearr == NULL){
        return 0;
    }
    b64decode(bytearr, cfgreadbuf, 96);
    *byteptr = bytearr;
    return 1;
}
