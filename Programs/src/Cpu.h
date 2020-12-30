#pragma once
#include <stdint.h>
#include "constants.h"

namespace nes { namespace detail {
    enum class Opcode {
        ADC,
        AND,
        ASL,
        BCC,
        BCS,
        BEQ,
        BIT,
        BMI,
        BNE,
        BPL,
        BRK,
        BVC,
        BVS,
        CLC,
        CLD,
        CLI,
        CLV,
        CMP,
        CPX,
        CPY,
        DEC,
        DEX,
        DEY,
        EOR,
        INC,
        INX,
        INY,
        JMP,
        JSR,
        LDA,
        LDX,
        LDY,
        LSR,
        NOP,
        ORA,
        PHA,
        PHP,
        PLA,
        PLP,
        ROL,
        ROR,
        RTI,
        RTS,
        SBC,
        SEC,
        SED,
        SEI,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA,
    };
    enum class AddressingMode {
        Implied,
        Accumulator,
        Immediate,
        Absolute,
        ZeroPage,
        ZeroPageX,
        ZeroPageY,
        AbsoluteX,
        AbsoluteY,
        Relative,
        Indirect,
        IndirectX,
        IndirectY,
    };

    class Instruction {
    public:
        Instruction(Opcode opcode, AddressingMode mode, uint8_t bytes, uint8_t cycles)
            :m_Opcode(opcode)
            ,m_AddressingMode(mode)
            ,m_Bytes(bytes)
            ,m_Cycles(cycles)
        {}
        Opcode m_Opcode;
        AddressingMode m_AddressingMode;
        uint8_t m_Bytes;
        uint8_t m_Cycles;
    };

    Instruction ByteToInstruction(uint8_t byte);
}}