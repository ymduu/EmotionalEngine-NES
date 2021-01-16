#pragma once
#include <stdint.h>
#include "cassette.h"
#include "constants.h"

namespace nes { namespace detail {
	class CpuBus;
	class PpuBus;

	// PPU のメモリ空間、 System の private メンバとして持たせるので public にしておく
	class PpuSystem {
	public:
		// 0x0000 - 0x1FFF: CHR-ROM(System のカセットを参照する)
		// 0x2000 - 0x2FFF: Nametable
		uint8_t m_NameTable[NAMETABLE_SIZE];
		// 0x3000 - 0x3EFF: 0x2000-0x2EFF のミラー(0x2FFF までではないことに注意する)
		// 0x3F00 - 0x3F1F: Palette
		uint8_t m_Pallettes[PALETTE_SIZE];
		// 0x3F20 - 0x3FFF: 0x3F00 - 0x3FFF のミラー

		PpuSystem()
			:m_NameTable{}
			,m_Pallettes{}
		{}
	};

	class System {
		friend detail::CpuBus;
		friend detail::PpuBus;
	public:
		// TORIAEZU: カセットの内容はコンストラクタで受け取る
		System(uint8_t* pBuffer, size_t bufferSize)
			:m_Wram{}
			,m_PpuReg{}
			,m_IoReg{}
		{
			m_Cassette.Initialize(pBuffer, bufferSize);
		}

		// TODO: PAD 操作をする関数を公開する
		// TODO: VRAM の中身を取得する関数を公開する
	private:
		// 0x0000 - 0x07FF: WRAM
		// 0x0800 - 0x1FFF: WRAM Mirror * 3 
		uint8_t m_Wram[WRAM_SIZE];
		// 0x2000 - 0x2007: PPU IO Register
		// 0x2009 - 0x3FFF: PPU IO Register Mirror * 1023
		uint8_t m_PpuReg[PPU_REG_SIZE];
		// 0x4000 - 0x401F: APU IO, PAD
		uint8_t m_IoReg[APU_IO_REG_SIZE];

		detail::Cassette m_Cassette;
		detail::PpuSystem m_PpuSystem;

	};

	// System へのポインタを持ち、CPU から見えるメモリ空間に基づいてアクセスするクラス
	class CpuBus {
	public:
		CpuBus(System* pSystem) 
			:m_pSystem(pSystem)
		{}
		uint8_t ReadByte(uint16_t addr);
		void WriteByte(uint16_t addr, uint8_t data);
	private:
		System* m_pSystem;
	};

	// PPU から見えるメモリ空間に基づいてアクセスするクラス、PPU <-> カセット、 VRAM へのバス
	// CPU -> PPU は CpuBus に持たせる
	class PpuBus {
	public:
		PpuBus(System* pSystem, PpuSystem* pPpuSystem)
			:m_pSystem(pSystem)
			,m_pPpuSystem(pPpuSystem)
		{}
		uint8_t ReadByte(uint16_t addr);
		// パレットテーブルの "下"にある nametableのミラーが PPU の内部バッファに読まれるのでそれに対応する
		uint8_t ReadByte(uint16_t addr, bool isPpuBuffering);
		void WriteByte(uint16_t addr, uint8_t data);
	private:
		System* m_pSystem;
		PpuSystem* m_pPpuSystem;
	};

}}