
# include <Siv3D.hpp> // OpenSiv3D v0.4.3
#include "testUtil.h"
#include "Nes.h"

namespace {
	// HELLO WORLD する ROM 読み込む
	void ReadHelloWorldNes(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
	{
		auto rootPath = test::GetRepositoryRootPath();
		assert(rootPath);

		auto nesFile = rootPath.value();
		//nesFile += "/Tests/TestBinaries/helloworld/sample1/sample1.nes";
		//nesFile += "/Tests/TestBinaries/nestest/nestest.nes";
		//nesFile += "/Tests/TestBinaries/giko/giko011.nes";
		//nesFile += "/Tests/TestBinaries/rom/donkey.nes";
		nesFile += "/Tests/TestBinaries/rom/mario.nes";

		test::ReadFile(nesFile, pOutBuf, pOutSize);
	}
}

void Main()
{
	// 雑 NES エミュ 準備
	std::shared_ptr<uint8_t[]> rom;
	size_t size;
	ReadHelloWorldNes(&rom, &size);

	nes::Emulator emu(rom, size);

	// window をファミコンサイズに
	Window::Resize(Size(256, 240));
	Window::SetTitle(U"nes");
	Image image(256, 240, Palette::White);
	DynamicTexture texture(image);

	nes::Color result[240][256];

	while (System::Update())
	{
		// キー入力
		if (KeyLeft.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::LEFT);
		}
		if (KeyRight.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::RIGHT);
		}
		if (KeyUp.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::UP);
		}
		if (KeyDown.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::DOWN);
		}

		if (KeyLeft.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::LEFT);
		}
		if (KeyRight.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::RIGHT);
		}
		if (KeyUp.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::UP);
		}
		if (KeyDown.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::DOWN);
		}

		if (KeyZ.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::A);
		}
		if (KeyZ.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::A);
		}
		if (KeyX.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::B);
		}
		if (KeyX.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::B);
		}
		if (KeyS.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::SELECT);
		}
		if (KeyS.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::SELECT);
		}
		if (KeyA.down())
		{
			emu.PushButton(nes::PadId::Zero, nes::PadButton::START);
		}
		if (KeyA.up())
		{
			emu.ReleaseButton(nes::PadId::Zero, nes::PadButton::START);
		}

		emu.StepFrame();
		emu.GetPicture(result);
		for (int y = 0; y < 240; y++) {
			for (int x = 0; x < 256; x++) {
				auto& c = result[y][x];
				image[y][x] = Color(c.Red, c.Green, c.Blue, 0xFF);
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
