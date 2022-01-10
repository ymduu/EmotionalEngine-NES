#pragma once
#include <cassert>
#include <utility>
#include "Ppu.h"

namespace {
	// テーブルをひきひきするヘルパー関数
	// tile の id と attribute table の要素 -> パレット番号
	uint8_t GetPaletteId(int tileId, uint8_t attributeTableElement)
	{
		uint8_t higher = (tileId / 64) % 2 == 0 ? 0 : 1;
		uint8_t lower = (tileId / 2) % 2 == 0 ? 0 : 1;
		higher <<= 1;

		uint8_t paletteIdx = higher | lower;

		// attribute table から paletteIdx 番目の値を取り出す
		uint8_t paletteId = attributeTableElement & (0b11 << (paletteIdx * 2));
		paletteId >>= (paletteIdx * 2);

		return paletteId;
	}

	// TODO: この辺のヘルパー関数は(nametable の解決部分が)嘘なので使っているところを撲滅して消す、 Internal Register を使う実装にすればいらないはず
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

		// Attribute Table の 1要素 は 32 * 32 px に1つ対応するので、 1行に8個含まれる 
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
		int beforeCycles = m_Cycles;
		int beforeLines = m_Lines;
		
		// クロックの加算は BG 描画しながら行う
		DrawBackGround(clk);

		// Line 241 にきてたら NMI する
		if (m_Lines != beforeLines && m_Lines == 241)
		{
			// 画面が完成する直前に sprite 描画
			BuildSprites();
			// VBLANK フラグ立てる
			SetVBlankFlag(true);

			if (PPUCTRL & (1 << 7))
			{
				m_pPpuBus->GenerateCpuInterrupt();
			}
		}

		if (m_Lines < 240)
		{
			// 可視ラインのときだけ Sprite 0 hit を 1px ずつチェックする。[2, 257] cycle 目で判定する。
			// いまみてる line の beforecycles - 2 まで見終わってるはずなので、その次のピクセルから見る
			// line をまたいでる場合は start を 0 にする、 m_Cycles が 1以下 のとき end が負になるけど壊れないはず
			int start = beforeCycles + clk >= 341 ? 0 : std::max(0, beforeCycles - 2 + 1);
			int end = std::min(255, m_Cycles - 2);

			for (int x = start; x <= end; x++)
			{
				if (IsSprite0Hit(m_Lines, x))
				{
					PPUSTATUS |= PPUSTATUS_SPRITE_0_HIT;
				}
			}
		}
		// line 261の先頭で sprite 0 hit フラグ と VBlank フラグを折る
		if (m_Lines == PPU_OUTPUT_Y + PPU_VBLANK_Y - 1)
		{
			PPUSTATUS &= ~PPUSTATUS_SPRITE_0_HIT;
			SetVBlankFlag(false);
		}

