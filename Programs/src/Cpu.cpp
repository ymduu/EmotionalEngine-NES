#pragma once
#include <cstdlib>
#include <stdint.h>
#include "constants.h"
#include "Cpu.h"

namespace nes { namespace detail {
	Instruction ByteToInstruction(uint8_t byte)
	{
		switch (byte)
		{
        case 0x69:
            return Instruction(Opcode::ADC, AddressingMode::Immediate, 2, 2);
        case 0x65:
            return Instruction(Opcode::ADC, AddressingMode::ZeroPage, 2, 3);
        case 0x75:
            return Instruction(Opcode::ADC, AddressingMode::ZeroPageX, 2, 4);
        case 0x6D:
            return Instruction(Opcode::ADC, AddressingMode::Absolute, 3, 4);
        case 0x7D:
            return Instruction(Opcode::ADC, AddressingMode::AbsoluteX, 3, 4);
        case 0x79:
            return Instruction(Opcode::ADC, AddressingMode::AbsoluteY, 3, 4);
        case 0x61:
            return Instruction(Opcode::ADC, AddressingMode::IndirectX, 2, 6);
        case 0x71:
            return Instruction(Opcode::ADC, AddressingMode::IndirectY, 2, 5);
        case 0x29:
            return Instruction(Opcode::AND, AddressingMode::Immediate, 2, 2);
        case 0x25:
            return Instruction(Opcode::AND, AddressingMode::ZeroPage, 2, 3);
        case 0x35:
            return Instruction(Opcode::AND, AddressingMode::ZeroPageX, 2, 4);
        case 0x2D:
            return Instruction(Opcode::AND, AddressingMode::Absolute, 3, 4);
        case 0x3D:
            return Instruction(Opcode::AND, AddressingMode::AbsoluteX, 3, 4);
        case 0x39:
            return Instruction(Opcode::AND, AddressingMode::AbsoluteY, 3, 4);
        case 0x21:
            return Instruction(Opcode::AND, AddressingMode::IndirectX, 2, 6);
        case 0x31:
            return Instruction(Opcode::AND, AddressingMode::IndirectY, 2, 5);
        case 0x0A:
            return Instruction(Opcode::ASL, AddressingMode::Accumulator, 1, 2);
        case 0x06:
            return Instruction(Opcode::ASL, AddressingMode::ZeroPage, 2, 5);
        case 0x16:
            return Instruction(Opcode::ASL, AddressingMode::ZeroPageX, 2, 6);
        case 0x0E:
            return Instruction(Opcode::ASL, AddressingMode::Absolute, 3, 6);
        case 0x1E:
            return Instruction(Opcode::ASL, AddressingMode::AbsoluteX, 3, 7);
        case 0x90:
            return Instruction(Opcode::BCC, AddressingMode::Relative, 2, 2);
        case 0xB0:
            return Instruction(Opcode::BCS, AddressingMode::Relative, 2, 2);
        case 0xF0:
            return Instruction(Opcode::BEQ, AddressingMode::Relative, 2, 2);
        case 0x24:
            return Instruction(Opcode::BIT, AddressingMode::ZeroPage, 2, 3);
        case 0x2C:
            return Instruction(Opcode::BIT, AddressingMode::Absolute, 3, 4);
        case 0x30:
            return Instruction(Opcode::BMI, AddressingMode::Relative, 2, 2);
        case 0xD0:
            return Instruction(Opcode::BNE, AddressingMode::Relative, 2, 2);
        case 0x10:
            return Instruction(Opcode::BPL, AddressingMode::Relative, 2, 2);
        case 0x00:
            return Instruction(Opcode::BRK, AddressingMode::Implied, 1, 7);
        case 0x50:
            return Instruction(Opcode::BVC, AddressingMode::Relative, 2, 2);
        case 0x70:
            return Instruction(Opcode::BVS, AddressingMode::Relative, 2, 2);
        case 0x18:
            return Instruction(Opcode::CLC, AddressingMode::Implied, 1, 2);
        case 0xD8:
            return Instruction(Opcode::CLD, AddressingMode::Implied, 1, 2);
        case 0x58:
            return Instruction(Opcode::CLI, AddressingMode::Implied, 1, 2);
        case 0xB8:
            return Instruction(Opcode::CLV, AddressingMode::Implied, 1, 2);
        case 0xC9:
            return Instruction(Opcode::CMP, AddressingMode::Immediate, 2, 2);
        case 0xC5:
            return Instruction(Opcode::CMP, AddressingMode::ZeroPage, 2, 3);
        case 0xD5:
            return Instruction(Opcode::CMP, AddressingMode::ZeroPageX, 2, 4);
        case 0xCD:
            return Instruction(Opcode::CMP, AddressingMode::Absolute, 3, 4);
        case 0xDD:
            return Instruction(Opcode::CMP, AddressingMode::AbsoluteX, 3, 4);
        case 0xD9:
            return Instruction(Opcode::CMP, AddressingMode::AbsoluteY, 3, 4);
        case 0xC1:
            return Instruction(Opcode::CMP, AddressingMode::IndirectX, 2, 6);
        case 0xD1:
            return Instruction(Opcode::CMP, AddressingMode::IndirectY, 2, 5);
        case 0xE0:
            return Instruction(Opcode::CPX, AddressingMode::Immediate, 2, 2);
        case 0xE4:
            return Instruction(Opcode::CPX, AddressingMode::ZeroPage, 2, 3);
        case 0xEC:
            return Instruction(Opcode::CPX, AddressingMode::Absolute, 3, 4);
        case 0xC0:
            return Instruction(Opcode::CPY, AddressingMode::Immediate, 2, 2);
        case 0xC4:
            return Instruction(Opcode::CPY, AddressingMode::ZeroPage, 2, 3);
        case 0xCC:
            return Instruction(Opcode::CPY, AddressingMode::Absolute, 3, 4);
        case 0xC6:
            return Instruction(Opcode::DEC, AddressingMode::ZeroPage, 2, 5);
        case 0xD6:
            return Instruction(Opcode::DEC, AddressingMode::ZeroPageX, 2, 6);
        case 0xCE:
            return Instruction(Opcode::DEC, AddressingMode::Absolute, 3, 6);
        case 0xDE:
            return Instruction(Opcode::DEC, AddressingMode::AbsoluteX, 3, 7);
        case 0xCA:
            return Instruction(Opcode::DEX, AddressingMode::Implied, 1, 2);
        case 0x88:
            return Instruction(Opcode::DEY, AddressingMode::Implied, 1, 2);
        case 0x49:
            return Instruction(Opcode::EOR, AddressingMode::Immediate, 2, 2);
        case 0x45:
            return Instruction(Opcode::EOR, AddressingMode::ZeroPage, 2, 3);
        case 0x55:
            return Instruction(Opcode::EOR, AddressingMode::ZeroPageX, 2, 4);
        case 0x4D:
            return Instruction(Opcode::EOR, AddressingMode::Absolute, 3, 4);
        case 0x5D:
            return Instruction(Opcode::EOR, AddressingMode::AbsoluteX, 3, 4);
        case 0x59:
            return Instruction(Opcode::EOR, AddressingMode::AbsoluteY, 3, 4);
        case 0x41:
            return Instruction(Opcode::EOR, AddressingMode::IndirectX, 2, 6);
        case 0x51:
            return Instruction(Opcode::EOR, AddressingMode::IndirectY, 2, 5);
        case 0xE6:
            return Instruction(Opcode::INC, AddressingMode::ZeroPage, 2, 5);
        case 0xF6:
            return Instruction(Opcode::INC, AddressingMode::ZeroPageX, 2, 6);
        case 0xEE:
            return Instruction(Opcode::INC, AddressingMode::Absolute, 3, 6);
        case 0xFE:
            return Instruction(Opcode::INC, AddressingMode::AbsoluteX, 3, 7);
        case 0xE8:
            return Instruction(Opcode::INX, AddressingMode::Implied, 1, 2);
        case 0xC8:
            return Instruction(Opcode::INY, AddressingMode::Implied, 1, 2);
        case 0x4C:
            return Instruction(Opcode::JMP, AddressingMode::Absolute, 3, 3);
        case 0x6C:
            return Instruction(Opcode::JMP, AddressingMode::Indirect, 3, 5);
        case 0x20:
            return Instruction(Opcode::JSR, AddressingMode::Absolute, 3, 6);
        case 0xA9:
            return Instruction(Opcode::LDA, AddressingMode::Immediate, 2, 2);
        case 0xA5:
            return Instruction(Opcode::LDA, AddressingMode::ZeroPage, 2, 3);
        case 0xB5:
            return Instruction(Opcode::LDA, AddressingMode::ZeroPageX, 2, 4);
        case 0xAD:
            return Instruction(Opcode::LDA, AddressingMode::Absolute, 3, 4);
        case 0xBD:
            return Instruction(Opcode::LDA, AddressingMode::AbsoluteX, 3, 4);
        case 0xB9:
            return Instruction(Opcode::LDA, AddressingMode::AbsoluteY, 3, 4);
        case 0xA1:
            return Instruction(Opcode::LDA, AddressingMode::IndirectX, 2, 6);
        case 0xB1:
            return Instruction(Opcode::LDA, AddressingMode::IndirectY, 2, 5);
        case 0xA2:
            return Instruction(Opcode::LDX, AddressingMode::Immediate, 2, 2);
        case 0xA6:
            return Instruction(Opcode::LDX, AddressingMode::ZeroPage, 2, 3);
        case 0xB6:
            return Instruction(Opcode::LDX, AddressingMode::ZeroPageY, 2, 4);
        case 0xAE:
            return Instruction(Opcode::LDX, AddressingMode::Absolute, 3, 4);
        case 0xBE:
            return Instruction(Opcode::LDX, AddressingMode::AbsoluteY, 3, 4);
        case 0xA0:
            return Instruction(Opcode::LDY, AddressingMode::Immediate, 2, 2);
        case 0xA4:
            return Instruction(Opcode::LDY, AddressingMode::ZeroPage, 2, 3);
        case 0xB4:
            return Instruction(Opcode::LDY, AddressingMode::ZeroPageX, 2, 4);
        case 0xAC:
            return Instruction(Opcode::LDY, AddressingMode::Absolute, 3, 4);
        case 0xBC:
            return Instruction(Opcode::LDY, AddressingMode::AbsoluteX, 3, 4);
        case 0x4A:
            return Instruction(Opcode::LSR, AddressingMode::Accumulator, 1, 2);
        case 0x46:
            return Instruction(Opcode::LSR, AddressingMode::ZeroPage, 2, 5);
        case 0x56:
            return Instruction(Opcode::LSR, AddressingMode::ZeroPageX, 2, 6);
        case 0x4E:
            return Instruction(Opcode::LSR, AddressingMode::Absolute, 3, 6);
        case 0x5E:
            return Instruction(Opcode::LSR, AddressingMode::AbsoluteX, 3, 7);
        case 0xEA:
            return Instruction(Opcode::NOP, AddressingMode::Implied, 1, 2);
        case 0x09:
            return Instruction(Opcode::ORA, AddressingMode::Immediate, 2, 2);
        case 0x05:
            return Instruction(Opcode::ORA, AddressingMode::ZeroPage, 2, 3);
        case 0x15:
            return Instruction(Opcode::ORA, AddressingMode::ZeroPageX, 2, 4);
        case 0x0D:
            return Instruction(Opcode::ORA, AddressingMode::Absolute, 3, 4);
        case 0x1D:
            return Instruction(Opcode::ORA, AddressingMode::AbsoluteX, 3, 4);
        case 0x19:
            return Instruction(Opcode::ORA, AddressingMode::AbsoluteY, 3, 4);
        case 0x01:
            return Instruction(Opcode::ORA, AddressingMode::IndirectX, 2, 6);
        case 0x11:
            return Instruction(Opcode::ORA, AddressingMode::IndirectY, 2, 5);
        case 0x48:
            return Instruction(Opcode::PHA, AddressingMode::Implied, 1, 3);
        case 0x08:
            return Instruction(Opcode::PHP, AddressingMode::Implied, 1, 3);
        case 0x68:
            return Instruction(Opcode::PLA, AddressingMode::Implied, 1, 4);
        case 0x28:
            return Instruction(Opcode::PLP, AddressingMode::Implied, 1, 4);
        case 0x2A:
            return Instruction(Opcode::ROL, AddressingMode::Accumulator, 1, 2);
        case 0x26:
            return Instruction(Opcode::ROL, AddressingMode::ZeroPage, 2, 5);
        case 0x36:
            return Instruction(Opcode::ROL, AddressingMode::ZeroPageX, 2, 6);
        case 0x2E:
            return Instruction(Opcode::ROL, AddressingMode::Absolute, 3, 6);
        case 0x3E:
            return Instruction(Opcode::ROL, AddressingMode::AbsoluteX, 3, 7);
        case 0x6A:
            return Instruction(Opcode::ROR, AddressingMode::Accumulator, 1, 2);
        case 0x66:
            return Instruction(Opcode::ROR, AddressingMode::ZeroPage, 2, 5);
        case 0x76:
            return Instruction(Opcode::ROR, AddressingMode::ZeroPageX, 2, 6);
        case 0x6E:
            return Instruction(Opcode::ROR, AddressingMode::Absolute, 3, 6);
        case 0x7E:
            return Instruction(Opcode::ROR, AddressingMode::AbsoluteX, 3, 7);
        case 0x40:
            return Instruction(Opcode::RTI, AddressingMode::Implied, 1, 6);
        case 0x60:
            return Instruction(Opcode::RTS, AddressingMode::Implied, 1, 6);
        case 0xE9:
            return Instruction(Opcode::SBC, AddressingMode::Immediate, 2, 2);
        case 0xE5:
            return Instruction(Opcode::SBC, AddressingMode::ZeroPage, 2, 3);
        case 0xF5:
            return Instruction(Opcode::SBC, AddressingMode::ZeroPageX, 2, 4);
        case 0xED:
            return Instruction(Opcode::SBC, AddressingMode::Absolute, 3, 4);
        case 0xFD:
            return Instruction(Opcode::SBC, AddressingMode::AbsoluteX, 3, 4);
        case 0xF9:
            return Instruction(Opcode::SBC, AddressingMode::AbsoluteY, 3, 4);
        case 0xE1:
            return Instruction(Opcode::SBC, AddressingMode::IndirectX, 2, 6);
        case 0xF1:
            return Instruction(Opcode::SBC, AddressingMode::IndirectY, 2, 5);
        case 0x38:
            return Instruction(Opcode::SEC, AddressingMode::Implied, 1, 2);
        case 0xF8:
            return Instruction(Opcode::SED, AddressingMode::Implied, 1, 2);
        case 0x78:
            return Instruction(Opcode::SEI, AddressingMode::Implied, 1, 2);
        case 0x85:
            return Instruction(Opcode::STA, AddressingMode::ZeroPage, 2, 3);
        case 0x95:
            return Instruction(Opcode::STA, AddressingMode::ZeroPageX, 2, 4);
        case 0x8D:
            return Instruction(Opcode::STA, AddressingMode::Absolute, 3, 4);
        case 0x9D:
            return Instruction(Opcode::STA, AddressingMode::AbsoluteX, 3, 5);
        case 0x99:
            return Instruction(Opcode::STA, AddressingMode::AbsoluteY, 3, 5);
        case 0x81:
            return Instruction(Opcode::STA, AddressingMode::IndirectX, 2, 6);
        case 0x91:
            return Instruction(Opcode::STA, AddressingMode::IndirectY, 2, 6);
        case 0x86:
            return Instruction(Opcode::STX, AddressingMode::ZeroPage, 2, 3);
        case 0x96:
            return Instruction(Opcode::STX, AddressingMode::ZeroPageY, 2, 4);
        case 0x8E:
            return Instruction(Opcode::STX, AddressingMode::Absolute, 3, 4);
        case 0x84:
            return Instruction(Opcode::STY, AddressingMode::ZeroPage, 2, 3);
        case 0x94:
            return Instruction(Opcode::STY, AddressingMode::ZeroPageX, 2, 4);
        case 0x8C:
            return Instruction(Opcode::STY, AddressingMode::Absolute, 3, 4);
        case 0xAA:
            return Instruction(Opcode::TAX, AddressingMode::Implied, 1, 2);
        case 0xA8:
            return Instruction(Opcode::TAY, AddressingMode::Implied, 1, 2);
        case 0xBA:
            return Instruction(Opcode::TSX, AddressingMode::Implied, 1, 2);
        case 0x8A:
            return Instruction(Opcode::TXA, AddressingMode::Implied, 1, 2);
        case 0x9A:
            return Instruction(Opcode::TXS, AddressingMode::Implied, 1, 2);
        case 0x98:
            return Instruction(Opcode::TYA, AddressingMode::Implied, 1, 2);

		default:
			abort();
			break;
		}
	}

