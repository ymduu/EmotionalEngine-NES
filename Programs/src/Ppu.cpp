#pragma once
#include <cassert>
#include "Ppu.h"

namespace nes { namespace detail {
	void Ppu::WritePpuCtrl(uint8_t data)
	{
		PPUCTRL = data;
	}
	void Ppu::WritePpuMask(uint8_t data)
	{
		PPUMASK = data;
	}
	void Ppu::WriteOamAddr(uint8_t data)
	{
		OAMADDR = data;
	}
	void Ppu::WriteOamData(uint8_t data)
	{
		// TODO: OAM 書き込み + アドレスを1インクリメント
	}
	void Ppu::WritePpuScroll(uint8_t data)
	{
		if (m_IsVerticalScrollVal)
		{
			m_ScrollY = data;
			m_IsVerticalScrollVal = false;
		}
		else
		{
			m_ScrollX = data;
			m_IsVerticalScrollVal = true;
		}
	}
	void Ppu::WritePpuAddr(uint8_t data)
	{
		if (m_IsLowerPpuAddr)
		{
			m_VramAddr |= data;
			m_IsLowerPpuAddr = false;
			m_IsValidPpuAddr = true;
		}
		else
		{
			m_VramAddr = static_cast<uint16_t>(data);
			m_VramAddr <<= 8;
			m_IsLowerPpuAddr = true;
			m_IsValidPpuAddr = false;
		}
	}
	void Ppu::WritePpuData(uint8_t data)
	{
		m_PpuBus->WriteByte(m_VramAddr, data);
		m_VramAddr += GetVramOffset();
	}

	uint8_t Ppu::ReadPpuStatus()
	{
		// 2回読みフラグをリセット
		m_IsVerticalScrollVal = false;
		m_IsLowerPpuAddr = false;
		m_IsValidPpuAddr = false;

		// VBlank フラグをクリア
		SetVBlankFlag(false);

		return PPUSTATUS;
	}
	uint8_t Ppu::ReadPpuData()
	{
		uint8_t buffered = m_VramReadBuf;
		if (m_VramAddr >= PALETTE_BASE)
		{
			// パレットテーブルは即時読み出し、 "下"にあるネームテーブルのミラーがバッファに入る
			m_VramReadBuf = m_PpuBus->ReadByte(m_VramAddr, true);
			uint8_t ret = m_PpuBus->ReadByte(m_VramAddr);
			m_VramAddr += GetVramOffset();

			return ret;
		}
		else
		{
			m_VramReadBuf = m_PpuBus->ReadByte(m_VramAddr);
			m_VramAddr += GetVramOffset();
		}

		return buffered;
	}

	void Ppu::SetVBlankFlag(bool flag)
	{
		if (flag)
		{
			PPUSTATUS |= (1 << 7);
		}
		else
		{
			PPUSTATUS &= ~(1 << 7);
		}
	}

	uint16_t Ppu::GetVramOffset()
	{
		return (PPUCTRL & (1 << 2)) ? 32 : 1;
	}
}}