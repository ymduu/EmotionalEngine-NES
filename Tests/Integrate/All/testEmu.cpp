#include <testUtil.h>
#include "DxLib.h"
#include "Nes.h"

namespace {
	// HELLO WORLD する ROM 読み込む
	void ReadRom(std::shared_ptr<uint8_t[]>* pOutBuf, size_t* pOutSize)
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

	void DrawEmulatedPicture(int softImage, nes::Color result[][256])
	{
		BYTE* p = (BYTE*)GetImageAddressSoftImage(softImage);

		for (int y = 0; y < 240 * 2; y++) {
			for (int x = 0; x < 256 * 2; x++) {
				auto& c = result[y/2][x/2];
				// B, G, R の順番 https://dxlib.xsrv.jp/cgi/patiobbs/patio.cgi?mode=view&no=4620&p=2
				p[0] = c.Blue;
				p[1] = c.Green;
				p[2] = c.Red;

				// 1 px は 3 byte
				p += 3;
			}
		}

		DrawSoftImage(0, 0, softImage);
		//DrawSoftImage(256, 0, softImage);
		//DrawSoftImage(0, 240, softImage);
		//DrawSoftImage(256, 240, softImage);
	}

	// キー入力をエミュに入れる
	void InputKey(nes::Emulator* emu)
	{
		char keyInput[256];
		GetHitKeyStateAll(keyInput);

		// ほんとは入力の立ち上がり立ち下がりを検出したいが、エミュの実装上そうしなくてもいいのでサボる(定数倍遅くなるけど)
		if (keyInput[KEY_INPUT_LEFT] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::LEFT);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::LEFT);
		}

		if (keyInput[KEY_INPUT_RIGHT] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::RIGHT);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::RIGHT);
		}

		if (keyInput[KEY_INPUT_UP] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::UP);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::UP);
		}

		if (keyInput[KEY_INPUT_DOWN] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::DOWN);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::DOWN);
		}

		if (keyInput[KEY_INPUT_Z] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::A);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::A);
		}

		if (keyInput[KEY_INPUT_X] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::B);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::B);
		}

		if (keyInput[KEY_INPUT_S] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::SELECT);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::SELECT);
		}

		if (keyInput[KEY_INPUT_A] != 0) 
		{
			emu->PushButton(nes::PadId::Zero, nes::PadButton::START);
		}
		else 
		{
			emu->ReleaseButton(nes::PadId::Zero, nes::PadButton::START);
		}
	}
}

// プログラムは WinMain から始まります
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	// 雑 NES エミュ 準備
	std::shared_ptr<uint8_t[]> rom;
	size_t size;
	ReadRom(&rom, &size);

	nes::Emulator emu(rom, size);

	if (ChangeWindowMode(true) != DX_CHANGESCREEN_OK) 
	{
		return -1;
	}

    if( DxLib_Init() == -1) 
	{
        return -1 ;
    }

    
	// 描画バッファ
	nes::Color result[240][256];

	// dxlib で動的にイメージを描画するためにつかうやつ
	auto softImage = MakeRGB8ColorSoftImage(256 * 2, 240 * 2);

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	while (1) 
	{
		if (ProcessMessage() != 0) 
		{ // メッセージ処理
			break;//ウィンドウの×ボタンが押されたらループを抜ける
		}

		InputKey(&emu);
		emu.StepFrame();
		emu.GetPicture(result);

		DrawEmulatedPicture(softImage, result);

		ScreenFlip();
	}

    WaitKey();
    DxLib_End();
    return 0;
}