    void Cpu::SetNegativeFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 7);
        }
        else {
            P &= ~(1 << 7);
        }
    }
    void Cpu::SetOverflowFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 6);
        }
        else {
            P &= ~(1 << 6);
        }
    }
    void Cpu::SetBreakFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 4);
        }
        else {
            P &= ~(1 << 4);
        }
    }
    void Cpu::SetDecimalFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 3);
        }
        else {
            P &= ~(1 << 3);
        }
    }
    void Cpu::SetInterruptFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 2);
        }
        else {
            P &= ~(1 << 2);
        }
    }
    void Cpu::SetZeroFlag(bool flag)
    {
        if (flag)
        {
            P |= (1 << 1);
        }
        else {
            P &= ~(1 << 1);
        }
    }
    void Cpu::SetCarryFlag(bool flag)
    {
        if (flag)
        {
            P |= 1;
        }
        else {
            P &= ~(1);
        }
    }

    bool Cpu::GetNegativeFlag()
    {
        return (P & (1 << 7)) == (1 << 7);
    }
    bool Cpu::GetOverflowFlag()
    {
        return (P & (1 << 6)) == (1 << 6);
    }
    bool Cpu::GetBreakFlag()
    {
        return (P & (1 << 4)) == (1 << 4);
    }
    bool Cpu::GetDecimalFlag()
    {
        return (P & (1 << 3)) == (1 << 3);
    }
    bool Cpu::GetInterruptFlag()
    {
        return (P & (1 << 2)) == (1 << 2);
    }
    bool Cpu::GetZeroFlag()
    {
        return (P & (1 << 1)) == (1 << 1);
    }
    bool Cpu::GetCarryFlag()
    {
        return (P & 1) == 1;
    }
}}