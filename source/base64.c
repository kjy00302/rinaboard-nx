
const static char B64ENCTABLE[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static char B64DECTABLE[96] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, // 48
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, // 64
    0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, // 80
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, // 96
    0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 112
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, // 128
}; // Starts from 32!

void b64encode(char* dest, const char* src, unsigned int len){
    int buf=0;
    int bufcnt = 0;
    int outcnt = 0;
    for (int i=0;i<len;i++){
        buf <<= 8;
        buf |= src[i];
        bufcnt += 8;
        while (bufcnt >= 6){
            dest[outcnt++] = B64ENCTABLE[(buf >> (bufcnt - 6)) & 0x3f];
            bufcnt -= 6;
        }
    }
    if (bufcnt > 0){
        bufcnt = 6 - bufcnt;
        buf <<= bufcnt;
        dest[outcnt++] = B64ENCTABLE[buf & 0x3f];
        for (int i=bufcnt;i>0;i-=2){
            dest[outcnt++] = '=';
        }
    }
    dest[outcnt] = '\0';
}

void b64decode(char* dest, const char* src, unsigned int len){
    int buf = 0;
    int bufcnt = 0;
    int outcnt = 0;
    for (int i=0;i<len;i++){
        buf <<= 6;
        if (src[i] == '=') break;
        buf |= B64DECTABLE[src[i] - 32];
        bufcnt += 6;
        while (bufcnt >= 8){
            dest[outcnt++] = (buf >> (bufcnt - 8) & 0xff);
            bufcnt -= 8;
        }
    }
}
