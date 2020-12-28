#include <cassert>
#include <cassette.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <testUtil.h>

int main()
{
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto nesFile = rootPath.value();
    nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";

    std::shared_ptr<uint8_t[]> nesData;
    size_t size;

    test::ReadFile(nesFile, &nesData, &size);

    nes::detail::Cassette cst;
    cst.Initialize(nesData.get(), size);

    // TORIAEZU: PRG ROM から HELLO WORLD! がよめたらオッケー
    char buf[16];
    cst.ReadPrgRom(97, reinterpret_cast<uint8_t*>(buf), 15);
    std::cout << buf << "\n";

    char prgRom[2 * 16 * 1024];
    char chrRom[8 * 1024];

    // とりあえず全域が読めればオッケーとしておく
    cst.ReadChrRom(0, reinterpret_cast<uint8_t*>(chrRom), 8 * 1024);
    cst.ReadPrgRom(0, reinterpret_cast<uint8_t*>(prgRom), 2 * 16 * 1024);

    return 0;
}