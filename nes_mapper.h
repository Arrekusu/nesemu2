#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <fstream>
#include <assert.h>

class nes_memory;
class nes_ppu;

enum nes_mapper_flags : uint16_t
{
    nes_mapper_flags_none = 0,

    nes_mapper_flags_mirroring_mask = 0x3,

    // A, B
    // A, B
            nes_mapper_flags_vertical_mirroring = 0x2,

    // A, A
    // B, B
            nes_mapper_flags_horizontal_mirroring = 0x3,

    // ?
            nes_mapper_flags_one_screen_upper_bank = 0x1,

    // ?
            nes_mapper_flags_one_screen_lower_bank = 0x0,

    // Has registers
            nes_mapper_flags_has_registers = 0x4,
};

struct nes_mapper_info
{
    uint16_t code_addr;            // start running code here
    uint16_t reg_start;            // beginning of mapper registers
    uint16_t reg_end;              // end of mapper registers - inclusive
    nes_mapper_flags flags;        // whatever flags you might need
};

struct ines_header
{
    uint8_t magic[4];       // 0x4E, 0x45, 0x53, 0x1A
    uint8_t prg_size;       // PRG ROM in 16K
    uint8_t chr_size;       // CHR ROM in 8K, 0 -> using CHR RAM
    uint8_t flag6;
    uint8_t flag7;
    uint8_t prg_ram_size;   // PRG RAM in 8K
    uint8_t flag9;
    uint8_t flag10;         // unofficial
    uint8_t reserved[5];    // reserved
};

class nes_mapper
{
public :
    nes_mapper(){};
    //
    // Called when mapper is loaded into memory
    // Useful when all you need is a one-time memcpy
    //
    virtual void on_load_ram(nes_memory &mem) = 0;

    //
    // Called when mapper is loaded into PPU
    // Useful when all you need is a one-time memcpy
    //
    virtual void on_load_ppu(nes_ppu &ppu) {}

    //
    // Returns various mapper related information
    //
    virtual void get_info(nes_mapper_info &) = 0;

    //
    // Write mapper register in the given address
    // Caller should check if addr is in range of register first
    //
    //virtual void write_reg(uint16_t addr, uint8_t val) {};

    virtual ~nes_mapper(){};
};

class nes_mapper_nrom : public nes_mapper
{
public :
    nes_mapper_nrom(std::shared_ptr<std::vector<uint8_t>> &prg_rom, std::shared_ptr<std::vector<uint8_t>> &chr_rom, bool vertical_mirroring)
            :_prg_rom(prg_rom), _chr_rom(chr_rom), _vertical_mirroring(vertical_mirroring)
    {

    }

    void on_load_ram(nes_memory &mem);
    void on_load_ppu(nes_ppu &ppu);
    void get_info(nes_mapper_info &info);

private :
    std::shared_ptr<std::vector<uint8_t>> _prg_rom;
    std::shared_ptr<std::vector<uint8_t>> _chr_rom;
    bool _vertical_mirroring;
};

#define FLAG_6_USE_VERTICAL_MIRRORING_MASK 0x1
#define FLAG_6_HAS_BATTERY_BACKED_PRG_RAM_MASK 0x2
#define FLAG_6_HAS_TRAINER_MASK  0x4
#define FLAG_6_USE_FOUR_SCREEN_VRAM_MASK 0x8
#define FLAG_6_LO_MAPPER_NUMBER_MASK 0xf0
#define FLAG_7_HI_MAPPER_NUMBER_MASK 0xf0

class nes_rom_loader
{
public :
    struct ines_header
    {
        uint8_t magic[4];       // 0x4E, 0x45, 0x53, 0x1A
        uint8_t prg_size;       // PRG ROM in 16K
        uint8_t chr_size;       // CHR ROM in 8K, 0 -> using CHR RAM
        uint8_t flag6;
        uint8_t flag7;
        uint8_t prg_ram_size;   // PRG RAM in 8K
        uint8_t flag9;
        uint8_t flag10;         // unofficial
        uint8_t reserved[5];    // reserved
    };

    // Loads a NES ROM file
    // Automatically detects format according to extension and header
    // Returns a nes_mapper instance which has all necessary memory mapped
    static std::shared_ptr<nes_mapper> load_from(const char *path)

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

        int mapper_id = ((header.flag6 & FLAG_6_LO_MAPPER_NUMBER_MASK) >> 4) + ((header.flag7 & FLAG_7_HI_MAPPER_NUMBER_MASK));

        int prg_rom_size = header.prg_size * 0x4000;    // 16KB
        int chr_rom_size = header.chr_size * 0x2000;    // 8KB

        auto prg_rom = std::make_shared<std::vector<uint8_t>>(prg_rom_size);
        auto chr_rom = std::make_shared<std::vector<uint8_t>>(chr_rom_size);

        file.read((char *)prg_rom->data(), prg_rom->size());
        file.read((char *)chr_rom->data(), chr_rom->size());

        std::shared_ptr<nes_mapper> mapper;

        // @TODO - Change this into a mapper factory class
        switch (mapper_id)
        {
            case 0: mapper = std::make_shared<nes_mapper_nrom>(prg_rom, chr_rom, vertical_mirroring); break;
            default:
                assert(!"Unsupported mapper id");
        }

        file.close();

        return mapper;
    }
};
