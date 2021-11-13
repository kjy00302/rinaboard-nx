#include <SDL.h>
#include <switch.h>


extern "C" {
    #include "base64.h"
    #include "cfgread.h"
    #include "utils.h"
}

typedef struct {
    int idx;
    const char* formatstr;
} face_map;

const face_map face_format[]= {
    {0, "FACE_DEFAULT"},
    {1,"FACE_A"},
    {2,"FACE_B"},
    {4,"FACE_X"},
    {8,"FACE_Y"},
    {3,"FACE_AB"},
    {5,"FACE_AX"},
    {10,"FACE_BY"},
    {12,"FACE_XY"},
};

const int BUTTON_A = 0;
const int BUTTON_B = 1;
const int BUTTON_X = 2;
const int BUTTON_Y = 3;
const int BUTTON_PLUS = 10;
const int BUTTON_RIGHTSR = 27;



void clear_screen(SDL_Renderer* renderer, int r, int g, int b){
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xff);
    SDL_RenderClear(renderer);
}

void draw_rect(SDL_Renderer* renderer, int x, int y, unsigned char* color){
    SDL_Rect rect;
    rect.x = x; rect.y = y;
    rect.w = 38; rect.h = 38;
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], 0xff);
    SDL_RenderFillRect(renderer, &rect);
}

int main(int argc, char *argv[])
{
    char* facedata[16] = {0};
    unsigned char color_foreground[3];
    unsigned char color_background[3];
    unsigned char color_line[3];
    
    FILE* cfgfile = fopen("rinaboard_config", "r");
    if (!cfgfile){
        errorhandle("Cannot open config");
    }
    if (cfgread_color(cfgfile, "FOREGROUNDCOLOR", color_foreground) == 0){
        errorhandle("Cannot read foreground color");
    }
    if (cfgread_color(cfgfile, "BACKGROUNDCOLOR", color_background) == 0){
        errorhandle("Cannot read background color");
    }
    if (cfgread_color(cfgfile, "LINECOLOR", color_line) == 0){
        errorhandle("Cannot read line color");
    }
    for (int i=0;i<9;i++){
        cfgread_face(cfgfile, face_format[i].formatstr, &facedata[face_format[i].idx]);
    }
    fclose(cfgfile);
    
    
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Joystick *joystick;
    SDL_Texture *facetexture;
    int done = 0;
    int redraw = 1;
    int face_idx = 0;
    int current_idx = 0;

    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // create a renderer (OpenGL ES2)
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    facetexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, 1280, 720);

    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    
    joystick = SDL_JoystickOpen(0);
    if (!joystick) {
        SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (event.jbutton.which == 0) {
                        if (event.jbutton.button == BUTTON_RIGHTSR) {
                            // (A) button down
                            redraw = 1;
                        } else if (event.jbutton.button == BUTTON_PLUS) {
                            // (+) button down
                            done = 1;
                        }
                    }
                    break;

                default:
                    break;
            }
        }
        
        if (redraw){
            face_idx = 0;
            face_idx += SDL_JoystickGetButton(joystick, BUTTON_A);
            face_idx += SDL_JoystickGetButton(joystick, BUTTON_B) << 1;
            face_idx += SDL_JoystickGetButton(joystick, BUTTON_X) << 2;
            face_idx += SDL_JoystickGetButton(joystick, BUTTON_Y) << 3;
            
            if (facedata[face_idx] != NULL){
                current_idx = face_idx;
                SDL_SetRenderTarget(renderer, facetexture);
                clear_screen(renderer, color_line[0], color_line[1], color_line[2]);
                for (int i=0;i<32*18;i++){
                    int t = facedata[current_idx][i / 8] & 0x80 >> (i%8);
                    draw_rect(renderer, (i%32)*40+1, (i/32)*40+1, t?color_foreground:color_background);
                }
            }
            redraw = 0;
        }
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy( renderer, facetexture, NULL, NULL );
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
