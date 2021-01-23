
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include "testUtil.h"

namespace {
	// HELLO WORLD する ROM 読み込む
	void ReadHelloWorldNes(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
	{
		auto rootPath = test::GetRepositoryRootPath();
		assert(rootPath);

		auto nesFile = rootPath.value();
		nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";

		test::ReadFile(nesFile, pOutBuf, pOutSize);
	}
}

void Main()
{
	// 雑 NES エミュ 準備
	std::shared_ptr<uint8_t[]> rom;
	size_t size;
	ReadHelloWorldNes(&rom, &size);
	nes::detail::System sys(rom.get(), size);
	nes::detail::PpuSystem ppuSys;
	nes::detail::PpuBus ppuBus(&sys, &ppuSys);
	nes::detail::Ppu ppu(&ppuBus);
	nes::detail::CpuBus cpuBus(&sys, &ppu);

	nes::detail::Cpu cpu(&cpuBus);

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

	// window をファミコンサイズに
	Window::Resize(Size(256, 240));
	Window::SetTitle(U"nes");
	Image image(256, 240, Palette::White);
	DynamicTexture texture(image);

	while (System::Update())
	{
		StepFrame();
		for (int y = 0; y < 240; y++) {
			for (int x = 0; x < 256; x++) {
				// 今回は、 0x00, 0x0f, 0x20 しか色が現れないので、色変換をサボる
				switch (result[y][x])
				{
				case 0x00:
					image[y][x] = Color(0x80, 0x80, 0x80, 0xFF);
					break;
				case 0x0F:
					image[y][x] = Color(0x5, 0x5, 0x5, 0xFF);
					break;
				case 0x20:
					image[y][x] = Color(0xFF, 0xFF, 0xFF, 0xFF);
					break;
				default:
					break;
				}
			}
		}
		texture.fill(image);
		texture.draw();
	}
}

//
// = アドバイス =
// Debug ビルドではプログラムの最適化がオフになります。
// 実行速度が遅いと感じた場合は Release ビルドを試しましょう。
// アプリをリリースするときにも、Release ビルドにするのを忘れないように！
//
// 思ったように動作しない場合は「デバッグの開始」でプログラムを実行すると、
// 出力ウィンドウに詳細なログが表示されるので、エラーの原因を見つけやすくなります。
//
// = お役立ちリンク =
//
// OpenSiv3D リファレンス
// https://siv3d.github.io/ja-jp/
//
// チュートリアル
// https://siv3d.github.io/ja-jp/tutorial/basic/
//
// よくある間違い
// https://siv3d.github.io/ja-jp/articles/mistakes/
//
// サポートについて
// https://siv3d.github.io/ja-jp/support/support/
//
// Siv3D ユーザコミュニティ Slack への参加
// https://siv3d.github.io/ja-jp/community/community/
//
// 新機能の提案やバグの報告
// https://github.com/Siv3D/OpenSiv3D/issues
//
