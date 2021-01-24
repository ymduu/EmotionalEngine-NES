#include <cassert>
#include <cassette.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <testUtil.h>
#include <System.h>
#include <Cpu.h>
#include <Nes.h>
#include <set>

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
    std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::PpuSystem ppuSys;
    nes::detail::PpuBus ppuBus(&sys, &ppuSys);
    nes::detail::Ppu ppu(&ppuBus);

    nes::detail::CpuBus bus(&sys, &ppu);

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
    std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::PpuSystem ppuSys;
    nes::detail::PpuBus ppuBus(&sys, &ppuSys);
    nes::detail::Ppu ppu(&ppuBus);
    nes::detail::CpuBus cpuBus(&sys, &ppu);

    nes::detail::Cpu cpu(&cpuBus);
    ppuBus.Initialize(&cpu);

    cpu.Interrupt(nes::detail::InterruptType::RESET);

    uint64_t clk = 7;
    uint64_t inst = 1;

    for (int i = 0; i < 175; i++) {
        // ログ出したい場合、アンコメント
        //auto info = cpu.GetCpuInfoForDebug();
        //test::LogCpuStatusFceuxStyle(&info, clk, inst);
        clk += cpu.Run();
        inst++;
    }

    auto finalInfo = cpu.GetCpuInfoForDebug();

    assert(finalInfo.A == 0x1E);
    assert(finalInfo.X == 0x0D);
    assert(finalInfo.Y == 0x00);
    assert(finalInfo.P == 0x24);
    assert(finalInfo.SP == 0x00FF);
    assert(finalInfo.PC == 0x804E);
    assert(clk == 525);

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

void TestSystem_HelloWorld_Cpu_Ppu()
{
    std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::PpuSystem ppuSys;
    nes::detail::PpuBus ppuBus(&sys, &ppuSys);
    nes::detail::Ppu ppu(&ppuBus);
    nes::detail::CpuBus cpuBus(&sys, &ppu);

    nes::detail::Cpu cpu(&cpuBus);
    ppuBus.Initialize(&cpu);

    cpu.Interrupt(nes::detail::InterruptType::RESET);

    uint64_t clk = 7;
    uint64_t inst = 1;

    uint8_t result[240][256];

    // CPU を 1フレーム分動かして、 result を更新する
    auto StepFrame = [&]() {
        bool calculated = false;
        while (!calculated)
        {
            int add = cpu.Run();
            clk += add;
            calculated = ppu.Run(add * 3);
            inst++;
        }
        ppu.GetPpuOutput(result);
    };

    StepFrame();

    // 期待値と比較
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto txt = rootPath.value();
    txt += "/Tests/TestBinaries/helloworld/expected.txt";

    std::ifstream ifs(txt);
    assert(ifs);

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 256; x++) {
            int expected;
            ifs >> expected;

            assert(static_cast<uint8_t>(expected) == result[y][x]);
        }
    }

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

// PPU テストのテストケース作成
void CreateTestCase_TestSystem_HelloWorld_Cpu_Ppu()
{
    std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadHelloWorldNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::PpuSystem ppuSys;
    nes::detail::PpuBus ppuBus(&sys, &ppuSys);
    nes::detail::Ppu ppu(&ppuBus);
    nes::detail::CpuBus cpuBus(&sys, &ppu);

    nes::detail::Cpu cpu(&cpuBus);
    ppuBus.Initialize(&cpu);


    cpu.Interrupt(nes::detail::InterruptType::RESET);

    uint64_t clk = 7;
    uint64_t inst = 1;

    uint8_t result[240][256];

    // CPU を 1フレーム分動かして、 result を更新する
    auto StepFrame = [&]() {
        bool calculated = false;
        while (!calculated)
        {
            int add = cpu.Run();
            clk += add;
            calculated = ppu.Run(add * 3);
            inst++;
        }
        ppu.GetPpuOutput(result);
    };

    StepFrame();
    // result をテキストファイルに出力
    auto rootPath = test::GetRepositoryRootPath();
    assert(rootPath);

    auto txt = rootPath.value();
    txt += "/Tests/TestBinaries/helloworld/expected.txt";

    std::ofstream ofs(txt);
    assert(ofs);

    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 256; x++) {
            ofs << static_cast<int>(result[y][x]) << (x == 255 ? "\n" : " ");
        }
    }

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

// nestest.nes
void TestSystem_NesTest()
{
    std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadNesTestNes(&rom, &size);
    nes::detail::System sys(rom.get(), size);
    nes::detail::PpuSystem ppuSys;
    nes::detail::PpuBus ppuBus(&sys, &ppuSys);
    nes::detail::Ppu ppu(&ppuBus);
    nes::detail::CpuBus cpuBus(&sys, &ppu);

    nes::detail::Cpu cpu(&cpuBus);
    ppuBus.Initialize(&cpu);

    cpu.Interrupt(nes::detail::InterruptType::RESET);

    // CPU だけで実行するために、 PC を 0xC000 にセット
    cpu.SetPCForDebug(0xC000);

    uint64_t clk = 7;
    uint64_t inst = 1;

    // official 命令: 5003 まで、 unofficial 命令: 8991まで
    for (int i = 0; i < 8991; i++) {
        // ログ出したい時はコメントアウトを外す
        //auto info = cpu.GetCpuInfoForDebug();
        //test::LogCpuStatusNesTestStyle(&info, clk, inst);
        clk += cpu.Run();
        inst++;
    }

    // TORIAEZU: レジスタ状態とサイクル数だけあってればよしとする
    auto finalInfo = cpu.GetCpuInfoForDebug();

    assert(finalInfo.A == 0x00);
    assert(finalInfo.X == 0xFF);
    assert(finalInfo.Y == 0x15);
    assert(finalInfo.P == 0x27);
    assert(finalInfo.SP == 0x00FF);
    assert(finalInfo.PC == 1);
    assert(clk == 26560);

    std::cout << "==== " << __FUNCTION__ << " OK ====\n";
}

void TestSystem_NesTest_Emulator()
{
    //std::cout << "==== " << __FUNCTION__ << " ====\n";
    std::shared_ptr<uint8_t[]> rom;
    size_t size;
    ReadNesTestNes(&rom, &size);

    nes::Emulator emu(rom, size);

    // 雑に 70000 命令くらいステップする
    for (int i = 0; i < 70000; i++) {
        nes::EmuInfo info;
        emu.GetEmuInfo(&info);
        test::LogEmuStatusNintendulatorStyle(&info);

        emu.Step();
    }

    std::cout << "====" << __FUNCTION__ << " END ====\n";
}

int main()
{
    // テストケース生成したい時だけコメントアウトをもどす
    //CreateTestCase_TestSystem_HelloWorld_Cpu_Ppu();

    //TestSystem_ReadWrite();
    //TestSystem_HelloWorld();
    //TestSystem_NesTest();

    //TestSystem_HelloWorld_Cpu_Ppu();
    TestSystem_NesTest_Emulator();
    return 0;
}