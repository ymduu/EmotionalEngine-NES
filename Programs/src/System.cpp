#pragma once
#include <cassert>
#include "System.h"

namespace nes {
	namespace detail {
		uint8_t CpuBus::ReadByte(uint16_t addr)
		{
			// WRAM からの読み出し
			if (addr < PPU_REG_BASE)
			{
				// Mirror 対応のため WRAM SIZE であまりをとる
				size_t idx = addr % WRAM_SIZE;
				return m_pSystem->m_Wram[idx];
			}
			else if (addr < APU_IO_REG_BASE)
			{
				size_t offset = addr - PPU_REG_BASE;
				size_t idx = offset % PPU_REG_SIZE;
				// TODO: PPU レジスタ特有の処理(破壊読出しとか)
				return m_pSystem->m_PpuReg[idx];
			}
			else if (addr < CASSETTE_BASE) 
			{
				size_t idx = addr - APU_IO_REG_BASE;
				// TODO: PAD, APU 実装
				return m_pSystem->m_IoReg[idx];
			}
			else 
			{
				// TORIAEZU: カセットの拡張 ROM RAM は気にしない
				assert(addr >= CASSETTE_PRG_ROM_BASE);

				int offset = addr - CASSETTE_PRG_ROM_BASE;
				uint8_t ret;
				// CPU から見えるのは PRG ROM のみ
				m_pSystem->m_Cassette.ReadPrgRom(&ret, offset, 1);
				return ret;
			}
		}

		void CpuBus::WriteByte(uint16_t addr, uint8_t data)
		{
			// WRAM からの読み出し
			if (addr < PPU_REG_BASE)
			{
				// Mirror 対応のため WRAM SIZE であまりをとる
				size_t idx = addr % WRAM_SIZE;
				m_pSystem->m_Wram[idx] = data;
			}
			else if (addr < APU_IO_REG_BASE)
			{
				size_t offset = addr - PPU_REG_BASE;
				size_t idx = offset % PPU_REG_SIZE;
				// TODO: PPU レジスタ特有の処理(破壊読出しとか)
				m_pSystem->m_PpuReg[idx] = data;
			}
			else if (addr < CASSETTE_BASE)
			{
				size_t idx = addr - APU_IO_REG_BASE;
				// TODO: PAD, APU 実装
				m_pSystem->m_IoReg[idx] = data;
			}
			else
			{
				// TORIAEZU: カセットの拡張 ROM RAM は気にしない
				assert(addr >= CASSETTE_PRG_ROM_BASE);

				int offset = addr - CASSETTE_PRG_ROM_BASE;
				// CPU から見えるのは PRG ROM のみ
				m_pSystem->m_Cassette.WritePrgRom(&data, offset, 1);
			}
		}
	}
}