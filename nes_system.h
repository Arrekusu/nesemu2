#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include "nes_mapper.h"
#include "nes_cpu.h"
#include "nes_ppu.h"

class nes_memory;

class nes_system {
private:
    std::unique_ptr<nes_cpu> _cpu;
    std::unique_ptr<nes_ppu> _ppu;
    std::unique_ptr<nes_input> _input;
    std::unique_ptr<nes_memory> _mem;
    bool _stop_requested;
public:
    nes_cycle_t _master_cycle;

    void init();
    nes_system();
    nes_cpu* getCpu(){ return _cpu.get();}
    nes_ppu* getPpu(){ return _ppu.get();}
    nes_input* getInput(){ return _input.get();}
    nes_memory* getMem(){ return _mem.get();}
    void run_program(std::vector<uint8_t> program, uint16_t addr);
    void run_rom(const char *rom_path);

    void step(nes_cycle_t count);
    void stop(){
        _stop_requested = true;
    }

    bool stop_requested() { return _stop_requested; }

#define FLAG_6_USE_VERTICAL_MIRRORING_MASK 0x1
#define FLAG_6_HAS_BATTERY_BACKED_PRG_RAM_MASK 0x2
#define FLAG_6_HAS_TRAINER_MASK  0x4
#define FLAG_6_USE_FOUR_SCREEN_VRAM_MASK 0x8
#define FLAG_6_LO_MAPPER_NUMBER_MASK 0xf0
#define FLAG_7_HI_MAPPER_NUMBER_MASK 0xf0

    void load_rom(const char *rom_path);

    static nes_mapper_nrom load_from(const char *path)
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path, std::ifstream::in | std::ifstream::binary);

        // Parse header
        ines_header header;
        file.read((char *)&header, sizeof(header));

        if (header.flag6 & FLAG_6_HAS_TRAINER_MASK)
        {
            // skip the 512-byte trainer
            file.seekg(0x200, std::ios_base::cur);
        }

        bool vertical_mirroring = header.flag6 & FLAG_6_USE_VERTICAL_MIRRORING_MASK;

        if (header.flag7 == 0x44)
        {
            // This might be one of the earlier dumps with bad iNes header (D stands for diskdude)
            header.flag7 = 0;
        }

        int prg_rom_size = header.prg_size * 0x4000;    // 16KB
        int chr_rom_size = header.chr_size * 0x2000;    // 8KB

        auto prg_rom = std::make_shared<std::vector<uint8_t>>(prg_rom_size);
        auto chr_rom = std::make_shared<std::vector<uint8_t>>(chr_rom_size);

        file.read((char *)prg_rom->data(), prg_rom->size());
        file.read((char *)chr_rom->data(), chr_rom->size());

        file.close();

        return {prg_rom, chr_rom, vertical_mirroring};
    }
};

