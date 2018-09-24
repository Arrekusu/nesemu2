#pragma once

#include "common.h"
#include <cstdint>
#include <memory>
#include "nes_mapper.h"
#include "nes_system.h"
#include "nes_cycle.h"

// PPU has its own separate 16KB memory address space
// http://wiki.nesdev.com/w/index.php/PPU_memory_map

#define PPU_VRAM_SIZE 0x4000

// OAM (Object Attribute Memory) - internal memory inside PPU for 64 sprites of 4 bytes each
// wiki.nesdev.com/w/index.php/PPU_OAM
#define PPU_OAM_SIZE 0x100

//
// All register masks
// http://wiki.nesdev.com/w/index.php/PPU_registers
//

// Base name table address
// 0 = $2000; 1 = $2400; 2 = $2800; 3 = $2c00
#define PPUCTRL_BASE_NAME_TABLE_ADDR_MASK 0x3

// VRAM address increment per CPU read/write of PPUDATA
// 0: add 1, going across; 1: add 32, going down
#define PPUCTRL_VRAM_ADDR_MASK 0x4

// Sprite pattern table address for 8x8 sprites
// 0: $0000; 1: $1000; Ignored for 8x16 mode
#define PPUCTRL_SPRITE_PATTERN_TABLE_ADDR_MASK 0x8

// Background table pattern address
// 0: $0000; 1: $1000
#define PPUCTRL_BACKGROUND_PATTERN_TABLE_ADDRESS_MASK    0x10

// Sprite size
// 0: 8x8; 1: 8x16
#define PPUCTRL_SPRITE_SIZE_MASK     0x20

// PPU master/slave select
// 0: read backdrop from EXT pins; 1: output color on EXT pins
#define PPUCTRL_PPU_MASTER_SLAVE_SELECT 0x40

// Generate a NMI at start of vertical blanking interval
// 0: off; 1: on
#define PPUCTRL_NMI_AT_VBLANK_MASK 0x80

// 0: normal color; 1: grayscale
#define PPUMASK_GRAYSCALE 0x1

#define PPUMASK_BACKGROUND_IN_LEFTMOST_8 0x2
#define PPUMASK_SPRITE_IN_LEFTMOST_8 0x4

#define PPUMASK_SHOW_BACKGROUND 0x8
#define PPUMASK_SHOW_SPRITES 0x10

#define PPUMASK_EMPHASIZE_RED 0x20
#define PPUMASK_EMPHASIZE_GREEN 0x40
#define PPUMASK_EMPHASIZE_BLUE 0x80

// Previously written to a PPU register (due to not being updated for this address)
#define PPUSTATUS_LATCH_MASK 0x1f

// 0: no more than 8 sprites appear on a scanline
// 1: otherwise. however, there are bugs in hardware implementation that it can be
// set incorrectly for false positive and false negatives
// set during sprite evaluation and clear at dot 1 of pre-render line
#define PPUSTATUS_SPRITE_OVERFLOW 0x20

// Set when a nonzero pixel of sprite 0 overlaps a nonzero background pixel
// Cleared at dot 1 (second dot) of the pre-render line
#define PPUSTATUS_SPRITE_0_HIT 0x40

// 0: not in vblank; 1: in vblank
// Set at dot 1 of line 241 (the line *after* the post-render line)
// Clear after reading $2002 and at dot 1 of the pre-render line
#define PPUSTATUS_VBLANK_START 0x80

#define PPU_SCREEN_X 256
#define PPU_SCREEN_Y 240

#define PPU_SCANLINE_COUNT 262

// Only max of 8 sprites can be drawn in one scanline
#define PPU_ACTIVE_SPRITE_MAX 0x8
#define PPU_SPRITE_MAX 64

#define PPU_SPRITE_ATTR_BIT32_MASK 0x3
#define PPU_SPRITE_ATTR_BEHIND_BG 0x20
#define PPU_SPRITE_ATTR_HORIZONTAL_FLIP 0x40
#define PPU_SPRITE_ATTR_VERTICAL_FLIP 0x80

class nes_ppu {
private:
    std::shared_ptr<nes_mapper> _mapper;
    nes_mapper_flags _mirroring_flags;  // mapper flags masked by mirroring flags

    nes_system* _system;
    uint8_t *_frame_buffer;             // entire frame buffer - only 4 bit is used

    // PPUCTRL data
    uint16_t _name_tbl_addr;
    uint16_t _bg_pattern_tbl_addr;
    uint16_t _sprite_pattern_tbl_addr;
    uint16_t _ppu_addr_inc;
    bool _vblank_nmi;
    bool _use_8x16_sprite;
    uint8_t _sprite_height;

    // PPUMASK
    bool _show_bg;
    bool _show_sprites;
    bool _gray_scale_mode;

