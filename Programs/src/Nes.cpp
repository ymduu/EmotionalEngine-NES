#include <cassert>
#include <cstdlib>
#include <string.h>

#include <Nes.h>

namespace {
	// �F�ϊ��e�[�u��
	// ref: https://qiita.com/bokuweb/items/1575337bef44ae82f4d3#%E3%83%91%E3%83%AC%E3%83%83%E3%83%88
	nes::Color g_Color[0x40] = {
		{0x80, 0x80, 0x80} ,{0x00, 0x3D, 0xA6},{0x00, 0x12, 0xB0},{0x44, 0x00, 0x96},
		{0xA1, 0x00, 0x5E},{0xC7, 0x00, 0x28},{0xBA, 0x06, 0x00},{0x8C, 0x17, 0x00},
		{0x5C, 0x2F, 0x00},{0x10, 0x45, 0x00},{0x05, 0x4A, 0x00},{0x00, 0x47, 0x2E},
		{0x00, 0x41, 0x66},{0x00, 0x00, 0x00},{0x05, 0x05, 0x05},{0x05, 0x05, 0x05},
		{0xC7, 0xC7, 0xC7},{0x00, 0x77, 0xFF},{0x21, 0x55, 0xFF},{0x82, 0x37, 0xFA},
		{0xEB, 0x2F, 0xB5},{0xFF, 0x29, 0x50},{0xFF, 0x22, 0x00},{0xD6, 0x32, 0x00},
		{0xC4, 0x62, 0x00},{0x35, 0x80, 0x00},{0x05, 0x8F, 0x00},{0x00, 0x8A, 0x55},
		{0x00, 0x99, 0xCC},{0x21, 0x21, 0x21},{0x09, 0x09, 0x09},{0x09, 0x09, 0x09},
		{0xFF, 0xFF, 0xFF},{0x0F, 0xD7, 0xFF},{0x69, 0xA2, 0xFF},{0xD4, 0x80, 0xFF},
		{0xFF, 0x45, 0xF3},{0xFF, 0x61, 0x8B},{0xFF, 0x88, 0x33},{0xFF, 0x9C, 0x12},
		{0xFA, 0xBC, 0x20},{0x9F, 0xE3, 0x0E},{0x2B, 0xF0, 0x35},{0x0C, 0xF0, 0xA4},
		{0x05, 0xFB, 0xFF},{0x5E, 0x5E, 0x5E},{0x0D, 0x0D, 0x0D},{0x0D, 0x0D, 0x0D},
		{0xFF, 0xFF, 0xFF},{0xA6, 0xFC, 0xFF},{0xB3, 0xEC, 0xFF},{0xDA, 0xAB, 0xEB},
		{0xFF, 0xA8, 0xF9},{0xFF, 0xAB, 0xB3},{0xFF, 0xD2, 0xB0},{0xFF, 0xEF, 0xA6},
		{0xFF, 0xF7, 0x9C},{0xD7, 0xE8, 0x95},{0xA6, 0xED, 0xAF},{0xA2, 0xF2, 0xDA},
		{0x99, 0xFF, 0xFC},{0xDD, 0xDD, 0xDD},{0x11, 0x11, 0x11},{0x11, 0x11, 0x11},
	};
}

namespace nes { 

	void Emulator::StepFrame()
	{
		bool finished = false;
		while (!finished)
		{
			int add = m_Cpu.Run();
			m_ClockCount += add;
			finished = m_Ppu.Run(add);
			m_InstructionCount++;
		}
	}
	Color Emulator::GetColor(uint8_t color)
	{
		return g_Color[color];
	}

	// �e�[�u���ϊ��O�̊G���擾
	void Emulator::GetPicture(uint8_t pBuffer[240][256])
	{
		m_Ppu.GetPpuOutput(pBuffer);
	}
	// �e�[�u���ϊ���̊G���擾
	void Emulator::GetPicture(Color pBuffer[240][256])
	{
		uint8_t raw[240][256];
		m_Ppu.GetPpuOutput(raw);

		for (int y = 0; y < 240; y++) 
		{
			for (int x = 0; x < 256; x++)
			{
				pBuffer[y][x] = GetColor(raw[y][x]);
			}
		}
	}
}