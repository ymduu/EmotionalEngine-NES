#pragma once
#include <stdint.h>
#include "constants.h"
#include "System.h"

namespace nes { namespace detail {
	class PpuBus;
	class Ppu {
	public:
		// PPU レジスタは不思議な副作用がたくさんあるので、それを実現できるようにすべてアクセサーでアクセスすることにする
		void WritePpuCtrl(uint8_t data);
		void WritePpuMask(uint8_t data);
		void WriteOamAddr(uint8_t data);
		void WriteOamData(uint8_t data);
		void WritePpuScroll(uint8_t data);
		void WritePpuAddr(uint8_t data);
		void WritePpuData(uint8_t data);

		uint8_t ReadPpuStatus();
		uint8_t ReadPpuData();

		// レジスタ読み書き 終
		void Run(int clk);

		Ppu(PpuBus* pPpuBus)
			:PPUCTRL(0)
			, PPUMASK(0)
			, PPUSTATUS(0)
			, OAMADDR(0)
			, PPUSCROLL(0)
			, PPUADDR(0)
			, PPUDATA(0)
			, OAMDMA(0)
			, m_VramReadBuf(0)
			, m_pPpuBus(pPpuBus)
			, m_IsLowerPpuAddr(false)
			, m_IsValidPpuAddr(false)
			,m_VramAddr(0)
			, m_IsVerticalScrollVal(false)
			,m_ScrollX(0)
			,m_ScrollY(0)
		{}

	private:
		// 例によってレジスタは命名規則に従わないことにします
		// 0x2000
		uint8_t PPUCTRL;
		// 0x2001
		uint8_t PPUMASK;
		// 0x2002
		uint8_t PPUSTATUS;
		// 0x2003
		uint8_t OAMADDR;
		// 0x2004 = OAMDATA は OAM(Sprite RAM) に書かれるので保持しない
		// 0x2005
		uint8_t PPUSCROLL;
		// 0x2006
		uint8_t PPUADDR;
		// 0x2007
		uint8_t PPUDATA;
		// 0x4014
		uint8_t OAMDMA;

		uint8_t m_VramReadBuf;
		// PPU Bus 経由で VRAM を読み書きする
		PpuBus* m_pPpuBus;

		// 2度書き用フラグシリーズ
		// PPUADDR は上位バイト -> 下位バイトの順にかきこみ
		bool m_IsLowerPpuAddr;
		// PPUADDR への2回書き込みが完了しているか？
		bool m_IsValidPpuAddr;
		uint16_t m_VramAddr;
		// PPUSCROLL は水平スクロール値 -> 垂直スクロール値 の順に書き込み(NES on FPGA と NesDev で言ってることが逆だが……)
		bool m_IsVerticalScrollVal;
		uint8_t m_ScrollX;
		uint8_t m_ScrollY;
		
		// コントロールレジスタ 読み書き系
		uint16_t GetVramOffset();
		void SetVBlankFlag(bool flag);

	};
}}