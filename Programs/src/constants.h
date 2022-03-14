#pragma once
#include <stdint.h>

namespace nes {
	// マッパー次第で バンク 16 個まであるかも
	const size_t PRG_ROM_MAX = 0x8000 * 16;
	const size_t CHR_ROM_MAX = 0x2000;

	// CPU Memory Map
	const size_t WRAM_SIZE = 0x800;
	const size_t PPU_REG_SIZE = 0x8;
	const size_t APU_IO_REG_SIZE = 0x20;

	// PPU Memory Map
	// TORIAEZU: ミラーリングは考慮しないで4画面分とっておく
	const size_t NAMETABLE_SIZE = 0x1000;
	const size_t PALETTE_SIZE = 0x20;
	// Nametable と Attribute Tableのひとつぶんのサイズ
	const size_t NAME_TABLE_SINGLE_SIZE = 0x3C0;
	const size_t ATTRIBUTE_TABLE_SINGLE_SIZE = 0x40;
	// ネームテーブルをインデックス指定するときに必要……
	const size_t NAME_TABLE_AND_ATTRIBUTE_TABLE_SINGLE_SIZE = NAME_TABLE_SINGLE_SIZE + ATTRIBUTE_TABLE_SINGLE_SIZE;
	// パターンテーブルをインデックス指定するときに必要(パターンテーブル1要素のサイズは16byte)
	const size_t PATTERN_TABLE_ELEMENT_SIZE = 16;

	// OAM(Object Attribute Memory / Sprite Memory)
	const size_t OAM_SIZE = 256;

	// Base Addr
	const uint16_t PPU_REG_BASE = 0x2000;
	const uint16_t APU_IO_REG_BASE = 0x4000;
	const uint16_t CASSETTE_BASE = 0x4020;
	const uint16_t CASSETTE_PRG_ROM_BASE = 0x8000;

	// PPU Base Addr
	const uint16_t NAMETABLE_BASE = 0x2000;
	// 0x3000-0x3EFF にネームテーブル 4 枚分のミラー
	const uint16_t NAMETABLE_MIRROR_BASE = 0x3000;
	const uint16_t PALETTE_BASE = 0x3F00;

	// PPU Offset
	// PALETTE_BASE から スプライトパレットのベースアドレスへのオフセット
	const uint16_t SPRITE_PALETTE_OFFSET = 0x10;

	// PPU Output Size
	const size_t PPU_OUTPUT_X = 256;
	const size_t PPU_OUTPUT_Y = 240;
	const size_t PPU_VBLANK_Y = 22;


	// masks
	const uint8_t B_FLAG_MASK = 0b00110000;

	// ppu masks(0x2001)
	const uint8_t PPUMASKS_DISPLAY_TYPE			= 1 << 0;
	const uint8_t PPUMASKS_ENABLE_BG_MASK		= 1 << 1;
	const uint8_t PPUMASKS_ENABLE_SPRITE_MASK	= 1 << 2;
	const uint8_t PPUMASKS_ENABLE_BG			= 1 << 3;
	const uint8_t PPUMASKS_ENABLE_SPRITE		= 1 << 4;
	const uint8_t PPUMASKS_BG_COLOR				= 0b111 << 5;

	// ppu status(0x2002)
	const uint8_t PPUSTATUS_SPRITE_OVERFLOW = 1 << 5;
	const uint8_t PPUSTATUS_SPRITE_0_HIT    = 1 << 6;
	const uint8_t PPUSTATUS_VBLANK			= 1 << 7;
}