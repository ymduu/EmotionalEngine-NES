#pragma once
#include <stdint.h>

namespace nes {
	const size_t PRG_ROM_MAX = 0x8000;
	const size_t CHR_ROM_MAX = 0x2000;

	// CPU Memory Map
	const size_t WRAM_SIZE = 0x800;
	const size_t PPU_REG_SIZE = 0x8;
	const size_t APU_IO_REG_SIZE = 0x20;

	// Base Addr
	const uint16_t PPU_REG_BASE = 0x2000;
	const uint16_t APU_IO_REG_BASE = 0x4000;
	const uint16_t CASSETTE_BASE = 0x4020;
	const uint16_t CASSETTE_PRG_ROM_BASE = 0x8000;

}