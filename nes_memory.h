#pragma once

#include <vector>
#include <cstdint>
#include <assert.h>
#include <cstring>
#include <cerrno>
#include "nes_mapper.h"
#include "nes_input.h"

#define RAM_SIZE 0x10000

class nes_system;

class nes_memory {
public:
    std::vector<uint8_t> memory;
    std::shared_ptr<nes_mapper> _mapper;
    nes_ppu *_ppu;
    nes_input *_input;
    nes_mapper_info _mapper_info;
    nes_memory(){
        memory.reserve(RAM_SIZE);
    }

    void init(nes_system* system);

    static error_t memcpy_s(void *dest, size_t dest_size, const void *src, size_t count)
    {
        if (dest_size < count)
            return ERANGE;

        memcpy(dest, src, count);

        return 0;
    }

    void redirect_addr(uint16_t &addr)
    {
        if ((addr & 0xE000) == 0)
        {
            // map 0x0000~0x07ff 4 times until 0x1fff
            addr &= 0x7ff;
        }
        else if ((addr & 0xE000) == 0x2000)
        {
            // map 0x2000~0x2008 every 8 bytes until 0x3fff
            addr &= 0x2007;
        }
    }

    uint16_t get_word(uint16_t addr)
    {
        redirect_addr(addr);
        return get_byte(addr) + (uint16_t(get_byte(addr + 1)) << 8);
    }

    void set_bytes(uint16_t addr, uint8_t* data, std::size_t size){
        assert(size + addr <= RAM_SIZE);
        redirect_addr(addr);
        memcpy_s(&memory[0] + addr, RAM_SIZE - addr, data, size);
    }

    uint8_t read_io_reg(uint16_t addr);
    void write_io_reg(uint16_t addr, uint8_t val);

    bool is_io_reg(uint16_t addr)
    {
        // $2000~$2007
        if ((addr & 0xfff8) == 0x2000)
            return true;
        // $4000~401f
        if ((addr & 0xffe0) == 0x4000)
            return true;
        return false;
    }

    uint8_t get_byte(uint16_t addr)
    {
        redirect_addr(addr);
        if (is_io_reg(addr))
            return read_io_reg(addr);

        return memory[addr];
    }

    void get_bytes(uint8_t *dest, uint16_t dest_size, uint16_t src_addr, size_t src_size)
    {
        assert(src_addr + src_size <= RAM_SIZE);
        redirect_addr(src_addr);
        memcpy_s(dest, dest_size, & memory[0] + src_addr, src_size);
    }

    void set_byte(uint16_t addr, uint8_t val)
    {
        redirect_addr(addr);
        if (is_io_reg(addr))
        {
            write_io_reg(addr, val);
            return;
        }

        if (_mapper && (_mapper_info.flags & nes_mapper_flags_has_registers))
        {
            if (addr >= _mapper_info.reg_start && addr <= _mapper_info.reg_end)
            {
                //_mapper->write_reg(addr, val);
                return;
            }
        }

        memory[addr] = val;
    }

    void load_mapper(std::shared_ptr<nes_mapper> &mapper);
};

