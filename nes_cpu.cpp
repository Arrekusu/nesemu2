#include <iostream>
#include "nes_cpu.h"
#include "nes_system.h"
#include "opcodes.h"

void nes_cpu::NMI()
{
    // As per neswiki: NMI should set I(bit 5) but clear B(bit 4)
    // http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    push_word(registers.PC);
    push_byte(registers.P | 0x20);

    //step_cpu(7);
    registers.PC = peek_word(NMI_HANDLER);
}

void nes_cpu::OAMDMA()
{
    system->getPpu()->oam_dma(_dma_addr);

    // The entire DMA takes 513 or 514 cycles
    // http://wiki.nesdev.com/w/index.php/PPU_registers#OAMDMA
    //if (_cycle % 2 == nes_cpu_cycle_t(0))
      //  step_cpu(514);
    //else
      //  step_cpu(513);
}

void nes_cpu::init(nes_system* system) {
    this->system = system;
    memory = system->getMem();
    _nmi_pending = false;
    _dma_pending = false;

    // @TODO - Simulate full power-on state
    // http://wiki.nesdev.com/w/index.php/CPU_power_up_state
    registers.P = 0x24;          // @TODO - Should be 0x34 - but temporarily set to 0x24 to match nintendulator baseline
    registers.A = registers.X = registers.Y = 0;
    registers.SP = 0xfd;
    registers.PC = 0;
    _cycle = nes_cycle_t(0);
}

uint8_t nes_cpu::read_byte(uint16_t addr){
    return memory->get_byte(addr);
}

uint16_t nes_cpu::read_word(uint16_t addr){
    return memory->get_word(addr);
}

uint8_t nes_cpu::read_next_byte() {
    return read_byte(registers.PC++);
}

uint16_t nes_cpu::read_next_word() {
    uint16_t val = read_word(registers.PC);
    registers.PC += 2;
    return val;
}

void nes_cpu::step_to(nes_cycle_t new_count)
{
    while (_cycle < new_count && !system->stop_requested())
        exec_one_instruction();
}

void nes_cpu::nes_log(uint8_t op, opEntry* opEntry, uint16_t count){
    //C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD CYC:  0
    printf("%-4X %-2X %-2X %-2X %-3s %-28X                      A:%X X:%X Y:%X P:%X SP:%X CYC: %d\n", count, op, operand.value & 0xFF, (operand.value & 0xFF00) >> 8, opEntry->name, operand.value, registers.A, registers.X, registers.Y, registers.P, registers.SP, _cycle);
    fflush(stdout); // flushes the stdout buffer
}

