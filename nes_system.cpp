#include "nes_system.h"

nes_system::nes_system() {
    _mem = std::make_unique<nes_memory>();
    _cpu = std::make_unique<nes_cpu>();
    _ppu = std::make_unique<nes_ppu>();
    _input = std::make_unique<nes_input>();
}

void nes_system::init() {
    getCpu()->init(this);
    getPpu()->init(this);
    getMem()->init(this);
    getInput()->init();
    _stop_requested = false;
    _master_cycle = nes_cycle_t(0);
}

void nes_system::run_program(std::vector <uint8_t> program, uint16_t addr) {
    _mem->set_bytes(addr, program.data(), program.size());
    _cpu->reg().PC = addr;

    auto tick = nes_cycle_t(1);
    while (!stop_requested())
    {
        step(tick);
    }

}

void nes_system::step(nes_cycle_t count)
{
    _master_cycle += count;

    _cpu->step_to(_master_cycle);
    _ppu->step_to(_master_cycle);
}

void nes_system::load_rom(const char *rom_path) {

    auto mapper = nes_rom_loader::load_from(rom_path);
    _mem->load_mapper(mapper);
    _ppu->load_mapper(mapper);

    nes_mapper_info info;
    mapper->get_info(info);
    _cpu->reg().PC = info.code_addr;
}

