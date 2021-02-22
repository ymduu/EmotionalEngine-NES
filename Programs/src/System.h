#pragma once
#include <stdint.h>
#include <Nes_Pad.h>
#include "cassette.h"
#include "constants.h"
#include "Cpu.h"
#include "Pad.h"
#include "Ppu.h"

namespace nes { namespace detail {
	class CpuBus;
	class PpuBus;
	class Ppu;
	class Cpu;


	// PPU のメモリ空間から参照される、名前が若干ややこしい(というかミスってる)が、要するに VRAM
	class PpuSystem {
		friend PpuBus;
	public:
		PpuSystem()
			:m_NameTable{}
			,m_Pallettes{}
		{}
	private:
		// 0x0000 - 0x1FFF: CHR-ROM(System のカセットを参照する)
		// 0x2000 - 0x2FFF: Nametable
		uint8_t m_NameTable[NAMETABLE_SIZE];
		// 0x3000 - 0x3EFF: 0x2000-0x2EFF のミラー(0x2FFF までではないことに注意する)
		// 0x3F00 - 0x3F1F: Palette
		uint8_t m_Pallettes[PALETTE_SIZE];
		// 0x3F20 - 0x3FFF: 0x3F00 - 0x3FFF のミラー
	
	};

	class System {
		friend detail::CpuBus;
		friend detail::PpuBus;
	public:
		// TORIAEZU: カセットの内容はコンストラクタで受け取る
		System(uint8_t* pBuffer, size_t bufferSize)
			:m_Wram{}
			,m_IoReg{}
		{
			m_Cassette.Initialize(pBuffer, bufferSize);
		}

		void PushButton(PadId id, PadButton button);
		void ReleaseButton(PadId id, PadButton button);

		// TODO: VRAM の中身を取得する関数を公開する
	private:
		// 0x0000 - 0x07FF: WRAM
		// 0x0800 - 0x1FFF: WRAM Mirror * 3 
		uint8_t m_Wram[WRAM_SIZE];
		// 0x2000 - 0x2007: PPU IO Register
		// 0x2009 - 0x3FFF: PPU IO Register Mirror * 1023
		// PPU レジスタは CpuBus を通して直接 PPU のレジスタを読み書きすることにする

		// 0x4000 - 0x401F: APU IO, PAD
		uint8_t m_IoReg[APU_IO_REG_SIZE];

		// Pad: 0x4016, 0x4017
		Pad m_Pads[2];

		detail::Cassette m_Cassette;
	};

	// System へのポインタを持ち、CPU から見えるメモリ空間に基づいてアクセスするクラス
	class CpuBus {
	public:
		CpuBus(System* pSystem, Ppu* pPpu)
			:m_pSystem(pSystem)
			,m_pPpu(pPpu)
			,m_IsDmaRunning(false)
			,m_DmaUpperSrcAddr(0)
		{}
		uint8_t ReadByte(uint16_t addr);
		void WriteByte(uint16_t addr, uint8_t data);

		// DMA 用 API
		// System 上のメモリから PPU 上の OAM に DMA する。 DMA は基本的に VBLANK 中に行われることに注意する。(4.5Line 分くらいのPPU時間を消費するが、それによって NMI を発生し損ねたり sprite 0 hit をし損ねたりすることはない。)

		// DMA 実行してかかったクロック数を返す、 Kick されてない場合は0
		int RunDma(uint64_t cpuCycles);
		// 転送元の アドレスの上位バイトを与えてDMA 起動、下位バイトは 00
		void KickDma(uint8_t upperSrcAddr);
	private:
		System* m_pSystem;
		Ppu* m_pPpu;
		// DMA は CpuBus が担う(実際の HW 構成もそうなっているはず。)
		bool m_IsDmaRunning;
		uint8_t m_DmaUpperSrcAddr;
	};

	// PPU から見えるメモリ空間に基づいてアクセスするクラス、PPU <-> カセット、 VRAM へのバス
	// CPU -> PPU は CpuBus に持たせる
	// TODO: 本当は Cassette だけへの依存でいいのに System 全体への依存になってしまっているのを直す
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
		// オブジェクト生成時の循環依存を切るため、 Initialize で Cpu へのポインタを渡す
		void Initialize(Cpu* pCpu);
		// 描画終了のタイミングに合わせて NMI を入れる
		void GenerateCpuInterrupt();
	private:
		// カセットで指定されるミラー方法に基づいてミラー元のアドレスを計算する
		uint16_t GetMirroredAddr(uint16_t addr);

		System* m_pSystem;
		PpuSystem* m_pPpuSystem;
		// CPU にバスを繋ぐ
		Cpu* m_pCpu;
		bool m_IsInitialized;
	};

}}