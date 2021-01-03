#include <cassert>
#include <fstream>
#include <testUtil.h>

namespace test {
	std::optional<std::filesystem::path> GetRepositoryRootPath()
	{
		std::filesystem::path cur = std::filesystem::current_path();
		std::optional<std::filesystem::path> ret = std::nullopt;

		while (true)
		{
			// .EENES_ROOT を見つけたらそこが ROOT
			for (const auto& file : std::filesystem::directory_iterator(cur))
			{
				if (!file.is_directory() && file.path().filename().string() == ".EENES_ROOT")
				{
					return cur;
				}
			}

			std::filesystem::path parent = cur.parent_path();
			if (parent == cur) {
				break;
			}
			cur = parent;
		}
		return ret;
	}

	void ReadFile(std::filesystem::path path, std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize) 
	{
		// エラーハンドリングはサボっている
		std::uintmax_t size = std::filesystem::file_size(path);
		std::shared_ptr<uint8_t[]> buf(new(uint8_t[size]));

		std::ifstream ifs(path, std::ios::binary);
		assert(ifs);

		ifs.read(reinterpret_cast<char*>(buf.get()), size);

		*pOutBuf = buf;
		*pOutSize = size;
	}

	void LogCpuStatusFceuxStyle(nes::detail::CpuInfo* info, uint64_t cycles, uint64_t instructions)
	{
		printf("c");
		printf("%-12llu", cycles);
		printf("i");
		printf("%-12llu", instructions);
		printf("A:%02hhX X:%02hhX Y:%02hhX S:%02hhX ", info->A, info->X, info->Y, info->SP);

		std::string regs = "czidbuvn";
		std::string res;
		for (int i = 0; i < 8; i++)
		{
			if (((1 << i) & info->P) && i != 5)
			{
				res = static_cast<char>(toupper(regs[i])) + res;
			}
			else
			{
				res = regs[i] + res;
			}
		}

		std::cout << "P:" << res << "  ";
		printf("$%X: ", info->PC);

		for (int i = 0; i < info->m_Instruction.m_Bytes; i++) {
			printf("%02hhX ", info->m_InstructionBytes[i]);
		}
		printf("\n");
	}

	void LogCpuStatusNesTestStyle(nes::detail::CpuInfo* info, uint64_t cycles, uint64_t instructions)
	{
		printf("%X  ", info->PC);
		for (int i = 0; i < 3; i++) {
			if (i < info->m_Instruction.m_Bytes)
			{
				printf("%02hhX ", info->m_InstructionBytes[i]);
			}
			else
			{
				printf("   ");
			}
		}
		printf("                                 ");
		printf("A:%02hhX X:%02hhX Y:%02hhX P:%2hhX SP:%02hhX CYC:%llu\n", info->A, info->X, info->Y, info->P, info->SP, cycles);
	}
}