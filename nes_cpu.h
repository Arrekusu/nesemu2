#pragma once

#include "nes_memory.h"
#include "nes_mapper.h"
#include "nes_cycle.h"

//Carry: 1 if last addition or shift resulted in a carry, or if last subtraction resulted in no borrow
#define PROCESSOR_STATUS_CARRY_MASK 0x1

//Zero: 1 if last operation resulted in a 0 value
#define PROCESSOR_STATUS_ZERO_MASK 0x2

//Interrupt: Interrupt inhibit (0: /IRQ and /NMI get through; 1: only /NMI gets through)
#define PROCESSOR_STATUS_INTERRUPT_MASK 0x4

//Decimal: 1 to make ADC and SBC use binary-coded decimal arithmetic (ignored on second-source 6502 like that in the NES)
#define PROCESSOR_STATUS_ADC_MASK 0x8

//No effect, used by the stack copy
#define PROCESSOR_STATUS_B_MASK 0x10
#define PROCESSOR_STATUS_A_MASK 0x20

class nes_system;

//Overflow: 1 if last ADC or SBC resulted in signed overflow, or D6 from last BIT
#define PROCESSOR_STATUS_OVERFLOW_MASK 0x40

//Negative: Set to bit 7 of the last operation
#define PROCESSOR_STATUS_NEGATIVE_MASK 0x80

// Vertical blanking interrupt handler
#define NMI_HANDLER     0xfffa

// Reset handler - the "main" for NES roms
#define RESET_HANDLER   0xfffc

#define IRQ_HANDLER     0xfffe

enum class nes_addr_mode {
    ABS,
    ABSX,
    ABSY,
    ACC,
    IMD,
    IMP,
    IND,
    IND_JMP,
    INDX,
    INDY,
    REL,
    UNKNOWN,
    ZP,
    ZPX,
    ZPY,
};

struct opEntry {
    const char *name;
    nes_addr_mode mode;
    int bytes;
    int modify;
    int cycles;
};

struct cpu_registers {
    uint16_t PC; // program counter
    uint8_t SP; // stack pointer
    uint8_t A; // accumulator
    uint8_t X; // x register
    uint8_t Y; // y register
    uint8_t P; // processor status
};

enum class operand_kind{
    ACCUMULATOR,
    IMMEDIATE,
    ADDRESS
};

struct operand_t{
    uint16_t value;
    operand_kind kind;
    bool page_crossed;
};

class nes_cpu {
private:
    cpu_registers registers;

    operand_t operand;
    nes_system* system;
    nes_memory* memory;
    nes_cycle_t _cycle;
    bool            _nmi_pending;           // NMI interrupt pending from PPU vertical blanking
    bool            _dma_pending;           // OAMDMA is requested from writing $4014
    uint16_t        _dma_addr;              // starting address
    void NMI();
    void OAMDMA();

public:
#define STACK_OFFSET 0x100
    void nes_log(uint8_t op, opEntry* opEntry, uint16_t pc);
    void request_nmi() { _nmi_pending = true; };
    void request_dma(uint16_t addr) { _dma_pending = true; _dma_addr = addr; }

    void push_byte(uint8_t val)
    {
        // stack grow top->down
        // no underflow/overflow detection
        memory->set_byte(registers.SP + STACK_OFFSET, val);
        registers.SP--;
    }

    void push_word(uint16_t val)
    {
        // high-order bytes push first since the stack grow top->down and the machine is little-endian
        push_byte(val >> 8);
        push_byte(val & 0xff);
    }

    int8_t pop_byte()
    {
        // stack grow top->down
        // no underflow/overflow detection
        registers.SP++;
        return memory->get_byte(registers.SP + STACK_OFFSET);
    }

    int16_t pop_word()
    {
        // low-order bytes pop first since the stack grow top->down and the machine is little-endian
        uint8_t lo = pop_byte();
        uint8_t hi = pop_byte();
        return uint16_t(hi << 8) + lo;
    }

    void set_carry_flag(bool set) { set_flag(PROCESSOR_STATUS_CARRY_MASK, set); }
    uint8_t get_carry() { return (registers.P & PROCESSOR_STATUS_CARRY_MASK); }

    void set_zero_flag(bool set) { set_flag(PROCESSOR_STATUS_ZERO_MASK, set); }
    bool is_zero() { return registers.P & PROCESSOR_STATUS_ZERO_MASK; }

    void set_interrupt_flag(bool set) { set_flag(PROCESSOR_STATUS_INTERRUPT_MASK, set); }
    bool is_interrupt() { return registers.P & PROCESSOR_STATUS_INTERRUPT_MASK; }