		if (m_Lines == PPU_OUTPUT_Y + PPU_VBLANK_Y)
		{
			m_Lines = 0;
			return true;
		}
		return false;
	}

	SpriteSize Ppu::GetSpriteSize()
	{
		return (PPUCTRL & (1 << 5)) ? SpriteSize::SpriteSize_8x16 : SpriteSize::SpriteSize_8x8;
	}

	Sprite Ppu::GetSprite(int idx)
	{
		int offset = idx * sizeof(Sprite);
		// OAM からはみ出さない
		assert(offset + 3 < static_cast<int>(OAM_SIZE));

		return Sprite(m_Oam[offset], m_Oam[offset + 1], m_Oam[offset + 2], m_Oam[offset + 3]);
	}

	// TODO: マリオのスクロールは上手くいってるけど嘘なので PPU Internal Register に基づいた Sprite 0 hit に直す
	bool Ppu::IsSprite0Hit(int y, int x)
	{
		// Sprite 0 hit が発生しない条件に当てはまっているなら早期 return しちゃう
		bool enableClippingBg	  = (PPUMASK & PPUMASKS_ENABLE_BG_MASK)		== 0;
		bool enableClippingSprite = (PPUMASK & PPUMASKS_ENABLE_SPRITE_MASK) == 0;
		bool enableBg			  = (PPUMASK & PPUMASKS_ENABLE_BG)		    == PPUMASKS_ENABLE_BG;
		bool enableSprite		  = (PPUMASK & PPUMASKS_ENABLE_SPRITE)      == PPUMASKS_ENABLE_SPRITE;

		// クリッピング有効 or {背景 or スプライト描画無効ならスプライト0hitしない}
		if (enableClippingBg || enableClippingSprite || !enableBg || !enableSprite)
		{
			return false;
		}

		Sprite sprite0 = GetSprite(0);
		// OAM に格納されるy座標は -1 されてるので足す
		sprite0.y++;

		// 相対座標 計算
		int relativeY = y - sprite0.y;
		int relativeX = x - sprite0.x;

		// TORIAEZU: sprite size 8x8 のみ対応
		assert(GetSpriteSize() == SpriteSize::SpriteSize_8x8);

		if (GetSpriteSize() == SpriteSize::SpriteSize_8x8)
		{
			// 範囲外
			if (relativeX < 0 || relativeY < 0 || relativeX >= 8 || relativeY >= 8)
			{
				return false;
			}
			// 範囲内 なら pattern table 引く、今回ほしいのは 透明 or not だけなので second だけ見る
			bool isSpriteClear = GetSpritePixelColor(sprite0, relativeY, relativeX).second;
			bool isBgClear = GetBackGroundPixelColor(y, x).second;

			// 両方不透明なら hit
			return (!isSpriteClear && !isBgClear) ? true : false;
		}
		else 
		{
			return false;
		}
	}

	// スプライトの左上を原点とした座標を指定してテーブルを引いてスプライトの色と透明か否か(透明 = true)を取得する、テスト用に公開しておく
	std::pair<uint8_t, bool> Ppu::GetSpritePixelColor(Sprite sprite, int relativeY, int relativeX)
	{
		assert(relativeX < 8);
		assert(relativeY < 8);

		// 反転フラグを確認して立ってたら反転させる
		bool invHorizontal = ((1 << 6) & sprite.attribute) == (1 << 6);
		bool invVertical   = ((1 << 7) & sprite.attribute) == (1 << 7);

		if (invHorizontal)
		{
			relativeX = 7 - relativeX;
		}
		if (invVertical)
		{
			relativeY = 7 - relativeY;
		}

		uint8_t patternTableLower = m_pPpuBus->ReadByte(GetSpritePatternTableBase() + sprite.patternTableIdx * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + relativeY);
		uint8_t patternTableUpper = m_pPpuBus->ReadByte(GetSpritePatternTableBase() + sprite.patternTableIdx * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + 8 + relativeY);

		int bitPos = 7 - relativeX;

		uint8_t colorLower = (patternTableLower & (1 << bitPos)) == (1 << bitPos);
		uint8_t colorUpper = (patternTableUpper & (1 << bitPos)) == (1 << bitPos);
		colorUpper <<= 1;

		uint8_t color = colorLower | colorUpper;
		assert(color <= 3);

		// パレットの id は Sprite がもってる
		// カラーパレットの上位2bit(http://pgate1.at-ninja.jp/NES_on_FPGA/nes_ppu.htm#sprite) というのの意味がわからないが、sprite palette base が 0x3f10 で、そこからのオフセットを4bitで指定すると考えるとつじつまがあうのでそう考える
		// バグったら疑う(最悪)
		uint16_t spritePaletteBase = PALETTE_BASE + SPRITE_PALETTE_OFFSET;
		// attribute の下位 2 bit が sprite palette の index
		uint16_t spritePaletteId = sprite.attribute & 0b11;
		// 各所で得られる情報にしたがって or にしとく(spritePaletteId を sprite palette のインデックスとみなして + spritePaletteId * 4 としても意味はおなじはず)
		uint16_t spritePaletteAddr = spritePaletteBase | (spritePaletteId << 2);
		// color を足して実際に読むべきアドレスにする
		uint16_t colorAddr = spritePaletteAddr + color;

		uint8_t ret = m_pPpuBus->ReadByte(colorAddr);

		return std::make_pair(ret, color == 0);
	}

	// 画面4枚分の座標(つまり、描画座標ではない)を入力してそのピクセルの色を取得する、その色が透明かどうかも取得する
	std::pair<uint8_t, bool> Ppu::GetBackGroundPixelColor(int y, int x)
	{
		auto [nametableId, tileId] = PositionToTileid(y, x);
		// paletteIdx: Attribute Table 内の何番目のパレットを使うか？
		auto [attributeIdx, paletteIdx] = PositionToAttributeTable(y, x);

		uint32_t addr = NAMETABLE_BASE + nametableId * NAME_TABLE_AND_ATTRIBUTE_TABLE_SINGLE_SIZE + tileId;
		// ネームテーブル / 属性テーブルがある領域のアドレスに収まっててほしい
		assert(addr < 0x3F00);

		uint8_t spriteNum = m_pPpuBus->ReadByte(static_cast<uint16_t>(addr));
		// 背景のスプライトは 8 * 8 なので、 8 で割った余りで offset が出る
		int offsetY = y % 8;
		int offsetX = x % 8;

		uint8_t patternTableLower = m_pPpuBus->ReadByte(GetBGPatternTableBase() + spriteNum * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + offsetY);
		uint8_t patternTableUpper = m_pPpuBus->ReadByte(GetBGPatternTableBase() + spriteNum * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + offsetY + 8);

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
		uint8_t paletteId = attributeTable & (0b11 << (paletteIdx * 2));
		paletteId >>= (paletteIdx * 2);

		// palette[paletteId][color] が実際に絵として現れる色。 color == 0 のときは透明色なので、その情報もまとめて返す
		const uint16_t PaletteSize = 4;
		uint8_t ret = m_pPpuBus->ReadByte(PALETTE_BASE + PaletteSize * paletteId + color);
		return std::make_pair(ret, color == 0);
	}

	void Ppu::DrawBackGround(int clk)
	{
		// clk クロック PPU をすすめる
		for (int i = 0; i < clk; i++)
		{
			// 1 cycle に 1 px 描画することに注意する
			if (m_Cycles == 0)
			{
				// Line の最初で Fine X Scroll を適用する
				m_BGRelativeX = m_InternalReg.GetFineX();
			}

			int x = m_Cycles;
			int y = m_Lines;

			// 描画範囲内なら描画する
			if (y < PPU_OUTPUT_Y && x < PPU_OUTPUT_X)
			{
				uint16_t tileAddr		= m_InternalReg.GetTileAddress();
				uint16_t attributeAddr  = m_InternalReg.GetAttributeAddress();

				uint8_t attributeTable = m_pPpuBus->ReadByte(attributeAddr);

				// 使うパレットを特定する
				// 下位 10 bit が tile id 
				uint16_t tileId = tileAddr & 0b1111111111;
				uint8_t paletteId = GetPaletteId(tileId, attributeTable);

				// tile 内での相対座標
				int RelativeX = m_BGRelativeX;
				int RelativeY = m_InternalReg.GetFineY(PpuInternalRegistertarget::PpuInternalRegistertarget_v);

				// nametable 引き
				uint8_t spriteNum = m_pPpuBus->ReadByte(static_cast<uint16_t>(tileAddr));

				// pattern table 引き
				uint8_t patternTableLower = m_pPpuBus->ReadByte(GetBGPatternTableBase() + spriteNum * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + RelativeY);
				uint8_t patternTableUpper = m_pPpuBus->ReadByte(GetBGPatternTableBase() + spriteNum * static_cast<uint16_t>(PATTERN_TABLE_ELEMENT_SIZE) + RelativeY + 8);

				int bitPos = 7 - RelativeX;

				uint8_t colorLower = (patternTableLower & (1 << bitPos)) == (1 << bitPos);
				uint8_t colorUpper = (patternTableUpper & (1 << bitPos)) == (1 << bitPos);
				colorUpper <<= 1;

				uint8_t color = colorLower | colorUpper;
				assert(color <= 3);

				// palette[paletteId][color] が実際に絵として現れる色。 color == 0 のときは透明色
				const uint16_t PaletteSize = 4;
				uint8_t outColor = m_pPpuBus->ReadByte(PALETTE_BASE + PaletteSize * paletteId + color);

				m_PpuOutput[y][x] = outColor;
				m_IsBackgroundClear[y][x] = color == 0;

				uint8_t masterBg = m_pPpuBus->ReadByte(PALETTE_BASE);
				if (color == 0)
				{
					m_PpuOutput[y][x] = masterBg;
				}
			}

			// v 更新
			if (x == 257)
			{
				m_InternalReg.UpdateHorizontalV();
			}

			if (m_Lines == 261 && 280 <= x && x <= 304)
			{
				m_InternalReg.UpdateVerticalV();
			}

			// 描画座標に基づいて内部レジスタをインクリメントする
			// Line の 256 dot 目にきてたら Y インクリメント
			if (x == 256)
			{
				m_InternalReg.IncrementY();
			}

			// X の相対座標(fine X から始まる奴) が 8 になっていたら Coarse X をインクリメント
			// Between dot 328 of a scanline, and 256 of the next scanline ???(https://wiki.nesdev.com/w/index.php/PPU_scrolling#Between_dot_328_of_a_scanline.2C_and_256_of_the_next_scanline)
			// 境界あやしいかも
			//if (x >= 328 || x < 256) 
			if (x < 256)
			{
				m_BGRelativeX++;
				if (m_BGRelativeX == 8)
				{
					m_BGRelativeX = 0;
					m_InternalReg.IncrementCoarseX();
				}
			}

			m_Cycles++;
			// PPU サイクルは mod 341 で保持する(341 PPU cycles で 1 Line 描画されるので)
			if (m_Cycles >= 341)
			{
				m_Cycles %= 341;
				m_Lines++;
			}
		}
	}

	void Ppu::BuildSprites()
	{
		// OAM に保持できるスプライトは 64 個
		for (int i = 0; i < OAM_SIZE / sizeof(Sprite); i++)
		{
			Sprite sprite = GetSprite(i);

			int offsetY = sprite.y + 1;
			int offsetX = sprite.x;

			for (int ry = 0; ry < 8; ry++)
			{
				for (int rx = 0; rx < 8; rx++)
				{
					// はみ出してるならなんもしない
					if (ry + offsetY >= PPU_OUTPUT_Y || rx + offsetX >= PPU_OUTPUT_X) 
					{
						continue;
					}

					auto [color, isClear] = GetSpritePixelColor(sprite, ry, rx);
					if (isClear) {
						// 透明色ならなんもしない
						continue;
					}

					bool isFront = (sprite.attribute & (1 << 5)) == 0;
					if (isFront)
					{
						// front: 問答無用で描画
						m_PpuOutput[ry + offsetY][rx + offsetX] = color;
					}
					else if(m_IsBackgroundClear[ry + offsetY][rx + offsetX])
					{
						// back: 背景が透明なら描画
						m_PpuOutput[ry + offsetY][rx + offsetX] = color;
					}
				}
			}
		}
	}

	uint16_t Ppu::GetBGPatternTableBase()
	{
		// PPUCTRL[4] で背景パターンテーブルアドレスを分岐する
		return (PPUCTRL & 1 << 4) ? 0x1000 : 0x0000;
	}
	uint16_t Ppu::GetSpritePatternTableBase()
	{
		// PPUCTRL[3] でスプライトパターンテーブルアドレスを分岐する
		return (PPUCTRL & 1 << 3) ? 0x1000 : 0x0000;
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
		
		// 内部レジスタの nametable select に反映(この辺 見る https://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls)
		uint8_t arg = data & 0b11;
		m_InternalReg.SetNametableSelect(PpuInternalRegistertarget::PpuInternalRegistertarget_t, arg);
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
		// 普通は DMA されるらしいので、あまり叩かれないかも
		// OAMADDR インクリメントの根拠: http://pgate1.at-ninja.jp/NES_on_FPGA/nes_ppu.htm
		m_Oam[OAMADDR] = data;
		OAMADDR++;
	}
	void Ppu::WritePpuScroll(uint8_t data)
	{
		if (!m_InternalReg.GetW())
		{
			uint8_t coarseX = data >> 3;
			uint8_t fineX = data & 0b111;

			m_InternalReg.SetCoarseX(PpuInternalRegistertarget::PpuInternalRegistertarget_t, coarseX);
			m_InternalReg.SetFineX(fineX);
			m_InternalReg.SetW(true);
		}
		else
		{
			uint8_t coarseY = data >> 3;
			uint8_t fineY = data & 0b111;

			m_InternalReg.SetCoarseY(PpuInternalRegistertarget::PpuInternalRegistertarget_t, coarseY);
			m_InternalReg.SetFineY(PpuInternalRegistertarget::PpuInternalRegistertarget_t, fineY);
			m_InternalReg.SetW(false);
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

		// 内部レジスタにも一応反映させとく
		if (!m_InternalReg.GetW())
		{
			m_InternalReg.SetUpperPpuAddr(data);
		}
		else
		{
			m_InternalReg.SetLowerPpuAddr(data);
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
		m_InternalReg.SetW(false);
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

	// PPU 内部レジスタアクセサー
	void PpuInternalRegister::SetCoarseX(PpuInternalRegistertarget target, uint8_t data)
	{
		// 下位 5 bit のみ有効
		assert((0b11100000 & data) == 0);
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			t &= ~COARSE_X_MASK;
			t |= data;
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			v &= ~COARSE_X_MASK;
			v |= data;
		}
	}
	void PpuInternalRegister::SetCoarseY(PpuInternalRegistertarget target, uint8_t data)
	{
		// 下位 5 bit のみ有効
		assert((0b11100000 & data) == 0);
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			t &= ~COARSE_Y_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 5;
			t |= writeData;
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			v &= ~COARSE_Y_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 5;
			v |= writeData;
		}
	}
	void PpuInternalRegister::SetNametableSelect(PpuInternalRegistertarget target, uint8_t data)
	{
		// 下位 2 bit のみ有効
		assert((0b11111100 & data) == 0);
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			t &= ~NAMETABLE_SELECT_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 10;
			t |= writeData;
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			v &= ~NAMETABLE_SELECT_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 10;
			v |= writeData;
		}
	}
	void PpuInternalRegister::SetFineY(PpuInternalRegistertarget target, uint8_t data)
	{
		// 下位 3 bit のみ有効
		assert((0b11111000 & data) == 0);
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			t &= ~FINE_Y_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 12;
			t |= writeData;
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			v &= ~FINE_Y_MASK;
			uint16_t writeData = static_cast<uint16_t>(data) << 12;
			v |= writeData;
		}
	}
	void PpuInternalRegister::SetFineX(uint8_t data)
	{
		// 下位 3 bit のみ有効
		assert((0b11111000 & data) == 0);
		x = data;
	}
	void PpuInternalRegister::SetW(bool data)
	{
		w = data;
	}
	void PpuInternalRegister::SetUpperPpuAddr(uint8_t data)
	{
		// 上 2 bit をマスクする(https://wiki.nesdev.com/w/index.php/PPU_scrolling#Register_controls)
		data &= 0b00111111;
		uint16_t writeData = static_cast<uint16_t>(data) << 8;
		t &= 0xFF;
		t |= writeData;
		w = true;
	}
	void PpuInternalRegister::SetLowerPpuAddr(uint8_t data)
	{
		// 下位8bit を更新するだけ
		t &= 0xFF00;
		t |= data;
		v = t;

		w = false;
	}

	uint8_t PpuInternalRegister::GetCoarseX(PpuInternalRegistertarget target)
	{
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			return static_cast<uint8_t>(t & COARSE_X_MASK);
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			return static_cast<uint8_t>(v & COARSE_X_MASK);
		}
		else
		{
			// unexpected default
			abort();
		}
	}
	uint8_t PpuInternalRegister::GetCoarseY(PpuInternalRegistertarget target)
	{
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			return static_cast<uint8_t>((t & COARSE_Y_MASK) >> 5);
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			return static_cast<uint8_t>((v & COARSE_Y_MASK) >> 5);
		}
		else
		{
			// unexpected default
			abort();
		}
	}
	uint8_t PpuInternalRegister::GetNametableSelect(PpuInternalRegistertarget target)
	{
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			return static_cast<uint8_t>((t & NAMETABLE_SELECT_MASK) >> 10);
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			return static_cast<uint8_t>((v & NAMETABLE_SELECT_MASK) >> 10);
		}
		else
		{
			// unexpected default
			abort();
		}
	}
	uint8_t PpuInternalRegister::GetFineY(PpuInternalRegistertarget target)
	{
		if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_t)
		{
			return static_cast<uint8_t>((t & FINE_Y_MASK) >> 12);
		}
		else if (target == PpuInternalRegistertarget::PpuInternalRegistertarget_v)
		{
			return static_cast<uint8_t>((v & FINE_Y_MASK) >> 12);
		}
		else
		{
			// unexpected default
			abort();
		}
	}
	uint8_t PpuInternalRegister::GetFineX()
	{
		return x;
	}
	bool PpuInternalRegister::GetW()
	{
		return w;
	}


	// 描画中のインクリメント(https://wiki.nesdev.com/w/index.php/PPU_scrolling#Wrapping_around)
	void PpuInternalRegister::IncrementCoarseX()
	{
		uint8_t coarseX = GetCoarseX(PpuInternalRegistertarget::PpuInternalRegistertarget_v);
		// tile 31 の次は 0 にもどす、上で実装したアクセサーは使わずに nesdev wiki の疑似コードをそのまま使っちゃう
		if (coarseX == 31)
		{
			v &= ~0x001F;          // coarse X = 0
			v ^= 0x0400;           // switch horizontal nametable
		}
		else
		{
			v++;
		}
	}

	void PpuInternalRegister::IncrementY()
	{
		// こちらも nesdev wiki の疑似コードをそのまま使っちゃう
		if ((v & 0x7000) != 0x7000)				// if fine Y < 7
		{
			v += 0x1000;						// increment fine Y
		}
		else
		{
			v &= ~0x7000;						// fine Y = 0
			int y = (v & 0x03E0) >> 5;			// let y = coarse Y
			if (y == 29)
			{
				y = 0;                          // coarse Y = 0
				v ^= 0x0800;                    // switch vertical nametable
			}
			else if (y == 31)
			{
				y = 0;                          // coarse Y = 0, nametable not switched
			}
			else 
			{
				y += 1;							// increment coarse Y
			}
			v = (v & ~0x03E0) | (y << 5);		// put coarse Y back into v
		}
	}

	void PpuInternalRegister::UpdateHorizontalV()
	{
		const uint16_t HORIZONTAL_MASK = 0b000010000011111;
		v &= ~HORIZONTAL_MASK;
		uint16_t update = t & HORIZONTAL_MASK;

		v |= update;
	}
	void PpuInternalRegister::UpdateVerticalV()
	{
		const uint16_t VERTICAL_MASK = 0b111101111100000;
		v &= ~VERTICAL_MASK;
		uint16_t update = t & VERTICAL_MASK;

		v |= update;
	}

	// 現在のタイルと attribute table のアドレス取得(https://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching)
	uint16_t PpuInternalRegister::GetTileAddress()
	{
		return static_cast<uint16_t>(0x2000 | (v & 0x0FFF));
	}
	uint16_t PpuInternalRegister::GetAttributeAddress()
	{
		return static_cast<uint16_t>(0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
	}
}}