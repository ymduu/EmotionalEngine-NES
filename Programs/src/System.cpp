#pragma once
#include <cassert>
#include <cstdlib>
#include "System.h"

namespace nes { namespace detail {
	void System::PushButton(PadId id, PadButton button)
	{
		if (id == PadId::Zero)
		{
			m_Pads[0].PushButton(button);
		}
		if (id == PadId::One)
		{
			m_Pads[1].PushButton(button);
		}
	}
	void System::ReleaseButton(PadId id, PadButton button)
	{
		if (id == PadId::Zero)
		{
			m_Pads[0].ReleaseButton(button);
		}
		if (id == PadId::One)
		{
			m_Pads[1].ReleaseButton(button);
		}
	}


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
			// TODO: APU 実装

			// Pad
			if (addr == 0x4016)
			{
				return m_pSystem->m_Pads[0].ReadPad();
			}
			else if (addr == 0x4017)
			{
				return m_pSystem->m_Pads[1].ReadPad();
			}

			// TORIAEZU: 未実装のレジスタ読み出しは残しておく
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
			// TORIAEZU: 疑似書き込みは残しておく(意味ないけど)
			m_pSystem->m_IoReg[idx] = data;
			// DMA
			if (addr == 0x4014)
			{
				KickDma(data);
			}

			// Pad
			if (addr == 0x4016)
			{
				m_pSystem->m_Pads[0].SetStrobe(static_cast<bool>(data & 1));
			}
			else if (addr == 0x4017)
			{
				m_pSystem->m_Pads[1].SetStrobe(static_cast<bool>(data & 1));
			}
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
			// mirror 計算
			uint16_t mirroredAddr = GetMirroredAddr(addr);

			// nametable 読み出し
			size_t offset = mirroredAddr - NAMETABLE_BASE;

			return m_pPpuSystem->m_NameTable[offset];
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
			uint16_t offset = addr - NAMETABLE_MIRROR_BASE;
			// nametable の中だったらどこかを計算してから nametable mirror を計算
			uint16_t nameTableAddr = NAMETABLE_BASE + offset;
			uint16_t nameTableMirroredAddr = GetMirroredAddr(nameTableAddr);

			size_t idx = nameTableMirroredAddr - NAMETABLE_BASE;
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
			// mirror 計算
			uint16_t mirroredAddr = GetMirroredAddr(addr);

			// nametable 書き込み
			size_t offset = mirroredAddr - NAMETABLE_BASE;
			m_pPpuSystem->m_NameTable[offset] = data;
		}
		else
		{
			// palette 書き込み
			size_t offset = addr - PALETTE_BASE;
			size_t idx = offset % PALETTE_SIZE;

			m_pPpuSystem->m_Pallettes[idx] = data;
		}
	}
	void PpuBus::GenerateCpuInterrupt()
	{
		assert(m_IsInitialized);
		m_pCpu->Interrupt(nes::detail::InterruptType::NMI);
	}

	void PpuBus::Initialize(Cpu* pCpu)
	{
		m_pCpu = pCpu;
		m_IsInitialized = true;
	}

	uint16_t PpuBus::GetMirroredAddr(uint16_t addr)
	{
		// nametable 以外の範囲のアドレスが渡されたらプログラミングミス
		assert(0x2000 <= addr && addr < 0x3000);

		Mirroring mirroring = m_pSystem->m_Cassette.GetMirroring();
		if (mirroring == Mirroring::Mirroring_Horizontal)
		{
			// 水平ミラー: [0x2000, 0x2400) が [0x2400, 0x2800) に、[0x2800, 0x2c00) が[0x2c00, 0x3000) にミラーされる
			// ミラー範囲だったら雑に引き算をするぜ
			if ((0x2400 <= addr && addr < 0x2800) ||
				(0x2C00 <= addr && addr < 0x3000))
			{
				addr -= 0x400;
			}
		}
		else if (mirroring == Mirroring::Mirroring_Vertical)
		{
			// 垂直ミラー: [0x2000, 0x2800) が [0x2800, 0x3000) にミラーされる
			if (0x2800 <= addr)
			{
				addr -= 0x800;
			}
		}

		return addr;
	}

	int CpuBus::RunDma(uint64_t cpuCycles)
	{
		if (!m_IsDmaRunning) 
		{
			return 0;
		}
		assert(m_pPpu->OAMADDR == 0);
		int addr = m_DmaUpperSrcAddr << 8;

		for (int i = 0; i < OAM_SIZE; i++)
		{
			m_pPpu->m_Oam[i] = ReadByte(addr + i);
		}

		return  cpuCycles % 2 == 0 ? 513 : 514;
	}

	void CpuBus::KickDma(uint8_t upperSrcAddr)
	{
		m_DmaUpperSrcAddr = upperSrcAddr;
		m_IsDmaRunning = true;
	}
}}