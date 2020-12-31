#pragma once
#include <stdint.h>
#include "constants.h"
#include "System.h"

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

    class Cpu {
    public:
        // 1命令実行し、実行にかかったクロックを返す
        uint8_t Run();
        // デバッグ出力用にサイクル数を数えておく
        uint64_t m_CyclesForDebug;
        // 今の状態をダンプする(nestest.log 形式と FCEUX 形式の両方に対応したい)
        void PrintStatusForDebug();

        Cpu(System* pSystem)
            :m_CyclesForDebug(0)
            ,A(0)
            ,X(0)
            ,Y(0)
            ,PC(0)
            ,SP(0)
            ,P(1 << 5)
            ,m_CpuBus(pSystem)
        {}

    private:
        // レジスタ名は命名規則に従わない
        uint8_t A;
        uint8_t X;
        uint8_t Y;
        uint16_t PC;
        uint16_t SP;
        // 下から順番に、 CZIDB1VN
        uint8_t P;

        // CPU BUS 経由でシステムを読み書きする
        CpuBus m_CpuBus;

        // ステータスフラグをいじる関数
        void SetNegativeFlag(bool flag);
        void SetOverflowFlag(bool flag);
        void SetBreakFlag(bool flag);
        void SetDecimalFlag(bool flag);
        void SetInterruptFlag(bool flag);
        void SetZeroFlag(bool flag);
        void SetCarryFlag(bool flag);

        bool GetNegativeFlag();
        bool GetOverflowFlag();
        bool GetBreakFlag();
        bool GetDecimalFlag();
        bool GetInterruptFlag();
        bool GetZeroFlag();
        bool GetCarryFlag();

        // アドレッシングモードによってオペランドのアドレスをフェッチし、アドレスと追加クロックサイクルを返す
        // 以下は、アドレスを対象にする命令(例: ストア) -> FetchAddr, 値(参照を剥がして値にするものも含む)を対象にする命令(例: ADC) -> FetchArg と使い分ける

        // ブランチ条件成立時の追加クロックサイクルは考慮しないことに注意すること
        void FetchAddr(AddressingMode mode, uint16_t* pOutAddr, uint8_t* pOutAdditionalCyc);
        // アドレッシングモードによってオペランドの参照を適切に剥がして値を返す
        void FetchArg(AddressingMode mode, uint8_t* pOutValue, uint8_t* pOutAdditionalCyc);

    };
}}