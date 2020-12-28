#include <cassert>
#include <cstdlib>
#include <string.h>

#include "./constants.h"
#include "./cassette.h"

namespace nes { namespace detail {
	void Cassette::Initialize(uint8_t* pBuffer, size_t bufferSize)
	{
		// iNES ヘッダーを読み込み
		assert(bufferSize >= 16);
		memcpy(&m_Header, pBuffer, sizeof(INESHeader));

		// マジックナンバー 確認
		assert(!memcmp(m_Header.m_Magic, "NES\x1A", 4));
		
		// TODO: バッファサイズのチェック

		// PROGRAM ROM 切り出し
		size_t prgRomSize = static_cast<size_t>(m_Header.m_PrgRomSize) * PRG_ROM_UNIT;
		memcpy(m_PrgRom, pBuffer + sizeof(INESHeader), prgRomSize);
		// CHARACTER ROM 切り出し
		size_t chrRomSize = static_cast<size_t>(m_Header.m_ChrRomSize) * CHR_ROM_UNIT;
		memcpy(m_ChrRom, pBuffer + sizeof(INESHeader) + prgRomSize, chrRomSize);

		m_PrgRomSize = prgRomSize;
		m_ChrRomSize = chrRomSize;
	
		m_Initialized = true;
	}

	void Cassette::ReadPrgRom(int offset, uint8_t* pBuffer, size_t size)
	{
		assert(m_Initialized);
		assert(offset + size <= m_PrgRomSize);
		memcpy(pBuffer, m_PrgRom + offset, size);
	}
	void Cassette::WritePrgRom(int offset, const uint8_t* pBuffer, size_t size) 
	{
		assert(m_Initialized);
		assert(offset + size <= m_PrgRomSize);
		memcpy(m_PrgRom + offset, pBuffer, size);
	}
	void Cassette::ReadChrRom(int offset, uint8_t* pBuffer, size_t size)
	{
		assert(m_Initialized);
		assert(offset + size <= m_ChrRomSize);
		memcpy(pBuffer, m_ChrRom + offset, size);
	}
	void Cassette::WriteChrRom(int offset, const uint8_t* pBuffer, size_t size)
	{
		assert(m_Initialized);
		assert(offset + size <= m_ChrRomSize);
		memcpy(m_ChrRom + offset, pBuffer, size);
	}

}}