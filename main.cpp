#include <iostream>
#include "SDL.h"
#include "nes_logger.h"
#include "nes_system.h"

const int TILE_SIZE = 8;
const int SCREEN_WIDTH = 32 * TILE_SIZE;
const int SCREEN_HEIGHT = 30 * TILE_SIZE;

const int BYTE2_FLIP_HORIZ_MASK = 0x40;
const int BYTE2_FLIP_VERT_MASK = 0x80;

const int NAME_TABLE_SIZE = 960;

struct tile {
    uint32_t* pixels;
    bool* transparency;
};

Uint32 make_argb(Uint8 r, Uint8 g, Uint8 b) {
    return static_cast<Uint32>((r << 16) | (g << 8) | b);
}

static uint32_t system_palette[] =
        {
                make_argb(84,  84,  84),    make_argb(0,  30, 116),    make_argb(8,  16, 144),    make_argb(48,   0, 136),   make_argb(68,   0, 100),   make_argb(92,   0,  48),   make_argb(84,   4,   0),   make_argb(60,  24,   0),   make_argb(32,  42,   0),   make_argb(8,  58,   0),   make_argb(0,  64,   0),    make_argb(0,  60,   0),    make_argb(0,  50,  60),    make_argb(0,   0,   0),   make_argb(0, 0, 0), make_argb(0, 0, 0),
                make_argb(152, 150, 152),   make_argb(8,  76, 196),    make_argb(48,  50, 236),   make_argb(92,  30, 228),   make_argb(136,  20, 176),  make_argb(160,  20, 100),  make_argb(152,  34,  32),  make_argb(120,  60,   0),  make_argb(84,  90,   0),   make_argb(40, 114,   0),  make_argb(8, 124,   0),    make_argb(0, 118,  40),    make_argb(0, 102, 120),    make_argb(0,   0,   0),   make_argb(0, 0, 0), make_argb(0, 0, 0),
                make_argb(236, 238, 236),   make_argb(76, 154, 236),   make_argb(120, 124, 236),  make_argb(176,  98, 236),  make_argb(228,  84, 236),  make_argb(236,  88, 180),  make_argb(236, 106, 100),  make_argb(212, 136,  32),  make_argb(160, 170,   0),  make_argb(116, 196,   0), make_argb(76, 208,  32),   make_argb(56, 204, 108),   make_argb(56, 180, 204),   make_argb(60,  60,  60),  make_argb(0, 0, 0), make_argb(0, 0, 0),
                make_argb(236, 238, 236),   make_argb(168, 204, 236),  make_argb(188, 188, 236),  make_argb(212, 178, 236),  make_argb(236, 174, 236),  make_argb(236, 174, 212),  make_argb(236, 180, 176),  make_argb(228, 196, 144),  make_argb(204, 210, 120),  make_argb(180, 222, 120), make_argb(168, 226, 144),  make_argb(152, 226, 180),  make_argb(160, 214, 228),  make_argb(160, 162, 160), make_argb(0, 0, 0), make_argb(0, 0, 0)
        };

Uint32 *getTilePixels(uint8_t* pattern_table, int tileId, Uint8* palette) {
    int count = 0x1000 + tileId * 8 * 2;
    tile res{};
    res.pixels = new Uint32[8 * 8];
    for (int row = 0; row < 8; row++){
        Uint8 tileRow2 = pattern_table[count];
        Uint8 tileRow1 = pattern_table[count + 8];
        for(int col = 0; col < 8; col++){
            Uint8 value = static_cast<Uint8>((((tileRow1 & 0x80) != 0) << 1) | (((tileRow2 & 0x80) != 0)));
            res.pixels[row * 8 + col] = system_palette[palette[value]];
            tileRow1 <<= 1;
            tileRow2 <<= 1;
        }
        count++;
    }
    return res.pixels;
}

void fillPixels(uint8_t* pattern_table, Uint32* pixels, int row, int col, int tileId, Uint8* palette){
    Uint32* pix = getTilePixels(pattern_table, tileId, palette);
    int count = 0;
    for(int i = row; i < row + 8; i++){
        for (int j = col; j < col + 8; j++){
            if (i < 0 || j < 0) continue;
            pixels[i * SCREEN_WIDTH + j] = pix[count++];
        }
    }
}

