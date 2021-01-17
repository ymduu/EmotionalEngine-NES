#pragma once
#include <cassert>
#include <cstdlib>
#include "System.h"

namespace nes { namespace detail {
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

			switch (idx)
			{
			case 2:
				return m_pPpu->ReadPpuStatus();
			case 7:
				return m_pPpu->ReadPpuData();
			default:
				// unexpected
				abort();
				break;
			}
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
			switch (idx)
			{
			case 0:
				m_pPpu->WritePpuCtrl(data);
				break;
			case 1:
				m_pPpu->WritePpuMask(data);
				break;
			case 3:
				m_pPpu->WriteOamAddr(data);
				break;
			case 4:
				m_pPpu->WriteOamData(data);
				break;
			case 5:
				m_pPpu->WritePpuScroll(data);
				break;
			case 6:
				m_pPpu->WritePpuAddr(data);
				break;
			case 7:
				m_pPpu->WritePpuData(data);
				break;
			default:
				// unexpected
				abort();
				break;
			}
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

	uint8_t PpuBus::ReadByte(uint16_t addr)
	{
		// CHR ROM からの読み出し
		if (addr < NAMETABLE_BASE)
		{
			uint8_t ret = 0;
			m_pSystem->m_Cassette.ReadChrRom(&ret, addr, 1);
			return ret;
		}
		else if (addr < PALETTE_BASE)
		{
			// nametable 読み出し
			size_t offset = addr - NAMETABLE_BASE;
			// mirror
			size_t idx = offset % NAMETABLE_SIZE;
			return m_pPpuSystem->m_NameTable[idx];
		}
		else
		{
			// palette 読み出し
			size_t offset = addr - PALETTE_BASE;
			size_t idx = offset % PALETTE_SIZE;

			return m_pPpuSystem->m_Pallettes[idx];
		}
	}

	uint8_t PpuBus::ReadByte(uint16_t addr, bool isPpuBuffering)
	{
		if (!isPpuBuffering)
		{
			return ReadByte(addr);
		}

		if (addr >= PALETTE_BASE)
		{
			// パレット領域の代わりに nametable 読み出し
			size_t offset = addr - NAMETABLE_BASE;
			// mirror
			size_t idx = offset % NAMETABLE_SIZE;
			return m_pPpuSystem->m_NameTable[idx];
		}
		else
		{
			return ReadByte(addr);
		}
	}

	void PpuBus::WriteByte(uint16_t addr, uint8_t data)
	{
		// CHR ROM 書き込み(？)
		if (addr < NAMETABLE_BASE)
		{
			m_pSystem->m_Cassette.WriteChrRom(&data, addr, 1);
		}
		else if (addr < PALETTE_BASE)
		{
			// nametable 書き込み
			size_t offset = addr - NAMETABLE_BASE;
			// mirror
			size_t idx = offset % NAMETABLE_SIZE;
			m_pPpuSystem->m_NameTable[idx] = data;
		}
		else
		{
			// palette 書き込み
			size_t offset = addr - PALETTE_BASE;
			size_t idx = offset % PALETTE_SIZE;

			m_pPpuSystem->m_Pallettes[idx] = data;
		}
	}
}}