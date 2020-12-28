#include <filesystem>
#include <optional>

namespace test {
	std::optional<std::filesystem::path> GetRepositoryRootPath();
	void ReadFile(std::filesystem::path path, std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize);
}