tile* getSpriteTilePixels(tile* res, uint8_t* pattern_table, int tileId, Uint8* palette) {
    int count = 0x0000 + tileId * 8 * 2;
    res->pixels = new Uint32[8 * 8];
    res->transparency = new bool[8 * 8];
    for (int row = 0; row < 8; row++){
        Uint8 tileRow2 = pattern_table[count];
        Uint8 tileRow1 = pattern_table[count + 8];
        for(int col = 0; col < 8; col++){
            Uint8 value = static_cast<Uint8>((((tileRow1 & 0x80) != 0) << 1) | (((tileRow2 & 0x80) != 0)));
            if (value != 0){
                res->pixels[row * 8 + col] = system_palette[palette[value]];
                res->transparency[row * 8 + col] = false;
            } else {
                res->transparency[row * 8 + col] = true;
            }
            tileRow1 <<= 1;
            tileRow2 <<= 1;
        }
        count++;
    }
    return res;
}

void fillSpritePixels(uint8_t* pattern_table, Uint32* pixels, int row, int col, int tileId, Uint8* palette, bool horizMirror, bool vertMirror){
    tile tile{};
    getSpriteTilePixels(&tile, pattern_table, tileId, palette);
    Uint32* pix = tile.pixels;
    bool* transparency = tile.transparency;
    int count = 0;
    if (horizMirror && vertMirror){
        for(int i = row + 7; i >= row; i--){
            for (int j = col + 7; j >= col; j--){
                uint32_t color = pix[count];
                if (!transparency[count++]){
                    pixels[i * SCREEN_WIDTH + j] = color;
                }
            }
        }
    } else if (horizMirror){
        for(int i = row; i < row + 8; i++){
            for (int j = col + 7; j >= col; j--){
                uint32_t color = pix[count];
                if (!transparency[count++]) {
                    pixels[i * SCREEN_WIDTH + j] = color;
                }
            }
        }
    } else if (vertMirror){
        for(int i = row + 7; i >= row; i--){
            for (int j = col; j < col + 8; j++){
                uint32_t color = pix[count];
                if (!transparency[count++]){
                    pixels[i * SCREEN_WIDTH + j] = color;
                }
            }
        }
    } else {
        for(int i = row; i < row + 8; i++){
            for (int j = col; j < col + 8; j++){
                uint32_t color = pix[count];
                if (!transparency[count++]){
                    pixels[i * SCREEN_WIDTH + j] = color;
                }
            }
        }
    }
}

class sdl_keyboard_controller : public nes_input_device
{
public:
    sdl_keyboard_controller()
    {
    }

    virtual nes_button_flags poll_status()
    {
        return sdl_keyboard_controller::get_status();
    }

    ~sdl_keyboard_controller()
    {
    }

public:
    static nes_button_flags get_status()
    {
        const Uint8 *states = SDL_GetKeyboardState(NULL);

        uint8_t flags = 0;
        for (int i = 0; i < 8; i++)
        {
            flags <<= 1;
            flags |= states[s_buttons[i]];
        }

        return (nes_button_flags)flags;
    }

private:

    static const SDL_Scancode s_buttons[];
};

