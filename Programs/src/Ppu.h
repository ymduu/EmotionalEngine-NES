#pragma once
#include <utility>
#include <stdint.h>
#include "constants.h"
#include "System.h"

namespace nes { namespace detail {
	class PpuBus;
	class CpuBus;

	struct Sprite
	{
		Sprite()
			: y(0)
			, patternTableIdx(0)
			, attribute(0)
			, x(0)
		{}

		Sprite(uint8_t aY, uint8_t aPatternTableIdx, uint8_t aAttribute, uint8_t aX)
			: y(aY)
			, patternTableIdx(aPatternTableIdx)
			, attribute(aAttribute)
			, x(aX)
		{}

		uint8_t y;
		uint8_t patternTableIdx;
		uint8_t attribute;
		uint8_t x;
	};

	enum class PpuInternalRegistertarget
	{
		PpuInternalRegistertarget_v,
		PpuInternalRegistertarget_t
	};

	// PPU 内部レジスタへのアクセスを提供するクラス
	class PpuInternalRegister
	{
	public:
		PpuInternalRegister()
			: v(0)
			, t(0)
			, x(0)
			, w(false)
		{}

		void SetCoarseX(PpuInternalRegistertarget target, uint8_t data);
		void SetCoarseY(PpuInternalRegistertarget target, uint8_t data);
		void SetNametableSelect(PpuInternalRegistertarget target, uint8_t data);
		void SetFineY(PpuInternalRegistertarget target, uint8_t data);
		void SetFineX(uint8_t data);

		uint8_t GetCoarseX(PpuInternalRegistertarget target);
		uint8_t GetCoarseY(PpuInternalRegistertarget target);
		uint8_t GetNametableSelect(PpuInternalRegistertarget target);
		uint8_t GetFineY(PpuInternalRegistertarget target);
		uint8_t GetFineX();

		// 描画中のインクリメント
		void IncrementCoarseX();
		void IncrementY();

		// 現在のタイルと attribute table のアドレス取得
		uint16_t GetTileAddress();
		uint16_t GetAttributeAddress();

	private:
		// v, t : 15 bit
		uint16_t v;
		uint16_t t;
		// x: 3 bit
		uint8_t x;
		bool w;

		// constants
		const uint16_t NAMETABLE_SELECT_MASK = 0b000110000000000;
		const uint16_t COARSE_X_MASK		 = 0b000000000011111;
		const uint16_t COARSE_Y_MASK		 = 0b000001111100000;
		const uint16_t FINE_Y_MASK			 = 0b111000000000000;
	};

	enum class SpriteSize 
	{
		SpriteSize_8x8,
		SpriteSize_8x16,
	};

	class Ppu {
		// CPU バスからの書き込みを許す
		friend CpuBus;
	public:
		// PPU レジスタは不思議な副作用がたくさんあるので、それを実現できるようにすべてアクセサーでアクセスすることにする
		void WritePpuCtrl(uint8_t data);
		void WritePpuMask(uint8_t data);
		void WriteOamAddr(uint8_t data);
		void WriteOamData(uint8_t data);
		void WritePpuScroll(uint8_t data);
		void WritePpuAddr(uint8_t data);
		void WritePpuData(uint8_t data);

		uint8_t ReadPpuStatus();
		uint8_t ReadPpuData();

		// レジスタ読み書き 終

		// クロックを与えてそのクロックだけ PPU を進める、1画面分処理したら true が返る
		bool Run(int clk);

		// 座標を指定してテーブルを引いて背景色と透明か否か(透明 = true)を取得する、テスト用に公開しておく
		std::pair<uint8_t, bool> GetBackGroundPixelColor(int y, int x);

		// スプライトの左上を原点とした座標を指定してテーブルを引いてスプライトの色と透明か否か(透明 = true)を取得する、テスト用に公開しておく
		std::pair<uint8_t, bool> GetSpritePixelColor(Sprite sprite, int relativeY, int relativeX);