void nes_cpu::exec_one_instruction(){
    uint16_t originalCount = registers.PC;
    if (_nmi_pending)
    {
        // generate NMI
        NMI();

        _nmi_pending = false;
    }
    else if (_dma_pending)
    {
        OAMDMA();

        _dma_pending = false;
    }
    else {
        auto op_code = read_next_byte();

        struct opEntry *opStruct = &opsTable[op_code];

        switch (op_code) {
            //ADC
            case 0x69:
                ADC_IMD(get_operand(nes_addr_mode::IMD));
                break;
            case 0x65:
                ADC(get_operand(nes_addr_mode::ZP));
                break;
            case 0x75:
                ADC(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x6D:
                ADC(get_operand(nes_addr_mode::ABS));
                break;
            case 0x7D:
                ADC(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x79:
                ADC(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x61:
                ADC(get_operand(nes_addr_mode::INDX));
                break;
            case 0x71:
                ADC(get_operand(nes_addr_mode::INDY));
                break;

                //AND
            case 0x29:
                AND(get_operand(nes_addr_mode::IMD));
                break;
            case 0x25:
                AND_IND(get_operand(nes_addr_mode::ZP));
                break;
            case 0x35:
                AND_IND(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x2D:
                AND_IND(get_operand(nes_addr_mode::ABS));
                break;
            case 0x3D:
                AND_IND(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x39:
                AND_IND(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x21:
                AND_IND(get_operand(nes_addr_mode::INDX));
                break;
            case 0x31:
                AND_IND(get_operand(nes_addr_mode::INDY));
                break;

                //ASL
            case 0x0A:
                ASL_ACC(get_operand(nes_addr_mode::ACC));
                break;
            case 0x06:
                ASL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x16:
                ASL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x0E:
                ASL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x1E:
                ASL(get_operand(nes_addr_mode::ABSX));
                break;

                //BCC
            case 0x90:
                BCC();
                break;

                //BCS
            case 0xB0:
                BCS();
                break;

                //BEQ
            case 0xF0:
                BEQ();
                break;

                //BIT
            case 0x24:
                BIT(get_operand(nes_addr_mode::ZP));
                break;
            case 0x2C:
                BIT(get_operand(nes_addr_mode::ABS));
                break;

                //BMI
            case 0x30:
                BMI();
                break;

                //BNE
            case 0xD0:
                BNE();
                break;

                //BPL
            case 0x10:
                BPL();
                break;

                //BVC
            case 0x50:
                BVC();
                break;

                //BVS
            case 0x70:
                BVS();
                break;

                //CMP
            case 0xC9:
                CMP(get_operand(nes_addr_mode::IMD));
                break;
            case 0xC5:
                CMP_IND(get_operand(nes_addr_mode::ZP));
                break;
            case 0xD5:
                CMP_IND(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xCD:
                CMP_IND(get_operand(nes_addr_mode::ABS));
                break;
            case 0xDD:
                CMP_IND(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xD9:
                CMP_IND(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xC1:
                CMP_IND(get_operand(nes_addr_mode::INDX));
                break;
            case 0xD1:
                CMP_IND(get_operand(nes_addr_mode::INDY));
                break;

                //CPX
            case 0xE0:
                CPX_IMD(get_operand(nes_addr_mode::IMD));
                break;
            case 0xE4:
                CPX(get_operand(nes_addr_mode::ZP));
                break;
            case 0xEC:
                CPX(get_operand(nes_addr_mode::ABS));
                break;

                //CPY
            case 0xC0:
                CPY_IMD(get_operand(nes_addr_mode::IMD));
                break;
            case 0xC4:
                CPY(get_operand(nes_addr_mode::ZP));
                break;
            case 0xCC:
                CPY(get_operand(nes_addr_mode::ABS));
                break;

                //DEC
            case 0xC6:
                DEC(get_operand(nes_addr_mode::ZP));
                break;
            case 0xD6:
                DEC(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xCE:
                DEC(get_operand(nes_addr_mode::ABS));
                break;
            case 0xDE:
                DEC(get_operand(nes_addr_mode::ABSX));
                break;

                //EOR
            case 0x49:
                EOR(get_operand(nes_addr_mode::IMD));
                break;
            case 0x45:
                EOR_IND(get_operand(nes_addr_mode::ZP));
                break;
            case 0x55:
                EOR_IND(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x4D:
                EOR_IND(get_operand(nes_addr_mode::ABS));
                break;
            case 0x5D:
                EOR_IND(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x59:
                EOR_IND(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x41:
                EOR_IND(get_operand(nes_addr_mode::INDX));
                break;
            case 0x51:
                EOR_IND(get_operand(nes_addr_mode::INDY));
                break;

                //INC
            case 0xE6:
                INC(get_operand(nes_addr_mode::ZP));
                break;
            case 0xF6:
                INC(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xEE:
                INC(get_operand(nes_addr_mode::ABS));
                break;
            case 0xFE:
                INC(get_operand(nes_addr_mode::ABSX));
                break;

                //JMP
            case 0x4C:
                JMP(get_operand(nes_addr_mode::ABS));
                break;
            case 0x6C:
                JMP(get_operand(nes_addr_mode::IND_JMP));
                break;

                //JSR
            case 0x20:
                JSR(get_operand(nes_addr_mode::ABS));
                break;

                //LDA
            case 0xA9:
                LDA(get_operand(nes_addr_mode::IMD));
                break;
            case 0xA5:
                LDA_ABS(get_operand(nes_addr_mode::ZP));
                break;
            case 0xB5:
                LDA_ABS(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xAD:
                LDA_ABS(get_operand(nes_addr_mode::ABS));
                break;
            case 0xBD:
                LDA_ABS(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xB9:
                LDA_ABS(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xA1:
                LDA_ABS(get_operand(nes_addr_mode::INDX));
                break;
            case 0xB1:
                LDA_ABS(get_operand(nes_addr_mode::INDY));
                break;

                //LDX
            case 0xA2:
                LDX(get_operand(nes_addr_mode::IMD));
                break;
            case 0xA6:
                LDX_ABS(get_operand(nes_addr_mode::ZP));
                break;
            case 0xB6:
                LDX_ABS(get_operand(nes_addr_mode::ZPY));
                break;
            case 0xAE:
                LDX_ABS(get_operand(nes_addr_mode::ABS));
                break;
            case 0xBE:
                LDX_ABS(get_operand(nes_addr_mode::ABSY));
                break;

                //LDY
            case 0xA0:
                LDY(get_operand(nes_addr_mode::IMD));
                break;
            case 0xA4:
                LDY_ABS(get_operand(nes_addr_mode::ZP));
                break;
            case 0xB4:
                LDY_ABS(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xAC:
                LDY_ABS(get_operand(nes_addr_mode::ABS));
                break;
            case 0xBC:
                LDY_ABS(get_operand(nes_addr_mode::ABSX));
                break;

                //LSR
            case 0x4A:
                LSR_ACC(get_operand(nes_addr_mode::ACC));
                break;
            case 0x46:
                LSR(get_operand(nes_addr_mode::ZP));
                break;
            case 0x56:
                LSR(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x4E:
                LSR(get_operand(nes_addr_mode::ABS));
                break;
            case 0x5E:
                LSR(get_operand(nes_addr_mode::ABSX));
                break;

                //ORA
            case 0x09:
                ORA(get_operand(nes_addr_mode::IMD));
                break;
            case 0x05:
                ORA_IND(get_operand(nes_addr_mode::ZP));
                break;
            case 0x15:
                ORA_IND(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x0D:
                ORA_IND(get_operand(nes_addr_mode::ABS));
                break;
            case 0x1D:
                ORA_IND(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x19:
                ORA_IND(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x01:
                ORA_IND(get_operand(nes_addr_mode::INDX));
                break;
            case 0x11:
                ORA_IND(get_operand(nes_addr_mode::INDY));
                break;

                //ROL
            case 0x2A:
                ROL_ACC(get_operand(nes_addr_mode::ACC));
                break;
            case 0x26:
                ROL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x36:
                ROL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x2E:
                ROL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x3E:
                ROL(get_operand(nes_addr_mode::ABSX));
                break;

                //ROR
            case 0x6A:
                ROR_ACC(get_operand(nes_addr_mode::ACC));
                break;
            case 0x66:
                ROR(get_operand(nes_addr_mode::ZP));
                break;
            case 0x76:
                ROR(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x6E:
                ROR(get_operand(nes_addr_mode::ABS));
                break;
            case 0x7E:
                ROR(get_operand(nes_addr_mode::ABSX));
                break;

                //SBC
            case 0xE9:
                SBC_IMD(get_operand(nes_addr_mode::IMD));
                break;
            case 0xE5:
                SBC(get_operand(nes_addr_mode::ZP));
                break;
            case 0xF5:
                SBC(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xED:
                SBC(get_operand(nes_addr_mode::ABS));
                break;
            case 0xFD:
                SBC(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xF9:
                SBC(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xE1:
                SBC(get_operand(nes_addr_mode::INDX));
                break;
            case 0xF1:
                SBC(get_operand(nes_addr_mode::INDY));
                break;

                //STA
            case 0x85:
                STA(get_operand(nes_addr_mode::ZP));
                break;
            case 0x95:
                STA(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x8D:
                STA(get_operand(nes_addr_mode::ABS));
                break;
            case 0x9D:
                STA(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x99:
                STA(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x81:
                STA(get_operand(nes_addr_mode::INDX));
                break;
            case 0x91:
                STA(get_operand(nes_addr_mode::INDY));
                break;

                //STX
            case 0x86:
                STX(get_operand(nes_addr_mode::ZP));
                break;
            case 0x96:
                STX(get_operand(nes_addr_mode::ZPY));
                break;
            case 0x8E:
                STX(get_operand(nes_addr_mode::ABS));
                break;

                //STY
            case 0x84:
                STY(get_operand(nes_addr_mode::ZP));
                break;
            case 0x94:
                STY(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x8C:
                STY(get_operand(nes_addr_mode::ABS));
                break;

                //IMPLIED ONES
            case 0x00:
                BRK();
                break;
            case 0x18:
                CLC();
                break;
            case 0xD8:
                CLD();
                break;
            case 0x58:
                CLI();
                break;
            case 0xB8:
                CLV();
                break;
            case 0xCA:
                DEX();
                break;
            case 0x88:
                DEY();
                break;
            case 0xE8:
                INX();
                break;
            case 0xC8:
                INY();
                break;
            case 0xEA:
                NOP();
                break;
            case 0x48:
                PHA();
                break;
            case 0x08:
                PHP();
                break;
            case 0x68:
                PLA();
                break;
            case 0x28:
                PLP();
                break;
            case 0x40:
                RTI();
                break;
            case 0x60:
                RTS();
                break;
            case 0x38:
                SEC();
                break;
            case 0xF8:
                SED();
                break;
            case 0x78:
                SEI();
                break;
            case 0xAA:
                TAX();
                break;
            case 0xA8:
                TAY();
                break;
            case 0xBA:
                TSX();
                break;
            case 0x8A:
                TXA();
                break;
            case 0x9A:
                TXS();
                break;
            case 0x98:
                TYA();
                break;

                //===============================================================================
                // Unofficial instructions
                //===============================================================================
            case 0x80:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x04:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x44:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x64:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x0c:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x14:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x34:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x54:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x74:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xd4:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xf4:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x1c:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x3c:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x5c:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x7c:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xdc:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xfc:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x89:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x82:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0xc2:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0xe2:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x1a:
                UNOFFICIAL();
                break;
            case 0x3a:
                UNOFFICIAL();
                break;
            case 0x5a:
                UNOFFICIAL();
                break;
            case 0x7a:
                UNOFFICIAL();
                break;
            case 0xda:
                UNOFFICIAL();
                break;
            case 0xfa:
                UNOFFICIAL();
                break;
            case 0x03:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0x07:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x0b:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x0f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x13:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0x17:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x1b:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x1f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x23:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0x27:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x2b:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x2f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x33:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0x37:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x3b:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x3f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x43:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0x47:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x4b:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x4f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x53:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0x57:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x5b:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x5f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x63:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0x67:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x6b:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x6f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x73:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0x77:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0x7b:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x7f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0x83:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0x87:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0x8b:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0x8f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0x93:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0x97:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPY));
                break;
            case 0x9b:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0x9f:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xa3:
                LAX(get_operand(nes_addr_mode::INDX));
                break;
            case 0xa7:
                LAX(get_operand(nes_addr_mode::ZP));
                break;
            case 0xab:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0xaf:
                LAX(get_operand(nes_addr_mode::ABS));
                break;
            case 0xb3:
                LAX(get_operand(nes_addr_mode::INDY));
                break;
            case 0xb7:
                LAX(get_operand(nes_addr_mode::ZPY));
                break;
            case 0xbb:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPY));
                break;
            case 0xbf:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xc3:
                UNOFFICIAL(get_operand(nes_addr_mode::INDX));
                break;
            case 0xc7:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0xcb:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0xcf:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0xd3:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xd7:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xdb:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xdf:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;
            case 0xe3:
                ISC(get_operand(nes_addr_mode::INDX));
                break;
            case 0xe7:
                UNOFFICIAL(get_operand(nes_addr_mode::ZP));
                break;
            case 0xeb:
                UNOFFICIAL(get_operand(nes_addr_mode::IMD));
                break;
            case 0xef:
                UNOFFICIAL(get_operand(nes_addr_mode::ABS));
                break;
            case 0xf3:
                UNOFFICIAL(get_operand(nes_addr_mode::INDY));
                break;
            case 0xf7:
                UNOFFICIAL(get_operand(nes_addr_mode::ZPX));
                break;
            case 0xfb:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSY));
                break;
            case 0xff:
                UNOFFICIAL(get_operand(nes_addr_mode::ABSX));
                break;

            default:
                std::cout << "Unknown instruction: " << op_code << std::endl;
                assert(false);
        }

        //nes_log(op_code, opStruct, originalCount);
        _cycle += nes_cycle_t(opStruct->cycles);
        operand = {};
    }
}

void nes_cpu::calc_alu_flag(uint8_t value)
{
    set_zero_flag(value == 0);
    set_negative_flag(value & 0x80);
}

void nes_cpu::ADC(operand_t operand) {
    uint8_t value = read_byte(operand.value);

    uint8_t oldValue = registers.A;
    uint8_t newValue = oldValue + value;

    bool bit7_overflow = false;
    if (newValue < oldValue) bit7_overflow = true;

    uint8_t old_val2 = newValue;
    newValue += get_carry();
    if (newValue < old_val2) bit7_overflow = true;

    registers.A = newValue;

    set_overflow_flag(is_sign_overflow(oldValue, value, newValue));
    set_carry_flag(bit7_overflow);
    calc_alu_flag(registers.A);
}

void nes_cpu::ADC_IMD(operand_t operand) {
    uint8_t value = operand.value;

    uint8_t oldValue = registers.A;
    uint8_t newValue = oldValue + value;

    bool bit7_overflow = false;
    if (newValue < oldValue) bit7_overflow = true;

    uint8_t old_val2 = newValue;
    newValue += get_carry();
    if (newValue < old_val2) bit7_overflow = true;

    registers.A = newValue;

    set_overflow_flag(is_sign_overflow(oldValue, value, newValue));
    set_carry_flag(bit7_overflow);
    calc_alu_flag(registers.A);
}

void nes_cpu::AND(operand_t operand) {
    uint8_t value = operand.value;
    registers.A &= value;
    calc_alu_flag(registers.A);
}

void nes_cpu::AND_IND(operand_t operand) {
    uint8_t value = read_byte(operand.value);
    registers.A &= value;
    calc_alu_flag(registers.A);
}

void nes_cpu::BRK() {
    system->stop();
}

operand_t nes_cpu::get_operand(nes_addr_mode mode) {
    if (mode == nes_addr_mode::ACC){
        return {registers.A, operand_kind::ACCUMULATOR, false};
    }
    if (mode == nes_addr_mode::IMD){
        return {read_next_byte(), operand_kind::IMMEDIATE, false};
    }

    uint16_t value = 0;
    bool page_crossed = false;
    if (mode == nes_addr_mode::ZP){
        value = read_next_byte();
    } else if (mode == nes_addr_mode::ZPX){
        value = (read_next_byte() + registers.X) & 0xFF;
    } else if (mode == nes_addr_mode::ZPY) {
        value = (read_next_byte() + registers.Y) & 0xFF;
    } else if (mode == nes_addr_mode::IND_JMP) {
        // Indirect
        uint16_t addr = read_next_word();
        if ((addr & 0xff) == 0xff)
        {
            // Account for JMP hardware bug
            // http://wiki.nesdev.com/w/index.php/Errata
            value = read_byte(addr) + (uint16_t(read_byte(addr & 0xff00)) << 8);
        }
        else
        {
            value = read_word(addr);
        }
    } else if (mode == nes_addr_mode::IND){
        uint16_t addr = read_next_word();
        if ((value & 0xFF) == 0xFF){
            value = peek(addr) + (uint16_t(peek(addr & 0xff00)) << 8);
        } else {
            value = peek_word(addr);
        }
    } else if (mode == nes_addr_mode::ABS){
        value = read_next_word();
    } else if (mode == nes_addr_mode::ABSX){
        uint16_t addr = read_next_word();
        uint16_t new_addr = addr + registers.X;
        page_crossed = ((addr & 0xff00) != (new_addr & 0xff00));
        value = new_addr;
    } else if (mode == nes_addr_mode::ABSY){
        uint16_t addr = read_next_word();
        uint16_t new_addr = addr + registers.Y;
        page_crossed = ((addr & 0xff00) != (new_addr & 0xff00));
        value = new_addr;
    } else if (mode == nes_addr_mode::INDX) {
        uint8_t addr = read_next_byte();
        value = peek((addr + registers.X) & 0xff) + (uint16_t(peek((addr + registers.X + 1) & 0xff)) << 8);
    } else if (mode == nes_addr_mode::INDY){
        uint8_t arg_addr = read_next_byte();
        uint16_t addr = peek(arg_addr) + (uint16_t(peek((arg_addr + 1) & 0xff)) << 8);
        uint16_t new_addr = addr + registers.Y;
        page_crossed = ((addr & 0xff00) != (new_addr & 0xff00));
        value = new_addr;
    } else {
        assert(false);
    }
    operand = {value, operand_kind::ADDRESS, page_crossed};
    return operand;
}

void nes_cpu::INC(operand_t operand) {
    uint16_t addr = operand.value;
    uint8_t new_val = peek(addr) + 1;
    // flags
    set_byte(addr, new_val);
    calc_alu_flag(new_val);
}

void nes_cpu::ASL_ACC(operand_t operand) {
    uint8_t val = operand.value;
    uint8_t new_val = val << 1;
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x80);

    // @DOCBUG:
    // http://obelisk.me.uk/6502/reference.html#ASL incorrectly states ASL detects A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::ASL(operand_t operand) {
    uint8_t val = read_byte(operand.value);
    uint8_t new_val = val << 1;
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x80);

    // @DOCBUG:
    // http://obelisk.me.uk/6502/reference.html#ASL incorrectly states ASL detects A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::branch(bool cond)
{
    int8_t rel = (int8_t) read_next_byte();
    operand = {rel, operand_kind::ADDRESS, false};
    if (cond)
    {
        registers.PC += rel;
        /*if (rel == -2 && _stop_at_infinite_loop)
        {
            _system->stop();
            _stop_at_infinite_loop = false;
        }*/
    }
}

void nes_cpu::BCC() {
    branch(!get_carry());
}

void nes_cpu::BCS() {
    branch(get_carry());
}

void nes_cpu::BEQ() {
    branch(is_zero());
}

void nes_cpu::BIT(operand_t operand) {
    uint8_t val = read_byte(operand.value);
    uint8_t new_val = val & registers.A;

    // flags
    set_zero_flag(new_val == 0);
    set_overflow_flag(val & 0x40);
    set_negative_flag(val & 0x80);
}

void nes_cpu::BMI() {
    branch(is_negative());
}

void nes_cpu::BNE() {
    branch(!is_zero());
}

void nes_cpu::BPL() {
    branch(!is_negative());
}

void nes_cpu::BVC() {
    branch(!is_overflow());
}

void nes_cpu::BVS() {
    branch(is_overflow());
}

void nes_cpu::CLC() {
    set_carry_flag(false);
}

void nes_cpu::CLD() {
    set_decimal_flag(false);
}

void nes_cpu::CLI() {
    set_interrupt_flag(false);
}

void nes_cpu::CLV() {
    set_overflow_flag(false);
}

void nes_cpu::CMP(operand_t operand) {
    uint8_t val = operand.value;

    // flags
    uint8_t diff = registers.A - val;

    set_carry_flag(registers.A >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::CMP_IND(operand_t operand) {
    uint8_t val = read_byte(operand.value);

    // flags
    uint8_t diff = registers.A - val;

    set_carry_flag(registers.A >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::CPX_IMD(operand_t operand) {
    uint8_t val = operand.value;

    // flags
    uint8_t diff = registers.X - val;

    set_carry_flag(registers.X >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::CPX(operand_t operand) {
    uint8_t val = read_byte(operand.value);

    // flags
    uint8_t diff = registers.X - val;

    set_carry_flag(registers.X >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::CPY_IMD(operand_t operand) {
    uint8_t val = operand.value;

    uint8_t diff = (registers.Y - val);

    set_carry_flag(registers.Y >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::CPY(operand_t operand) {
    uint8_t val = read_byte(operand.value);

    uint8_t diff = (registers.Y - val);

    set_carry_flag(registers.Y >= val);
    set_zero_flag(diff == 0);
    set_negative_flag(diff & 0x80);
}

void nes_cpu::DEC(operand_t operand) {
    uint16_t addr = operand.value;
    uint8_t new_val = peek(addr) - 1;
    set_byte(addr, new_val);

    calc_alu_flag(new_val);
}

void nes_cpu::DEX() {
    registers.X--;
    calc_alu_flag(registers.X);
}

void nes_cpu::DEY() {
    registers.Y--;
    calc_alu_flag(registers.Y);
}

void nes_cpu::EOR(operand_t operand) {
    uint8_t val = operand.value;

    registers.A ^= val;

    // flags
    calc_alu_flag(registers.A);
}

void nes_cpu::EOR_IND(operand_t operand) {
    uint8_t val = read_byte(operand.value);

    registers.A ^= val;

    // flags
    calc_alu_flag(registers.A);
}

void nes_cpu::INX() {
    registers.X++;
    calc_alu_flag(registers.X);
}

void nes_cpu::INY() {
    registers.Y++;
    calc_alu_flag(registers.Y);
}

void nes_cpu::JMP(operand_t operand) {
    registers.PC = operand.value;;
}

void nes_cpu::JSR(operand_t operand) {
    push_word(registers.PC - 1);
    registers.PC = operand.value;
}

void nes_cpu::LDA(operand_t operand){
    registers.A = operand.value;
    calc_alu_flag(registers.A);
}

void nes_cpu::LDA_ABS(operand_t operand){
    registers.A = read_byte(operand.value);

    calc_alu_flag(registers.A);
}

void nes_cpu::LDX(operand_t operand) {
    registers.X = operand.value;
    calc_alu_flag(registers.X);
}

void nes_cpu::LDX_ABS(operand_t operand) {
    registers.X = read_byte(operand.value);
    calc_alu_flag(registers.X);
}

void nes_cpu::LDY(operand_t operand) {
    registers.Y = operand.value;
    calc_alu_flag(registers.Y);
}

void nes_cpu::LDY_ABS(operand_t operand) {
    registers.Y = read_byte(operand.value);
    calc_alu_flag(registers.Y);
}

void nes_cpu::LSR_ACC(operand_t operand) {
    uint8_t val = operand.value;
    uint8_t new_val = (val >> 1);
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x1);

    // @DOCBUG:
    // http://obelisk.me.uk/6502/reference.html#LSR incorrectly states ASL detects A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::LSR(operand_t operand) {
    uint8_t val = read_byte(operand.value);
    uint8_t new_val = (val >> 1);
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x1);

    // @DOCBUG:
    // http://obelisk.me.uk/6502/reference.html#LSR incorrectly states ASL detects A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::NOP() {
    // For effective NOP (op codes that are "effectively" no-op but not the real NOP 0xea)
    // We always needed to decode the parameter
    /*if (addr_mode != nes_addr_mode::nes_addr_mode_imp)
    {
        operand_t op = decode_operand(addr_mode);
        step_cpu(get_cpu_cycle(op, addr_mode));
    }
    else
    {
        step_cpu(nes_cpu_cycle_t(2));
    }*/
}

void nes_cpu::ORA(operand_t operand) {
    uint8_t val = operand.value;

    registers.A |= val;

    calc_alu_flag(registers.A);
}

void nes_cpu::ORA_IND(operand_t operand) {
    uint8_t val = read_byte(operand.value);

    registers.A |= val;

    calc_alu_flag(registers.A);
}

void nes_cpu::PHA() {
    push_byte(registers.A);
}

void nes_cpu::PHP() {
    // http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    // Set bit 5 and 4 to 1 when copy status into from PHP
    push_byte(registers.P | 0x30);
}

void nes_cpu::PLA() {
    registers.A = pop_byte();
    calc_alu_flag(registers.A);
}

void nes_cpu::PLP() {
    // http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    // Bit 5 and 4 are ignored when pulled from stack - which means they are preserved
    // @TODO - Nintendulator actually always sets bit 5, not sure which one is correct
    // I'm setting bit 5 to make testing easier
    registers.P = (pop_byte() & 0xef) | (registers.P & 0x10) | 0x20;
}

void nes_cpu::ROL_ACC(operand_t operand) {
    uint8_t val = operand.value;
    uint8_t new_val = (val << 1) | get_carry();
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x80);
    // @DOCBUG
    // http://obelisk.me.uk/6502/reference.html#ROL incorrectly states zero is set if A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::ROL(operand_t operand) {
    uint8_t val = read_byte(operand.value);
    uint8_t new_val = (val << 1) | get_carry();
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x80);
    // @DOCBUG
    // http://obelisk.me.uk/6502/reference.html#ROL incorrectly states zero is set if A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::ROR_ACC(operand_t operand) {
    uint8_t val = operand.value;
    uint8_t new_val = (val >> 1) | (get_carry() << 7);
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x1);

    // @DOCBUG
    // http://obelisk.me.uk/6502/reference.html#ROR incorrectly states zero is set if A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::ROR(operand_t operand) {
    uint8_t val = read_byte(operand.value);
    uint8_t new_val = (val >> 1) | (get_carry() << 7);
    write_operand(operand, new_val);

    // flags
    set_carry_flag(val & 0x1);

    // @DOCBUG
    // http://obelisk.me.uk/6502/reference.html#ROR incorrectly states zero is set if A == 0
    set_zero_flag(new_val == 0);
    set_negative_flag(new_val & 0x80);
}

void nes_cpu::RTI() {
    PLP();

    uint16_t addr = pop_word();
    registers.PC = addr;
}

void nes_cpu::RTS() {
    uint16_t addr = pop_word() + 1;
    registers.PC = addr;
}

void nes_cpu::SBC(operand_t operand) {
    operand.value = ~read_byte(operand.value);
    ADC_IMD(operand);
}

void nes_cpu::SBC_IMD(operand_t operand) {
    operand.value = ~operand.value;
    ADC_IMD(operand);
}

void nes_cpu::SEC() {
    set_carry_flag(true);
}

void nes_cpu::SED() {
    set_decimal_flag(true);
}

void nes_cpu::SEI() {
    set_interrupt_flag(true);
}

void nes_cpu::STA(operand_t operand) {
    set_byte(operand.value, registers.A);
}

void nes_cpu::STX(operand_t operand) {
    set_byte(operand.value, registers.X);
}

void nes_cpu::STY(operand_t operand) {
    set_byte(operand.value, registers.Y);
}

void nes_cpu::TAX() {
    registers.X = registers.A;
    calc_alu_flag(registers.X);
}

void nes_cpu::TAY() {
    registers.Y = registers.A;
    calc_alu_flag(registers.Y);
}

void nes_cpu::TSX() {
    registers.X = registers.SP;
    calc_alu_flag(registers.X);
}

void nes_cpu::TXA() {
    registers.A = registers.X;
    calc_alu_flag(registers.A);
}

void nes_cpu::TXS() {
    registers.SP = registers.X;
}

void nes_cpu::TYA() {
    registers.A = registers.Y;
    calc_alu_flag(registers.A);
}

void nes_cpu::UNOFFICIAL(operand_t operand) {

}

void nes_cpu::UNOFFICIAL() {

}

void nes_cpu::LAX(operand_t operand) {
    // LDA + TAX
    uint8_t val = read_byte(operand.value);
    registers.X = registers.A = val;

    // flags
    calc_alu_flag(registers.X);
}

// ISC - INC value then SBC value
void nes_cpu::ISC(operand_t operand)
{
    operand.value++;
    // SBC
    SBC(operand);
}