const SDL_Scancode sdl_keyboard_controller::s_buttons[] = {
        SDL_SCANCODE_L,
        SDL_SCANCODE_J,
        SDL_SCANCODE_SPACE,
        SDL_SCANCODE_RETURN,
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D
};

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("Cannot init SDL!! Error: %s\n", SDL_GetError());
    } else {
        window = SDL_CreateWindow("NESEmu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4, SDL_WINDOW_SHOWN);
        if (window == NULL){
            printf("Cannot create window. Error: %s\n", SDL_GetError());
        } else {
            surface = SDL_GetWindowSurface(window);
            SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetScale(renderer, 4, 4);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

    nes_system system;
    system.init();
    system.load_rom("/home/alex/CLionProjects/nesemu2/ic.nes");

    system.getInput()->register_input(0, std::make_shared<sdl_keyboard_controller>());

    auto* pixels = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(pixels, 255, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

    auto* tableRender1 = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(tableRender1, 255, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

    auto* tableRender2 = new Uint32[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(tableRender2, 255, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

    Uint64 prev_counter = SDL_GetPerformanceCounter();
    Uint64 count_per_second = SDL_GetPerformanceFrequency();
    bool quit = false;
    SDL_Event event;

    while (!quit){
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        //
        // Calculate delta tick as the current frame
        // We ask the NES to step corresponding CPU cycles
        //
        Uint64 cur_counter = SDL_GetPerformanceCounter();

        Uint64 delta_ticks = cur_counter - prev_counter;
        prev_counter = cur_counter;
        if (delta_ticks == 0)
            delta_ticks = 1;
        auto cpu_cycles = ms_to_nes_cycle((double)delta_ticks * 1000 / count_per_second);

        // Avoids a scenario where the loop keeps getting longer
        if (cpu_cycles > nes_cycle_t(NES_CLOCK_HZ))
            cpu_cycles = nes_cycle_t(NES_CLOCK_HZ);

        for (nes_cycle_t i = nes_cycle_t(0); i < cpu_cycles; ++i)
            system.step(nes_cycle_t(1));

        uint8_t * chr_table = system.getPpu()->_vram.get();
        uint8_t * nameTable1 = chr_table + 0x2000;
        uint8_t * nameTable2 = chr_table + 0x2400;
        uint8_t * attrib1 = chr_table + 0x23C0;
        uint8_t * attrib2 = chr_table + 0x27C0;
        uint8_t * imagePalette = chr_table + 0x3F00;
        uint8_t * spritePalette = chr_table + 0x3F10;

        uint8_t * spriteOam = system.getPpu()->_oam.get();
        int count1 = 0;
        int count2 = 0;
        bool upper = false;
        bool left = false;
        //TABLE RENDER 1
        for(int row = 0; row < 30; row++){
            if ((row % 2) == 0) {
                upper = !upper;
            }
            left = false;
            for(int col = 0; col < 32; col++){
                if ((col % 2) == 0){
                    left = !left;
                }
                int shift = 0;
                if (upper && !left){
                    shift = 2;
                }
                if (!upper && left){
                    shift = 4;
                }
                if (!upper && !left){
                    shift = 6;
                }
                fillPixels(chr_table, tableRender1, row * 8, col * 8, nameTable1[count1++], imagePalette + 4 * ((attrib1[(row / 4) * 8 + (col / 4)] >> shift) & 0x3));
            }
        }

        count1 = 0;
        upper = false;
        left = false;
        //TABLE RENDER 2
        for(int row = 0; row < 30; row++){
            if ((row % 2) == 0) {
                upper = !upper;
            }
            left = false;
            for(int col = 0; col < 32; col++){
                if ((col % 2) == 0){
                    left = !left;
                }
                int shift = 0;
                if (upper && !left){
                    shift = 2;
                }
                if (!upper && left){
                    shift = 4;
                }
                if (!upper && !left){
                    shift = 6;
                }
                fillPixels(chr_table, tableRender2, row * 8, col * 8, nameTable2[count1++], imagePalette + 4 * ((attrib2[(row / 4) * 8 + (col / 4)] >> shift) & 0x3));
            }
        }

        int xScroll = system.getPpu()->_coarse_x_scroll * 8 + system.getPpu()->_fine_x_scroll;
        for (int row = 0; row < SCREEN_HEIGHT; row++){
            for (int col = 0; col < SCREEN_WIDTH; col++){
                if (col >= SCREEN_WIDTH - xScroll){
                    pixels[row * SCREEN_WIDTH + col] = tableRender2[row * SCREEN_WIDTH + ((col + xScroll) % SCREEN_WIDTH)];
                } else {
                    pixels[row * SCREEN_WIDTH + col] = tableRender1[row * SCREEN_WIDTH + col + xScroll];
                }
            }
        }

        int spriteCount = 0;
        for (int i = 0; i < 64; i++){
            uint8_t yPos = spriteOam[spriteCount++];
            uint8_t byte1 = spriteOam[spriteCount++];
            uint8_t byte2 = spriteOam[spriteCount++];
            bool horizFlip = byte2 & BYTE2_FLIP_HORIZ_MASK;
            bool vertFlip = byte2 & BYTE2_FLIP_VERT_MASK;
            uint8_t paletteNum = byte2 & 0x3;
            uint8_t xPos = spriteOam[spriteCount++];
            fillSpritePixels(chr_table, pixels, yPos, xPos, byte1, spritePalette + paletteNum * 4, horizFlip, vertFlip);
        }

        SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
