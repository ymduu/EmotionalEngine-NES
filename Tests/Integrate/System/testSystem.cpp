#include <cassert>
#include <cassette.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <testUtil.h>
#include <System.h>
#include <Cpu.h>

// HELLO WORLD する ROM 読み込む
void ReadHelloWorldNes(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
{
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto nesFile = rootPath.value();
    nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";

    test::ReadFile(nesFile, pOutBuf, pOutSize);
}
// nestest.nes 読み込む
void ReadNesTestNes(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
{
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto nesFile = rootPath.value();
    nesFile += "/Tests/TestBinaries/nestest/nestest.nes";

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

// Hello World
void TestSystem_HelloWorld()
{
    std::cout << "====" << __FUNCTION__ << "====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::Cpu cpu(&sys);

    cpu.Interrupt(nes::detail::InterruptType::RESET);

    uint64_t clk = 7;
    uint64_t inst = 1;

    for (int i = 0; i < 175; i++) {
        auto info = cpu.GetCpuInfoForDebug();
        test::LogCpuStatusFceuxStyle(&info, clk, inst);
        clk += cpu.Run();
        inst++;
    }

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

// nestest.nes
void TestSystem_NesTest()
{
    //std::cout << "====" << __FUNCTION__ << "====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadNesTestNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::Cpu cpu(&sys);

    cpu.Interrupt(nes::detail::InterruptType::RESET);

    // CPU だけで実行するために、 PC を 0xC000 にセット
    cpu.SetPCForDebug(0xC000);

    uint64_t clk = 7;
    uint64_t inst = 1;

    // official 命令: 5003 まで、 unofficial 命令: 8991まで
    for (int i = 0; i < 8991; i++) {
        auto info = cpu.GetCpuInfoForDebug();
        test::LogCpuStatusNesTestStyle(&info, clk, inst);
        clk += cpu.Run();
        inst++;
    }

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

int main()
{
    //TestSystem_ReadWrite();
    //TestSystem_HelloWorld();
    TestSystem_NesTest();

    return 0;
}