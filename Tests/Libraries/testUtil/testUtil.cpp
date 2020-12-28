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
}