    nes_cycle_t _master_cycle;
    nes_ppu_cycle_t _scanline_cycle;
    int _cur_scanline;
    uint32_t _frame_count;

    // PPUSTATUS
    uint8_t _latch;
    bool _sprite_overflow;
    bool _vblank_started;
    bool _sprite_0_hit;

    // OAMADDR, OAMDATA
    uint8_t _oam_addr;

    // PPUSCROLL
    bool _addr_toggle;                  // the "w" register - see http://wiki.nesdev.com/w/index.php/PPU_scrolling

    uint16_t _temp_ppu_addr;            // the "t" register - see http://wiki.nesdev.com/w/index.php/PPU_scrolling
    uint8_t _scroll_y;                  // cached _scroll_y value

    // PPUDATA
    uint8_t _vram_read_buf;             // delayed VRAM reads

public:
    // PPUADDR
    // yyy NN YYYYY XXXXX
    //    ||| || ||||| +++++-- coarse X scroll
    //    ||| || +++++-------- coarse Y scroll
    //    ||| ++-------------- nametable select
    //    +++----------------- fine Y scroll
    uint16_t _ppu_addr;                 // the "v" register - see http://wiki.nesdev.com/w/index.php/PPU_scrolling
    uint8_t  _fine_x_scroll;            // the "x" register - see http://wiki.nesdev.com/w/index.php/PPU_scrolling
    uint8_t  _coarse_x_scroll;
    void step_ppu(nes_ppu_cycle_t cycle);
    void step_to(nes_cycle_t count);
    std::unique_ptr<uint8_t[]> _vram;
    std::unique_ptr<uint8_t[]> _oam;
    void init(nes_system* system);
    nes_ppu()
    {
        _vram = std::make_unique<uint8_t[]>(PPU_VRAM_SIZE);
        _oam = std::make_unique<uint8_t[]>(PPU_OAM_SIZE);
    }

    void load_mapper(std::shared_ptr<nes_mapper> &mapper);

    void fetch_tile();

    struct sprite_info
    {
        uint8_t pos_y;            // off by 1
        uint8_t tile_index;
        uint8_t attr;
        uint8_t pos_x;
    };

    void set_mirroring(nes_mapper_flags flags);

    sprite_info *get_sprite(uint8_t sprite_id)
    {
        // sprite info resides in OAM memory and there are 64 sprites x 4 bytes each = 256 bytes
        //return &((sprite_info *)_oam.get())[sprite_id];
    }

    //
    // PPU internal ram
    //
    uint8_t read_byte(uint16_t addr)
    {
        redirect_addr(addr);

        if (addr >= PPU_VRAM_SIZE)
            return 0xff;

        return _vram[addr];
    }

    void write_byte(uint16_t addr, uint8_t val)
    {
        redirect_addr(addr);

        if (addr >= PPU_VRAM_SIZE)
            return;

        _vram[addr] = val;
    }

    void write_bytes(uint16_t addr, uint8_t *src, size_t src_size)
    {
        if (addr + src_size > PPU_VRAM_SIZE)
            return;

        redirect_addr(addr);
        memcpy_s(_vram.get() + addr, PPU_VRAM_SIZE - addr, src, src_size);
    }

    void redirect_addr(uint16_t &addr)
    {
        if ((addr & 0xff00) == 0x3f00)
        {
            // mirror of palette table every 0x20 bytes
            addr &= 0xff1f;

            // mirror special case 0x3f10 = 0x3f00, 0x3f14 = 0x3f04, ...
            if ((addr & 0xfff3) == 0x3f10)
                addr &= 0x3f0f;
        }
        else if ((addr & 0xf000) == 0x2000)
        {
            // name table mirroring
            if (_mirroring_flags == nes_mapper_flags_vertical_mirroring)
                addr &= 0xf7ff;     // $2000=$2800, $2400=$2c00
            else if (_mirroring_flags == nes_mapper_flags_horizontal_mirroring)
                addr &= 0xfbff;     // $2000=$2400, $2800=$2c00
            else if (_mirroring_flags == nes_mapper_flags_one_screen_lower_bank)
                addr &= 0xf3ff;     // $2000 mapped to all the other 3
            else if (_mirroring_flags == nes_mapper_flags_one_screen_upper_bank)
                addr = (addr & 0xf3ff) | 0x400; // $2400 mapped to all the other 3
            else assert(!"Unsupported mirroring modes");
        }
        else if (addr >= 0x3000)
        {
            // 0x3000~0x3f00 mirrors to 0x2000~0x2f00
            addr -= 0x1000;
        }
    }

    //
    // All registers
    //
    void write_latch(uint8_t val)
    {
        _latch = val;
    }

    uint8_t read_latch()
    {
        // This latch is also subject to decay but it is random so no need to emulate that
        return _latch;
    }

