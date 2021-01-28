#include <filesystem>
#include <optional>

#include <Cpu.h>
#include <Nes.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>

namespace test {
	std::optional<std::filesystem::path> GetRepositoryRootPath();
	void ReadFile(std::filesystem::path path, std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize);

	void LogCpuStatusFceuxStyle(nes::detail::CpuInfo* info, uint64_t cycles, uint64_t instructions);
	void LogCpuStatusNesTestStyle(nes::detail::CpuInfo* info, uint64_t cycles, uint64_t instructions);

	// Emulator ‚Ìî•ñ‘S•”æ‚é
	void LogEmuStatusNintendulatorStyle(nes::EmuInfo* info);
}
