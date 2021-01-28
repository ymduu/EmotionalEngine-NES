#pragma once
#include <cassert>
#include <utility>
#include "Ppu.h"

namespace {
	// テーブルをひきひきするヘルパー関数

	// 座標 -> (nametable id, tile id)のペア
	std::pair<int, int> PositionToTileid(int y, int x)
	{
		int nameTableId = 0;

		if (y >= nes::PPU_OUTPUT_Y)
		{
			y -= nes::PPU_OUTPUT_Y;
			nameTableId += 2;
		}
		if (x >= nes::PPU_OUTPUT_X)
		{
			x -= nes::PPU_OUTPUT_X;
			nameTableId += 1;
		}

		int tileX = x / 8;
		int tileY = y / 8;
		int tileId = tileY * 32 + tileX;

		return std::make_pair(nameTableId, tileId);
	}

	// 座標 -> (attribute table の idx, attribute table 内の idx)のペア
	std::pair<int, int> PositionToAttributeTable(int y, int x)
	{
		// TODO: スクロール 対応

		// パレットは 16 * 16ごと、 Attribute Table は 32 * 32 ごと  
		const int PaletteUnit = 16;
		const int AttributeUnit = PaletteUnit * 2;

		int attributeY = y / AttributeUnit;
		int attributeX = x / AttributeUnit;

		int attributeIdx = attributeY * 8 + attributeX;

		int baseX = attributeX * AttributeUnit;
		int baseY = attributeY * AttributeUnit;

		int dx = x - baseX;
		int dy = y - baseY;

		int paletteX = dx / PaletteUnit;
		int paletteY = dy / PaletteUnit;

		int paletteIdx = paletteY * 2 + paletteX;

		assert(paletteIdx <= 3);

		return std::make_pair(attributeIdx, paletteIdx);
	}
}

namespace nes { namespace detail {
	bool Ppu::Run(int clk)
	{
		m_Cycles += clk;

		// 1 ライン描画するのに 341 サイクルかかる
		if (m_Cycles >= 341)
		{
			m_Cycles -= 341;
			BuildBackGroundLine();
			m_Lines++;
			// Line 241 にきてたら NMI する
			if (m_Lines == 241)
			{
				// VBLANK フラグ立てる
				SetVBlankFlag(true);

				if (PPUCTRL & (1 << 7))
				{
					m_pPpuBus->GenerateCpuInterrupt();
				}
			}
		}
		if (m_Lines == PPU_OUTPUT_Y + PPU_VBLANK_Y)
		{
			m_Lines = 0;
			return true;
		}
		return false;
	}

	// 画面4枚分の座標(つまり、描画座標ではない)を入力してそのピクセルの色を取得する
	uint8_t Ppu::GetBackGroundPixelColor(int y, int x)
	{
		auto [nametableId, tileId] = PositionToTileid(y, x);
		auto [attributeIdx, paletteIdx] = PositionToAttributeTable(y, x);

		uint32_t addr = NAMETABLE_BASE + nametableId * NAME_TABLE_AND_ATTRIBUTE_TABLE_SINGLE_SIZE + tileId;
		// ネームテーブル / 属性テーブルがある領域のアドレスに収まっててほしい
		assert(addr < 0x3F00);

		uint8_t spriteNum = m_pPpuBus->ReadByte(static_cast<uint16_t>(addr));
		// 背景のスプライトは 8 * 8 なので、 8 で割った余りで offset が出る
		int offsetY = y % 8;
		int offsetX = x % 8;

		uint8_t patternTableLower = m_pPpuBus->ReadByte(spriteNum * 16 + offsetY);
		uint8_t patternTableUpper = m_pPpuBus->ReadByte(spriteNum * 16 + offsetY + 8);

		int bitPos = 7 - offsetX;

		uint8_t colorLower = (patternTableLower & (1 << bitPos)) == (1 << bitPos);
		uint8_t colorUpper = (patternTableUpper & (1 << bitPos)) == (1 << bitPos);
		colorUpper <<= 1;

		uint8_t color = colorLower | colorUpper;
		assert(color <= 3);

		// attribute table からパレットの id を求める
		uint32_t attributeTableAddr = NAMETABLE_BASE + nametableId * NAME_TABLE_AND_ATTRIBUTE_TABLE_SINGLE_SIZE + NAME_TABLE_SINGLE_SIZE + attributeIdx;
		assert(attributeTableAddr < 0x3F00);

		uint8_t attributeTable = m_pPpuBus->ReadByte(static_cast<uint16_t>(attributeTableAddr));

		// attribute table から paletteIdx 番目の値を取り出す
		uint8_t paletteId = attributeTable & (0b11 << paletteIdx);
		paletteId >>= (paletteIdx * 2);

		// palette[paletteId][color] が実際に絵として現れる色
		const uint16_t PaletteSize = 4;
		uint8_t ret = m_pPpuBus->ReadByte(PALETTE_BASE + PaletteSize * paletteId + color);
		return ret;
	}


	void Ppu::BuildBackGroundLine()
	{
		// TODO: スクロール(y と x に SCROLLレジスタの値を足すとか)

		int y = m_Lines;

		// VBLANK にはなんもしない
		if (y >= 240) 
		{
			return;
		}

		for (int x = 0; x < PPU_OUTPUT_X; x++)
		{
			m_PpuOutput[y][x] = GetBackGroundPixelColor(y, x);
		}
	}

	void Ppu::GetPpuOutput(uint8_t pOutBuffer[PPU_OUTPUT_Y][PPU_OUTPUT_X])
	{
		for (int y = 0; y < PPU_OUTPUT_Y; y++) {
			for (int x = 0; x < PPU_OUTPUT_X; x++) {
				pOutBuffer[y][x] = m_PpuOutput[y][x];
			}
		}
	}
	
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
		m_pPpuBus->WriteByte(m_VramAddr, data);
		m_VramAddr += GetVramOffset();
	}

	uint8_t Ppu::ReadPpuStatus()
	{
		// 2回読みフラグをリセット
		m_IsVerticalScrollVal = false;
		m_IsLowerPpuAddr = false;
		m_IsValidPpuAddr = false;

		// VBLANK フラグ クリアする前に値を保持しておく
		uint8_t ret = PPUSTATUS;

		// VBlank フラグをクリア
		SetVBlankFlag(false);

		return ret;
	}
	uint8_t Ppu::ReadPpuData()
	{
		uint8_t buffered = m_VramReadBuf;
		if (m_VramAddr >= PALETTE_BASE)
		{
			// パレットテーブルは即時読み出し、 "下"にあるネームテーブルのミラーがバッファに入る
			m_VramReadBuf = m_pPpuBus->ReadByte(m_VramAddr, true);
			uint8_t ret = m_pPpuBus->ReadByte(m_VramAddr);
			m_VramAddr += GetVramOffset();

			return ret;
		}
		else
		{
			m_VramReadBuf = m_pPpuBus->ReadByte(m_VramAddr);
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

	void Ppu::GetPpuInfo(int* pLines, int* pCycles)
	{
		*pLines = m_Lines;
		*pCycles = m_Cycles;
	}
}}