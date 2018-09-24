#include <cstdint>
#include "nes_ppu.h"
#include "nes_system.h"
#include "nes_cycle.h"

// fetching tile for current line
void nes_ppu::fetch_tile()
{

}

void nes_ppu::init(nes_system* system)
{
    _system = system;
    // PPUCTRL data
    _name_tbl_addr = 0;
    _bg_pattern_tbl_addr = 0;
    _sprite_pattern_tbl_addr = 0;
    _ppu_addr_inc = 1;
    _vblank_nmi = true;
    _use_8x16_sprite = false;
    _sprite_height = 8;

    // PPUMASK
    _show_bg = false;
    _show_sprites = false;
    _gray_scale_mode = false;

    // PPUSTATUS
    _latch = 0;
    _sprite_overflow = false;
    _vblank_started = false;
    _sprite_0_hit = false;

    // OAMADDR, OAMDATA
    _oam_addr = 0;

    // PPUSCROLL
    _addr_toggle = false;

    // PPUADDR
    _ppu_addr = 0;
    _temp_ppu_addr = 0;

    _coarse_x_scroll = 0;
    _fine_x_scroll = 0;
    _scroll_y = 0;

    // PPUDATA
    _vram_read_buf = 0;

    _master_cycle = nes_cycle_t(0);
    _scanline_cycle = nes_cycle_t(0);
    _cur_scanline = 0;
    _frame_count = 0;
}

void nes_ppu::load_mapper(std::shared_ptr<nes_mapper> &mapper)
{
    // unset previous mapper
    _mapper = nullptr;

    // Give mapper a chance to copy all the bytes needed
    mapper->on_load_ppu(*this);

    nes_mapper_info info;
    mapper->get_info(info);
    set_mirroring(info.flags);

    _mapper = mapper;
}

void nes_ppu::write_OAMDMA(uint8_t val)
{
    // @TODO - CPU is suspended and take 513/514 cycle
    _system->getCpu()->request_dma((uint16_t(val) << 8));
}

void nes_ppu::oam_dma(uint16_t addr)
{
    if (_oam_addr == 0)
    {
        // simple case - copy the 0x100 bytes directly
        _system->getMem()->get_bytes(_oam.get(), PPU_OAM_SIZE, addr, PPU_OAM_SIZE);
    }
    else
    {
        // the copy starts at _oam_addr and wraps around
        int copy_before_wrap = 0x100 - _oam_addr;
        _system->getMem()->get_bytes(_oam.get() + _oam_addr, copy_before_wrap, addr, copy_before_wrap);
        _system->getMem()->get_bytes(_oam.get(), PPU_OAM_SIZE - copy_before_wrap, addr + copy_before_wrap, PPU_OAM_SIZE - copy_before_wrap);
    }
}

void nes_ppu::set_mirroring(nes_mapper_flags flags)
{
    _mirroring_flags = nes_mapper_flags(flags & nes_mapper_flags_mirroring_mask);
}

void nes_ppu::step_to(nes_cycle_t count)
{
    while (_master_cycle < count && !_system->stop_requested())
    {
        step_ppu(nes_ppu_cycle_t(1));

        if (_cur_scanline <= 239)
        {
            //fetch_tile_pipeline();
            //fetch_sprite_pipeline();
        }
        else if (_cur_scanline == 240)
        {
        }
        else if (_cur_scanline < 261)
        {
            if (_cur_scanline == 241 && _scanline_cycle == nes_ppu_cycle_t(1))
            {
                //NES_TRACE4("[NES_PPU] SCANLINE = 241, VBlank BEGIN");
                _vblank_started = true;
                if (_vblank_nmi)
                {
                    // Request NMI so that games can do their rendering
                    _system->getCpu()->request_nmi();
                }
            }

            // @HACK - account for a race where you have LDA $2002_PPUSTATUS and end of VBLANK at the same time
            // This moves end of NMI a bit earlier to compensate for that
            if (_cur_scanline == 260 && _scanline_cycle > nes_ppu_cycle_t(341 - 12))
                _vblank_started = false;
        }
        else
        {
            if (_cur_scanline == 261)
            {
                if (_scanline_cycle == nes_ppu_cycle_t(0))
                {
                    //NES_TRACE4("[NES_PPU] SCANLINE = 261, VBlank END");
                    _vblank_started = false;

                    // Reset _ppu_addr to top-left of the screen
                    // But only do so when rendering is on (otherwise it will interfer with PPUDATA writes)
                    if (_show_bg || _show_sprites)
                    {
                        _ppu_addr = _temp_ppu_addr;
                    }
                }
                else if (_scanline_cycle == nes_ppu_cycle_t(1))
                {
                    _sprite_0_hit = false;
                }
            }

            // pre-render scanline
            if (_scanline_cycle == nes_ppu_cycle_t(340) && _frame_count % 2 == 1)
            {
                // odd frame skip the last cycle
                step_ppu(nes_ppu_cycle_t(1));
            }
        }
    }
}

void nes_ppu::step_ppu(nes_ppu_cycle_t count)
{
    assert(count < PPU_SCANLINE_CYCLE);

    _master_cycle += nes_ppu_cycle_t(count);
    _scanline_cycle += nes_ppu_cycle_t(count);

    if (_scanline_cycle >= PPU_SCANLINE_CYCLE)
    {
        _scanline_cycle %= PPU_SCANLINE_CYCLE;
        _cur_scanline++;
        if (_cur_scanline >= PPU_SCANLINE_COUNT)
        {
            _cur_scanline %= PPU_SCANLINE_COUNT;
            //swap_buffer();
            _frame_count++;
            //NES_TRACE4("[NES_PPU] FRAME " << std::dec << _frame_count << " ------ ");

            //if (_auto_stop && _frame_count > _stop_after_frame)
            //{
            //    NES_TRACE1("[NES_PPU] FRAME exceeding " << std::dec << _stop_after_frame << " -> stopping...");
            //    _system->stop();
            //}
        }
        //NES_TRACE4("[NES_PPU] SCANLINE " << std::dec << (uint32_t) _cur_scanline << " ------ ");
    }
}