		// PPU の絵をバッファにかきこむ
		void GetPpuOutput(uint8_t pOutBuffer[PPU_OUTPUT_Y][PPU_OUTPUT_X]);

		// PPUの情報を取得
		void GetPpuInfo(int* pLines, int* pCycles);

		Ppu(PpuBus* pPpuBus)
			: PPUCTRL(0)
			, PPUMASK(0)
			, PPUSTATUS(0)
			, OAMADDR(0)
			, PPUSCROLL(0)
			, PPUADDR(0)
			, PPUDATA(0)
			, OAMDMA(0)
			, m_VramReadBuf(0)
			, m_pPpuBus(pPpuBus)
			, m_IsLowerPpuAddr(false)
			, m_IsValidPpuAddr(false)
			, m_VramAddr(0)
			, m_IsVerticalScrollVal(false)
			, m_ScrollX(0)
			, m_ScrollY(0)
			, m_Lines(0)
			, m_Cycles(0)
			, m_Oam{}
			, m_PpuOutput{ {} }
			, m_IsBackgroundClear{ {} }
		{}

	private:
		// 例によってレジスタは命名規則に従わないことにします
		// 0x2000
		uint8_t PPUCTRL;
		// 0x2001
		uint8_t PPUMASK;
		// 0x2002
		uint8_t PPUSTATUS;
		// 0x2003
		uint8_t OAMADDR;
		// 0x2004 = OAMDATA は OAM(Sprite RAM) に書かれるので保持しない
		// 0x2005
		uint8_t PPUSCROLL;
		// 0x2006
		uint8_t PPUADDR;
		// 0x2007
		uint8_t PPUDATA;
		// 0x4014
		uint8_t OAMDMA;

		uint8_t m_VramReadBuf;
		// PPU Bus 経由で VRAM を読み書きする
		PpuBus* m_pPpuBus;

		// 描画用内部レジスタ(https://wiki.nesdev.com/w/index.php/PPU_scrolling#PPU_internal_registers)
		PpuInternalRegister m_InternalReg;

		// 2度書き用フラグシリーズ
		// PPUADDR は上位バイト -> 下位バイトの順にかきこみ
		bool m_IsLowerPpuAddr;
		// PPUADDR への2回書き込みが完了しているか？
		bool m_IsValidPpuAddr;
		uint16_t m_VramAddr;
		// PPUSCROLL は水平スクロール値 -> 垂直スクロール値 の順に書き込み(NES on FPGA と NesDev で言ってることが逆だが……)
		bool m_IsVerticalScrollVal;
		uint8_t m_ScrollX;
		uint8_t m_ScrollY;

		int m_Lines;
		int m_Cycles;
		
		// コントロールレジスタ 読み書き系
		uint16_t GetVramOffset();
		void SetVBlankFlag(bool flag);

		//　背景を 1 Line 分描画する
		void BuildBackGroundLine();

		// スプライト を全部描画する
		void BuildSprites();

		// パターンテーブルのベースアドレスを取得
		uint16_t GetBGPatternTableBase();
		uint16_t GetSpritePatternTableBase();

		// スプライトサイズを取得
		SpriteSize GetSpriteSize();

		// インデックスを指定して OAM からスプライトを一つ取得する
		Sprite GetSprite(int idx);

		// Sprite 0 hit してるか？
		bool IsSprite0Hit(int y, int x);

		// PPU は 256 byte の Object Attribute Memory(Sprite を書き込む場所)をもつ
		uint8_t m_Oam[OAM_SIZE];

		// PPU の出力(絵)。 Ppu に持たせるのが適切か若干微妙だけどとりあえずここ
		uint8_t m_PpuOutput[PPU_OUTPUT_Y][PPU_OUTPUT_X];

		// 背景が透明ピクセルか？
		bool m_IsBackgroundClear[PPU_OUTPUT_Y][PPU_OUTPUT_X];
	};
}}