    void write_PPUCTRL(uint8_t val)
    {
        write_latch(val);

        uint8_t name_table_addr_bit = val & PPUCTRL_BASE_NAME_TABLE_ADDR_MASK;
        _temp_ppu_addr = (_temp_ppu_addr & 0xf3ff) | ((val & PPUCTRL_BASE_NAME_TABLE_ADDR_MASK) << 10);
        _name_tbl_addr = 0x2000 + uint16_t(name_table_addr_bit) * 0x400;

        _bg_pattern_tbl_addr = (val & PPUCTRL_BACKGROUND_PATTERN_TABLE_ADDRESS_MASK) << 0x8;
        _sprite_pattern_tbl_addr = (val & PPUCTRL_SPRITE_PATTERN_TABLE_ADDR_MASK) << 0x8;

        _use_8x16_sprite = val & PPUCTRL_SPRITE_SIZE_MASK;
        if (_use_8x16_sprite)
            _sprite_height = 16;
        else
            _sprite_height = 8;

        _ppu_addr_inc = (val & PPUCTRL_VRAM_ADDR_MASK) ? 0x20 : 1;

        _vblank_nmi = (val & PPUCTRL_NMI_AT_VBLANK_MASK);
    }

    void write_PPUMASK(uint8_t val)
    {
        write_latch(val);

        _show_bg = val & PPUMASK_SHOW_BACKGROUND;
        _show_sprites = val & PPUMASK_SHOW_SPRITES;
        _gray_scale_mode = val & PPUMASK_GRAYSCALE;
    }

    uint8_t read_PPUSTATUS()
    {
        uint8_t status = _latch & PPUSTATUS_LATCH_MASK;
        if (_sprite_0_hit)
            status |= PPUSTATUS_SPRITE_0_HIT;
        if (_sprite_overflow)
            status |= PPUSTATUS_SPRITE_OVERFLOW;
        if (_vblank_started)
            status |= PPUSTATUS_VBLANK_START;


            // clear various flags after reading
            _vblank_started = false;
            _addr_toggle = false;
            write_latch(status);

        return status;
    }

    void write_OAMADDR(uint8_t val)
    {
        write_latch(val);

        _oam_addr = val;
    }

    void write_OAMDATA(uint8_t val)
    {
        write_latch(val);

        _oam[_oam_addr] = val;
        _oam_addr++;
    }

    uint8_t read_OAMDATA()
    {

        uint8_t val = _oam[_oam_addr];
        write_latch(val);
        return val;
    }

    void write_PPUSCROLL(uint8_t val)
    {

        write_latch(val);

        _addr_toggle = !_addr_toggle;
        if (_addr_toggle)
        {
            // first write
            _temp_ppu_addr = (_temp_ppu_addr & 0xffe0) | (val >> 3);
            _fine_x_scroll = val & 0x7;
            _coarse_x_scroll = _temp_ppu_addr & 0x1f;
        }
        else
        {
            // second write
            _temp_ppu_addr = (_temp_ppu_addr & 0xc1f) | (uint16_t(val & 0xf8) << 2) | (uint16_t(val & 0x7) << 12);
            _scroll_y = val;
        }
    }

    void write_PPUADDR(uint8_t val)
    {
        write_latch(val);

        _addr_toggle = !_addr_toggle;
        if (_addr_toggle)
        {
            // first write
            // note that both PPUADDR(2006) and PPUSCROLL (2005) share the same _temp_ppu_addr
            _temp_ppu_addr = (_temp_ppu_addr & 0x00ff) | (uint16_t(val & 0x3f) << 8);
        }
        else
        {
            // second write
            // note that both PPUADDR(2006) and PPUSCROLL (2005) share the same _temp_ppu_addr
            _temp_ppu_addr = (_temp_ppu_addr & 0xff00) | val;
            _ppu_addr = _temp_ppu_addr;
        }
    }

    void write_PPUDATA(uint8_t val)
    {
        write_latch(val);
        write_byte(_ppu_addr, val);
        _ppu_addr += _ppu_addr_inc;
    }

    uint8_t read_PPUDATA()
    {
        // use _vram_read_buf to implement VRAM delay reading buffer behavior
        // First time read will read from buffer and then update the buffer
        // This means all the reads are delayed by 1 read
        uint8_t val = _vram_read_buf;
        uint8_t new_val = read_byte(_ppu_addr);

        bool is_palette = ((_ppu_addr & 0xff00) == 0x3f00);
            // for palette - the read buf is updated with the mirrored nametable address
            if (is_palette)
                _vram_read_buf = read_byte(_ppu_addr - 0x1000);
            else
                _vram_read_buf = new_val;
            _ppu_addr += _ppu_addr_inc;

        write_latch(val);

        // palette reads returns the correct data immediately
        if (is_palette)
            return new_val;

        return val;
    }

    void write_OAMDMA(uint8_t val);

    void oam_dma(uint16_t addr);
};
