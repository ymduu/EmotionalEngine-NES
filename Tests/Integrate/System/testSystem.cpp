#include <cassert>
#include <cassette.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <testUtil.h>
#include <System.h>

// HELLO WORLD する ROM 読み込む
void ReadHelloWorldNes(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
{
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto nesFile = rootPath.value();
    nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";

    test::ReadFile(nesFile, pOutBuf, pOutSize);
}

// 各アドレスを読み書きする、とりあえず WRAM とカセット だけ
void TestSystem_ReadWrite()
{
    std::cout << "====" << __FUNCTION__ << "====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::CpuBus bus(&sys);

    // WRAM 読み書きできる
    // 初期値は 0
    assert(bus.ReadByte(0) == 0);
    // 書いてみる
    bus.WriteByte(0, 0x41);

    // 書いたものが読める
    assert(bus.ReadByte(0) == 0x41);
    // ミラー 読める
    assert(bus.ReadByte(0x0800) == 0x41);
    // ミラー 書ける
    bus.WriteByte(0x0800, 0x42);
    // ミラーに書き込んだものが元の場所からよめる
    assert(bus.ReadByte(0) == 0x42);

    // カセット 読める(PRG-ROM の 1 byte目)
    assert(bus.ReadByte(0x8000) == 0x78);
    // カセット 書ける(？)
    bus.WriteByte(0x8000, 0x10);
    // カセット 書いた結果が反映されてる
    assert(bus.ReadByte(0x8000) == 0x10);

    // RESET割り込み 読んでみる
    uint16_t reset = 0;
    reset |= static_cast<uint16_t>(bus.ReadByte(0xFFFD)) << 8;
    reset |= bus.ReadByte(0xFFFC);
    printf("Entry Point: 0x%hx\n", reset);

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

int main()
{
    TestSystem_ReadWrite();

    return 0;
}