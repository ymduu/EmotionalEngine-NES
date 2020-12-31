#pragma once
#include <cassert>
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
    // 対象のアドレスをフェッチする君、値の取得レイヤーは一つ上の層をつくってそこでやる
    void Cpu::FetchAddr(AddressingMode mode, uint16_t* pOutAddr, uint8_t* pOutAdditionalCyc)
    {
        // アドレスじゃないはずの人らが来てたらプログラミングミスなので assert しとく
        assert(mode != AddressingMode::Implied && mode != AddressingMode::Immediate && mode != AddressingMode::Accumulator);

        *pOutAddr = 0;
        *pOutAdditionalCyc = 0;

        // PC は命令とオペランドのフェッチでは動かさず、命令実行後にまとめて動かす(デバッグログの実装で有利になる……はず)
        if (mode == AddressingMode::Absolute)
        {
            uint16_t upper = 0;
            uint16_t lower = 0;
            lower = m_CpuBus.ReadByte(PC + 1);
            upper = m_CpuBus.ReadByte(PC + 2);

            uint16_t addr = 0;
            addr |= lower;
            addr |= (upper << 8);

            *pOutAddr = addr;
        }
        else if (mode == AddressingMode::ZeroPage)
        {
            uint16_t addr = m_CpuBus.ReadByte(PC + 1);

            *pOutAddr = addr;
        }
        else if (mode == AddressingMode::ZeroPageX)
        {
            uint8_t lower = m_CpuBus.ReadByte(PC + 1);
            
            // 上位バイトへの桁上げは無視、なので uint8 のまま加算する
            lower += X;
            *pOutAddr = lower;
        }
        else if (mode == AddressingMode::ZeroPageY)
        {
            uint8_t lower = m_CpuBus.ReadByte(PC + 1);

            // 上位バイトへの桁上げは無視、なので uint8 のまま加算する
            lower += Y;
            *pOutAddr = lower;
        }
        else if (mode == AddressingMode::AbsoluteX)
        {
            uint16_t upper = 0;
            uint16_t lower = 0;
            lower = m_CpuBus.ReadByte(PC + 1);
            upper = m_CpuBus.ReadByte(PC + 2);

            uint16_t addr = 0;
            addr |= lower;
            addr |= (upper << 8);

            uint16_t beforeAddr = addr;
            addr += X;

            *pOutAddr = addr;
            // ページクロスで +1 クロック
            if ((beforeAddr & 0xFF00) != (addr & 0xFF00))
            {
                *pOutAdditionalCyc = 1;
            }

        }
        else if (mode == AddressingMode::AbsoluteY)
        {
            uint16_t upper = 0;
            uint16_t lower = 0;
            lower = m_CpuBus.ReadByte(PC + 1);
            upper = m_CpuBus.ReadByte(PC + 2);

            uint16_t addr = 0;
            addr |= lower;
            addr |= (upper << 8);

            uint16_t beforeAddr = addr;
            addr += Y;

            *pOutAddr = addr;
            // ページクロスで +1 クロック
            if ((beforeAddr & 0xFF00) != (addr & 0xFF00))
            {
                *pOutAdditionalCyc = 1;
            }
        }
        else if (mode == AddressingMode::Relative)
        {
            uint8_t offset = m_CpuBus.ReadByte(PC + 1);
            // 符号拡張 する(若干怪しいので、バグったら疑う(最悪))
            int32_t signedOffset = static_cast<int8_t>(offset);
            int32_t signedPC = static_cast<int16_t>(PC);

            int32_t signedAddr = signedPC + signedOffset;
            // uint16_t に収まっていることを確認
            assert(signedAddr >= 0 && signedAddr <= 0xFFFF);
            uint16_t addr = static_cast<uint16_t>(signedAddr);

            *pOutAddr = addr;
            // ページクロスで +1 クロック、Relative はブランチ命令で使われるが、ブランチ成立時にはさらに +1 されることに注意する
            if ((PC & 0xFF00) != (addr & 0xFF00))
            {
                *pOutAdditionalCyc = 1;
            }
        }
        else if (mode == AddressingMode::IndirectX)
        {
            // *(lower + X)
            uint8_t indirectLower = 0;
            indirectLower = m_CpuBus.ReadByte(PC + 1);
            // キャリーは無視 = オーバーフローしても気にしない
            uint8_t indirectAddr = indirectLower + X;
            // Indirect なので、FetchAddr 内で1回参照を剥がす
            uint16_t lower = m_CpuBus.ReadByte(indirectAddr);
            uint16_t upper = m_CpuBus.ReadByte(indirectAddr + 1);

            uint16_t addr = lower | (upper << 8);

            *pOutAddr = addr;
        }
        else if (mode == AddressingMode::IndirectY)
        {
            // *(lower) + Y
            // キャリーは無視 = オーバーフローしても気にしない
            uint8_t indirectAddr = m_CpuBus.ReadByte(PC + 1);
            // Indirect なので、FetchAddr 内で1回参照を剥がす
            uint16_t lower = m_CpuBus.ReadByte(indirectAddr);
            uint16_t upper = m_CpuBus.ReadByte(indirectAddr + 1);

            uint16_t addr = lower | (upper << 8);
            uint16_t beforeAddr = addr;

            addr += Y;

            *pOutAddr = addr;
            // ページクロスで +1 クロック
            if ((beforeAddr & 0xFF00) != (addr & 0xFF00))
            {
                *pOutAdditionalCyc = 1;
            }
        }
        else if (mode == AddressingMode::Indirect)
        {
            // **(addr)
            uint16_t indirectAddr = 0;
            uint8_t indirectLower = 0;
            uint8_t indirectUpper = 0;

            indirectLower = m_CpuBus.ReadByte(PC + 1);
            indirectUpper = m_CpuBus.ReadByte(PC + 2);

            // Indirect なので、FetchAddr 内で1回参照を剥がす
            uint16_t addrLower = m_CpuBus.ReadByte(static_cast<uint16_t>(indirectLower) | (static_cast<uint16_t>(indirectUpper) << 8));
            // インクリメントにおいて下位バイトからのキャリーを無視するために、下位バイトに加算してからキャストする(ほんまか？？？？？)
            uint16_t addrUpper = m_CpuBus.ReadByte(static_cast<uint16_t>(indirectLower + 1) | (static_cast<uint16_t>(indirectUpper) << 8));

            uint16_t addr = addrLower | (addrUpper << 8);
            *pOutAddr = addr;
        }
        else
        {
            // unexpected default
            abort();
        }
    }

    // アドレッシングモードといまの PC の値から命令でつかう引数(？)を取得する
    void Cpu::FetchArg(AddressingMode mode, uint8_t* pOutValue, uint8_t* pOutAdditionalCyc)
    {
        // 引数を持たないアドレッシングモードで呼ばれたらプログラミングミスなので assert しとく
        assert(mode != AddressingMode::Implied);

        *pOutValue = 0;
        *pOutAdditionalCyc = 0;

        if (mode == AddressingMode::Accumulator)
        {
            *pOutValue = A;
        }
        else if (mode == AddressingMode::Immediate)
        {
            // Immediate は PC + 1 から素直に読む
            *pOutValue = m_CpuBus.ReadByte(PC + 1);
        }
        else
        {
            // 他はアドレスがオペランドになってるはずなので、アドレスを持ってきて1回参照を剥がす(Indirect は2回参照を剥がす必要があるが、1回は FetchAddr 側で剥がしている)
            uint16_t addr = 0;
            FetchAddr(mode, &addr, pOutAdditionalCyc);
            *pOutValue = m_CpuBus.ReadByte(addr);
        }
    }

    uint8_t Cpu::Run()
    {
        // 命令 フェッチ
        uint8_t instByte = m_CpuBus.ReadByte(PC);
        Instruction inst = ByteToInstruction(instByte);

        if (inst.m_Opcode == Opcode::ADC)
        {
            // オペランド フェッチ
            uint8_t operand;
            uint8_t additionalCyc;
            FetchArg(inst.m_AddressingMode, &operand, &additionalCyc);

            uint16_t calc = static_cast<uint16_t>(A) + operand + GetCarryFlag();
            uint8_t res = static_cast<uint8_t>(calc);

            SetCarryFlag(calc > 0xff);
            SetZeroFlag(res == 0);
            SetNegativeFlag((res & 0x80) == 0x80);
            // http://forums.nesdev.com/viewtopic.php?t=6331
            SetOverflowFlag(((A ^ res) & (operand ^ res) & 0x80) == 0x80);

            A = res;
            return inst.m_Cycles + additionalCyc;
        }

        return 0;
    }
}}