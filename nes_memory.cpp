#include "nes_memory.h"
#include "nes_system.h"

void nes_memory::init(nes_system *system) {
    memset(&memory[0], 0, RAM_SIZE);
    _ppu = system->getPpu();
    _input = system->getInput();
}

void nes_memory::load_mapper(std::shared_ptr<nes_mapper> &mapper)
{
    // unset previous mapper
    _mapper = nullptr;

    // Give mapper a chance to copy all the bytes needed
    mapper->on_load_ram(*this);

    _mapper = mapper;
    _mapper->get_info(_mapper_info);
}

uint8_t nes_memory::read_io_reg(uint16_t addr)
{
    switch (addr)
    {
        case 0x2002: return _ppu->read_PPUSTATUS();
        case 0x2004: return _ppu->read_OAMDATA();
        case 0x2007: return _ppu->read_PPUDATA();
        case 0x4016: return _input->read_CONTROLLER(0);
        case 0x4017: return _input->read_CONTROLLER(1);
    }

    return _ppu->read_latch();
}

void nes_memory::write_io_reg(uint16_t addr, uint8_t val)
{
    switch (addr)
    {
        case 0x2000: _ppu->write_PPUCTRL(val); return;
        case 0x2001: _ppu->write_PPUMASK(val); return;
        case 0x2003: _ppu->write_OAMADDR(val); return;
        case 0x2004: _ppu->write_OAMDATA(val); return;
        case 0x2005: _ppu->write_PPUSCROLL(val); return;
        case 0x2006: _ppu->write_PPUADDR(val); return;
        case 0x2007: _ppu->write_PPUDATA(val); return;
        case 0x4014: _ppu->write_OAMDMA(val); return;
        case 0x4016: _input->write_CONTROLLER(val); return;
        case 0x4017: _input->write_CONTROLLER(val); return;
    }

    _ppu->write_latch(val);
}