    void set_decimal_flag(bool set) { set_flag(PROCESSOR_STATUS_ADC_MASK, set); }

    void set_I_flag(bool set) { set_flag(PROCESSOR_STATUS_A_MASK, set); }
    void set_B_flag(bool set) { set_flag(PROCESSOR_STATUS_B_MASK, set); }

    void set_overflow_flag(bool set) { set_flag(PROCESSOR_STATUS_OVERFLOW_MASK, set); }
    bool is_overflow() { return registers.P & PROCESSOR_STATUS_OVERFLOW_MASK; }

    void set_negative_flag(bool set) { set_flag(PROCESSOR_STATUS_NEGATIVE_MASK, set); }
    bool is_negative() { return registers.P & PROCESSOR_STATUS_NEGATIVE_MASK; }

    void init(nes_system* system);
    uint8_t peek(uint16_t addr) { return memory->get_byte(addr); }
    uint16_t peek_word(uint16_t addr){ return  memory->get_word(addr);}
    void step_to(nes_cycle_t count);
    void exec_one_instruction();
    cpu_registers& reg(){return registers;}

    void calc_flags(uint8_t val){
        if (val == 0){
            set_flag(PROCESSOR_STATUS_ZERO_MASK, true);
        }
        if (val < 0){
            set_flag(PROCESSOR_STATUS_NEGATIVE_MASK, true);
        }
    }

    void set_flag(uint8_t mask, bool set){
        if (set){
            registers.P |= mask;
        } else {
            registers.P &= ~mask;
        }
    }

    void set_byte(uint16_t addr, uint8_t value)
    {
        memory->set_byte(addr, value);
    }

    uint8_t read_byte(uint16_t addr);
    uint8_t read_next_byte();

    uint16_t read_word(uint16_t addr);
    uint16_t read_next_word();

    operand_t get_operand(nes_addr_mode mode);
    void write_operand(operand_t op, int8_t value)
    {
        switch (op.kind)
        {
            case operand_kind::ACCUMULATOR:
                registers.A = value;
                break;
            case operand_kind::ADDRESS:
                set_byte(op.value, value);
                break;
            default:
                assert(false);
        }
    }

    void calc_alu_flag(uint8_t value);
    bool is_sign_overflow(uint8_t val1, int8_t val2, uint8_t new_value)
    {
        return (((val1 & 0x80) == (val2 & 0x80)) &&
                ((val1 & 0x80) != (new_value & 0x80)));
    }

    void branch(bool cond);

    void ADC(operand_t operand);
    void ADC_IMD(operand_t operand);
    void AND(operand_t operand);
    void AND_IND(operand_t operand);
    // ASL - Arithmetic Shift Left
    void ASL_ACC(operand_t operand);
    void ASL(operand_t operand);

    void LDA(operand_t operand);
    void LDA_ABS(operand_t operand);

    void STA(operand_t operand);
    void INC(operand_t operand);
    void LDY(operand_t operand);
    void LDY_ABS(operand_t operand);

    void INY();
    void BRK();

    void BCC();

    void BCS();

    void BEQ();

    void BIT(operand_t operand);

    void BMI();

    void BNE();

    void BPL();

    void BVC();

    void BVS();

    void CLC();

    void CLD();

    void CLI();

    void CLV();

    void CMP(operand_t operand);
    void CMP_IND(operand_t operand);

    void CPX_IMD(operand_t operand);
    void CPX(operand_t operand);

    void CPY_IMD(operand_t operand);
    void CPY(operand_t operand);

    void DEC(operand_t operand);

    void DEX();

    void DEY();

    void EOR(operand_t operand);
    void EOR_IND(operand_t operand);

    void INX();

    void JMP(operand_t operand);

    void JSR(operand_t operand);

    void LDX(operand_t operand);
    void LDX_ABS(operand_t operand);

    void LSR_ACC(operand_t operand);
    void LSR(operand_t operand);

    void NOP();

    void ORA(operand_t operand);
    void ORA_IND(operand_t operand);

    void PHA();

    void PHP();

    void PLA();

    void PLP();

    void ROL_ACC(operand_t operand);
    void ROL(operand_t operand);

    void ROR_ACC(operand_t operand);
    void ROR(operand_t operand);

    void RTI();

    void RTS();

    void SBC(operand_t operand);

    void SBC_IMD(operand_t operand);

    void SEC();

    void SED();

    void SEI();

    void STX(operand_t operand);

    void STY(operand_t operand);

    void TAX();

    void TAY();

    void TSX();

    void TXA();

    void TXS();

    void TYA();

    void UNOFFICIAL(operand_t operand);

    void LAX(operand_t operand);

    void UNOFFICIAL();

    void ISC(operand_t operand);
};