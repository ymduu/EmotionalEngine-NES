#pragma once
#include <memory>
#include <stdint.h>
#include <cassette.h>
#include <Cpu.h>
#include <Ppu.h>
#include <System.h>
#include <Nes_Pad.h>

namespace nes {
	struct Color
	{
		uint8_t Red;
		uint8_t Green;
		uint8_t Blue;
	};

	struct EmuInfo {
		// 雑に規定値を渡しておく
		EmuInfo()
			:CpuInfo(0, 0, 0, 0, 0, 0, detail::Instruction(detail::Opcode::ADC, detail::AddressingMode::Immediate, 0, 0), 0, 0)
		{}

		detail::CpuInfo CpuInfo;
		int PpuLines;
		int PpuCycles;

		uint64_t CpuCycles;
		uint64_t CpuInstructionCount;
	};

	class Emulator {
	public:
		Emulator(std::shared_ptr<uint8_t[]> rom, size_t romSize)
			:m_Rom(rom)
			,m_System(m_Rom.get(), romSize)
			,m_PpuBus(&m_System, &m_PpuSystem)
			,m_Ppu(&m_PpuBus)
			,m_Apu(&m_ApuBus)
			,m_CpuBus(&m_System, &m_Ppu, &m_Apu)
			,m_Cpu(&m_CpuBus)
			,m_ClockCount(7)
			,m_InstructionCount(1)
		{
			// コンストラクタで渡すと循環依存になってしまうのでここだけ Initialize で渡す
			m_PpuBus.Initialize(&m_Cpu);
			m_ApuBus.Initialize(&m_Cpu);
			// Reset 割り込み
			m_Cpu.Interrupt(nes::detail::InterruptType::RESET);
		}

		// テーブルを引いて uint8_t から RGB に変換
		Color GetColor(uint8_t src);
		// 1フレーム進める
		void StepFrame();
		// 1命令進める、1 フレーム完成してたら true が返る
		bool Step();
		// テーブル変換前の絵を取得
		void GetPicture(uint8_t pBuffer[240][256]);
		// テーブル変換後の絵を取得
		void GetPicture(Color pBuffer[240][256]);

		// TODO: RAM, VRAM もとれるようにする
		// CPU と PPU の情報を取得
		void GetEmuInfo(EmuInfo* pOutInfo);

		// Pad のボタン押す
		void PushButton(PadId id, PadButton button);
		void ReleaseButton(PadId id, PadButton button);

	private:
		std::shared_ptr<uint8_t[]> m_Rom;
		detail::System m_System;
		detail::PpuSystem m_PpuSystem;
		detail::PpuBus m_PpuBus;
		detail::Ppu m_Ppu;
		detail::ApuBus m_ApuBus;
		detail::Apu m_Apu;
		detail::CpuBus m_CpuBus;
		detail::Cpu m_Cpu;
		uint64_t m_ClockCount;
		uint64_t m_InstructionCount;
	};

}