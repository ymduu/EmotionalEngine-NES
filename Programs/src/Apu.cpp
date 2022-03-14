#pragma once
#include <cassert>
#include <utility>
#include "Apu.h"
#include "constants.h"


namespace {
	int g_LengthTable[32] = { 0 };
	int g_NoiseFreqTable[16] = { 0 };
	int g_DmcFreqTable[16] = { 0 };

	// コンストラクタでグローバルなテーブルを初期化するイニシャライザ
	class Initializer {
	public:
		Initializer()
		{
			// 値のソース http://pgate1.at-ninja.jp/NES_on_FPGA/nes_apu.htm
			g_LengthTable[0b00000] = 0x0A;
			g_LengthTable[0b00010] = 0x14;
			g_LengthTable[0b00100] = 0x28;
			g_LengthTable[0b00110] = 0x50;
			g_LengthTable[0b01000] = 0xA0;
			g_LengthTable[0b01010] = 0x3C;
			g_LengthTable[0b01100] = 0x0E;
			g_LengthTable[0b01110] = 0x1A;
			g_LengthTable[0b10000] = 0x0C;
			g_LengthTable[0b10010] = 0x18;
			g_LengthTable[0b10100] = 0x30;
			g_LengthTable[0b10110] = 0x60;
			g_LengthTable[0b11000] = 0xC0;
			g_LengthTable[0b11010] = 0x48;
			g_LengthTable[0b11100] = 0x10;
			g_LengthTable[0b11110] = 0x20;
			g_LengthTable[0b00001] = 0xFE;
			g_LengthTable[0b00011] = 0x02;
			g_LengthTable[0b00101] = 0x04;
			g_LengthTable[0b00111] = 0x06;
			g_LengthTable[0b01001] = 0x08;
			g_LengthTable[0b01011] = 0x0A;
			g_LengthTable[0b01101] = 0x0C;
			g_LengthTable[0b01111] = 0x0E;
			g_LengthTable[0b10001] = 0x10;
			g_LengthTable[0b10011] = 0x12;
			g_LengthTable[0b10101] = 0x14;
			g_LengthTable[0b10111] = 0x16;
			g_LengthTable[0b11001] = 0x18;
			g_LengthTable[0b11011] = 0x1A;
			g_LengthTable[0b11101] = 0x1C;
			g_LengthTable[0b11111] = 0x1E;

			// ノイズ、値のソースは NES on FPGA
			g_NoiseFreqTable[0] = 0x004;
			g_NoiseFreqTable[1] = 0x008;
			g_NoiseFreqTable[2] = 0x010;
			g_NoiseFreqTable[3] = 0x020;
			g_NoiseFreqTable[4] = 0x040;
			g_NoiseFreqTable[5] = 0x060;
			g_NoiseFreqTable[6] = 0x080;
			g_NoiseFreqTable[7] = 0x0A0;
			g_NoiseFreqTable[8] = 0x0CA;
			g_NoiseFreqTable[9] = 0x0FE;
			g_NoiseFreqTable[10] = 0x17C;
			g_NoiseFreqTable[11] = 0x1FC;
			g_NoiseFreqTable[12] = 0x2FA;
			g_NoiseFreqTable[13] = 0x3F8;
			g_NoiseFreqTable[14] = 0x7F2;
			g_NoiseFreqTable[15] = 0xFE4;

			// DMC、値のソースは以下略
			g_DmcFreqTable[0] = 0x1AC;
			g_DmcFreqTable[1] = 0x17C;
			g_DmcFreqTable[2] = 0x154;
			g_DmcFreqTable[3] = 0x140;
			g_DmcFreqTable[4] = 0x11E;
			g_DmcFreqTable[5] = 0x0FE;
			g_DmcFreqTable[6] = 0x0E2;
			g_DmcFreqTable[7] = 0x0D6;
			g_DmcFreqTable[8] = 0x0BE;
			g_DmcFreqTable[9] = 0x0A0;
			g_DmcFreqTable[0xA] = 0x08E;
			g_DmcFreqTable[0xB] = 0x080;
			g_DmcFreqTable[0xC] = 0x06A;
			g_DmcFreqTable[0xD] = 0x054;
			g_DmcFreqTable[0xE] = 0x048;
			g_DmcFreqTable[0xF] = 0x036;
		}
	};
	Initializer g_Initializer;

	// 7457 分周 == [0, 7457) のカウンタをつかう
	constexpr int ClocksToNextSequence = 7456;
}

namespace nes { namespace detail {
	void SquareWaveChannel::WriteRegister(uint8_t value, uint16_t addr)
	{
		uint16_t offset = addr - m_BaseAddr;

		switch (offset) {
		case 0 :
			m_DutyTable = m_DutyTables[value >> 6];
			m_DecayLoop = (value >> 5) & 1;
			m_LengthEnabled = !m_DecayLoop;
			m_DecayEnabled = ((value >> 4) & 1) == 0;
			m_DecayV = value & 0b1111;
			break;
		case 1:
			m_SweepTimer = (value >> 4) & 0b111;
			m_SweepNegate = (value >> 3) & 1;
			m_SweepShift = value & 0b111;
			m_SweepReload = true;
			m_SweepEnabled = ((value >> 7) & 1) == 1 && m_SweepShift != 0;
			break;
		case 2:
			// m_FreqTImer の上位3bitだけ残してクリア
			m_FreqTimer &= 0b11100000000;
			// m_FreqTimer の 下位8bitを更新
			m_FreqTimer |= value;
			break;
		case 3:
		{
			// m_FreqTimer の上位3 bit をクリア
			m_FreqTimer &= 0b11111111;
			uint16_t hi = value & 0b111;
			m_FreqTimer |= (hi << 8);

			// length Counter 更新
			// 書き込み値の上位5bitが table のインデックス
			int tableIndex = value &  0b11111000;
			tableIndex >>= 3;
			m_LengthCounter = g_LengthTable[tableIndex];

			m_FreqCounter = m_FreqTimer;
			m_DutyCounter = 0;
			m_DecayResetFlag = true;
			break;
		}
		default:
			break;
		}
	}

	void SquareWaveChannel::On4015Write(uint8_t value)
	{
		// 最下位1bit
		bool channelEnabled = false;

		if (m_IsChannel1) 
		{
			channelEnabled = value & 1;
		}
		else 
		{
			// 1(0-indexed)bit目
			channelEnabled = (value >> 1) & 1;
		}

		m_ChannelEnabled = channelEnabled;
		if (!m_ChannelEnabled) {
			m_LengthCounter = 0;
		}
	}

	uint8_t SquareWaveChannel::GetStatusBit()
	{
		return m_LengthCounter != 0 ? 1 : 0;
	}

	void SquareWaveChannel::ClockTimer() 
	{
		// タイマ(音の周波数 ≒ 高さ を指定するためのもの)をクロック
		if (m_FreqCounter > 0) 
		{
			m_FreqCounter--;
		}
		else 
		{
			m_FreqCounter = m_FreqTimer;
			// 波は 8 区分あって、インデックスは mod 8 で計算する(NES on FPGA の 1番目のレジスタによってデューディサイクルが設定されます。 シーケンサはタイマから励起され、次のような波形を出力します。 のとこ)
			m_DutyCounter = (m_DutyCounter + 1) & 7;
		}
	}

	void SquareWaveChannel::ClockQuarterFrame()
	{
		// コメントはだいたい NES on FPGA に準拠
		// フレームシーケンサによって励起されるとき、 最後のクロック以降チャンネルの4番目のレジスタへの書き込みがあった場合、 カウンタへ$Fをセットし、分周器へエンベロープ周期をセットします
		if (m_DecayResetFlag) 
		{
			m_DecayResetFlag = false;
			m_DecayHiddenVol = 0xf;

			// decay_counter == エンベロープ周期(分周器でつかうもの)
			// この if にはいるときの1回 + else の時が dacay_V 回なので、周期は decay_v+1になるよね(NES on FPGA)
			m_DecayCounter = m_DecayV;
		}
		else 
		{
			// そうでなければ、分周器を励起します。
			// カウンタ = decay_hidden_vol であってる？(たぶんあってると思う)
			// 特定条件でカウンタの値が volume になるからこの名前なのかも。
			if (m_DecayCounter > 0) 
			{
				m_DecayCounter--;
			}
			else 
			{
				m_DecayCounter = m_DecayV;
				// 分周器が励起されるとき、カウンタがゼロでなければデクリメントします
				if (m_DecayHiddenVol > 0) 
				{
					m_DecayHiddenVol--;
				}
				else if (m_DecayLoop)
				{
					// カウンタが0で、ループフラグがセットされているならカウンタへ$Fをセットします。
					m_DecayHiddenVol = 0xf;
				}
			}
		}
	}

	void SquareWaveChannel::ClockHalfFrame() 
	{
		// NES on FPGA でいうところのスイープユニット
		// $4001 書き込みで sweep_reload がたつ(NES on FPGA と食い違ってる？)
		// TODO: 裏取り だけど、 sweep のレジスタは $4001 なので、これであってる気もする
		if (m_SweepReload) 
		{
			m_SweepCounter = m_SweepTimer;
			m_SweepReload = false;
		}
		else if (m_SweepCounter > 0) 
		{
			// 分周器がクロックを出してないならなんもしない
			m_SweepCounter--;
		}
		else 
		{
			// 分周器のカウンタをもとにもどす
			m_SweepCounter = m_SweepTimer;

			// NES on FPGA において、 IsSweepForcingSilence は もしチャンネルの周期が8未満か、$7FFより大きくなったなら、スイープを停止し、 チャンネルを無音化します。 にあたる処理
			if (m_SweepEnabled && !IsSweepForcingSilence()) 
			{
				if (m_SweepNegate) 
				{
					// m_SweepShift は $4001 の下位3bit
					// モードによって適切な方法で周波数 = 音階を更新
					m_FreqTimer -= (m_FreqTimer >> m_SweepShift) + m_IsChannel1;
				}
				else 
				{
					m_FreqTimer += (m_FreqTimer >> m_SweepShift);
				}
			}
		}

		// 長さカウンタのクロック生成(NES on FPGA の l)
		if (m_LengthEnabled && m_LengthCounter > 0) 
		{
			m_LengthCounter--;
		}
	}

	bool SquareWaveChannel::IsSweepForcingSilence()
	{
		// チャンネルの周期が8未満
		if (m_FreqTimer < 8) 
		{
			return true;
		}
		else if (!m_SweepNegate && (m_FreqTimer + (m_FreqTimer >> m_SweepShift) >= 0x800)) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}

	int SquareWaveChannel::GetOutPut() 
	{
		if (m_DutyTable[m_DutyCounter] && m_LengthCounter != 0 && !IsSweepForcingSilence()) 
		{
			if (m_DecayEnabled) 
			{
				// decay 有効 or not は $4000 の 4 bit 目できまる
				return m_DecayHiddenVol;
			}
			else 
			{
				// decay_V は $4000 の下位4bit(0123bit目)できまる4bitのあたい
				// NES on FPGA  エンベロープジェネレータ の
				// チャンネルのボリューム出力として、 エンベロープ無効フラグがセットされているなら、 エンベロープ周期のnをそのまま出力します。 クリアされているならカウンタの値を出力します相当
				// 結局、エンベロープ無効なら $4000 の下位 4 bit がボリュームになって、有効ならカウンタの値 = decay_hidden_vol がボリュームになるとのこと
				return m_DecayV;
			}
		}
		else 
		{
			return 0;
		}
	}

	// -------------------- 三角波 --------------------
	void TriangleWaveChannel::WriteRegister(uint8_t value, uint16_t addr)
	{
		uint16_t offset = addr - m_BaseAddr;
		switch (offset) {
		case 0:
			// $4008 の 7 bit 目は長さカウンタフラグ
			m_LinearControl = ((value >> 7) & 1) == 1;
			m_LengthEnabled = !m_LinearControl;
			// 線形カウンタのロード値
			m_LinearLoad = value & 0b1111111;
			break;
		case 1:
			break;
		case 2:
			// $400A
			// m_FreqTimer の上位3bitだけ残してクリア
			m_FreqTimer &= 0b11100000000;
			// m_FreqTimer の 下位8bitを更新
			m_FreqTimer |= value;
			break;
		case 3:
		{
			// $400B
			// m_FreqTimer の上位3 bit をクリア
			m_FreqTimer &= 0b11111111;
			uint16_t hi = value & 0b111;
			m_FreqTimer |= (hi << 8);

			// length Counter 更新
			// 書き込み値の上位5bitが table のインデックス
			int tableIndex = value & 0b11111000;
			tableIndex >>= 3;

			if (m_ChannelEnabled) 
			{
				m_LengthCounter = g_LengthTable[tableIndex];
			}

			m_LinearReload = true;
			break;
		}
		default:
			break;
		}
	}

	void TriangleWaveChannel::On4015Write(uint8_t value)
	{
		// 2(0-indexed)bit目
		bool channelEnabled = false;
		channelEnabled = (value >> 2) & 1;

		m_ChannelEnabled = channelEnabled;
		if (!m_ChannelEnabled) 
		{
			m_LengthCounter = 0;
		}
	}

	uint8_t TriangleWaveChannel::GetStatusBit()
	{
		return m_LengthCounter != 0 ? 1 : 0;
	}

	int TriangleWaveChannel::GetOutPut()
	{
		return m_OutputVal;
	}

	void TriangleWaveChannel::ClockTimer()
	{
		// タイマをクロック、その値によって三角波チャンネルをクロック
		bool ultraSonic = false;

		if (m_FreqTimer < 2 && m_FreqCounter == 0) 
		{
			ultraSonic = true;
		}

		bool clockTriUnit = true;

		if (m_LengthCounter == 0) 
		{
			clockTriUnit = false;
		}
		if (m_LinearCounter == 0) 
		{
			clockTriUnit = false;
		}
		if (ultraSonic) 
		{
			clockTriUnit = false;
		}

		if (clockTriUnit) 
		{
			if (m_FreqCounter > 0) 
			{
				m_FreqCounter--;
			}
			else 
			{
				m_FreqCounter = m_FreqTimer;
				// F E D C B A 9 8 7 6 5 4 3 2 1 0 0 1 2 3 4 5 6 7 8 9 A B C D E F のシーケンスを生成 するためのインデックスが m_TriStep
				m_TriStep = (m_TriStep + 1) & 0x1F;
			}
		}

		// TORIAEZU: ClockTimer の責務からは外れるが、三角波ユニットをクロックした直後の値で出力値を更新する
		if (ultraSonic) 
		{
			// Disch の疑似コードでは 7.5 って言ってるけど[0, F]の中心で止める、という意味なので7でもいいはず
			m_OutputVal = 7;
		}
		else if (m_TriStep & 0x10) 
		{
			// 0x10 のビットが立ってたら、そのビットを0にして、その下の4bitを反転することで F E D C B A 9 8 7 6 5 4 3 2 1 0 0 1 2 3 4 5 6 7 8 9 A B C D E F のシーケンスを生成
			// cf. http://pgate1.at-ninja.jp/NES_on_FPGA/nes_apu.htm の 三角波 のとこ
			m_OutputVal = m_TriStep ^ 0x1F;
		}
		else 
		{
			m_OutputVal = m_TriStep;
		}
	}

	void TriangleWaveChannel::ClockQuarterFrame() 
	{
		// 線形カウンタの処理
		if (m_LinearReload) 
		{
			// レジスタ$400Bへの書き込みによって、線形カウンタを停止し、カウンタへ音の長さをロードします(NES on FPGA)
			m_LinearCounter = m_LinearLoad;
		}
		else if (m_LinearCounter > 0) 
		{
			// (線形カウンタのコントロールフラグ(http://pgate1.at-ninja.jp/NES_on_FPGA/nes_apu.htm)がクリアされてたら？) && カウンタが0でなければデクリメント
			m_LinearCounter--;
		}
		if (!m_LinearControl) 
		{
			// TODO: 出典をしらべる
			m_LinearReload = false;
		}
	}

	void TriangleWaveChannel::ClockHalfFrame() 
	{
		// 長さカウンタのクロック生成
		if (m_LengthEnabled && m_LengthCounter > 0) {
			m_LengthCounter--;
		}
	}

	void NoiseChannel::WriteRegister(uint8_t value, uint16_t addr)
	{
		uint16_t offset = addr - m_BaseAddr;

		switch (offset) {
		case 0:
			// $400C
			m_DecayLoop = (value >> 5) & 1;
			m_LengthEnabled = !m_DecayLoop;
			m_DecayEnabled = ((value >> 4) & 1) == 0;
			m_DecayV = value & 0b1111;
			break;
		case 1:
			break;
		case 2:
		{
			// $400E
			uint8_t idx = value & 0b1111;
			m_FreqTimer = g_NoiseFreqTable[idx];
			// m_FreqTimer = 0x40;

			// ランダムモード生成フラグ
			m_ShiftMode = (value >> 7) & 1;
			break;
		}
		case 3:
		{
			if (m_ChannelEnabled) 
			{
				// length Counter 更新
				// 書き込み値の上位5bitが table のインデックス
				int tableIndex = value & 0b11111000;
				tableIndex >>= 3;
				m_LengthCounter = g_LengthTable[tableIndex];

				m_DecayResetFlag = true;
			}
			break;
		}
		default:
			break;
		}
	}

	void NoiseChannel::ClockTimer() 
	{
		if (m_FreqCounter > 0) 
		{
			m_FreqCounter--;
		}
		else 
		{
			m_FreqCounter = m_FreqTimer;

			// NES on FPGA:
			// 15ビットシフトレジスタにはリセット時に1をセットしておく必要があります。 
			// タイマによってシフトレジスタが励起されるたびに1ビット右シフトし、 ビット14には、ショートモード時にはビット0とビット6のEORを、 ロングモード時にはビット0とビット1のEORを入れます。
			int topBit = 0;

			if (m_ShiftMode) 
			{
				int shift6 = (m_NoiseShift >> 6) & 1;
				int shift0 = m_NoiseShift & 1;

				topBit = shift6 ^ shift0;
			}
			else 
			{
				int shift1 = (m_NoiseShift >> 1) & 1;
				int shift0 = m_NoiseShift & 1;

				topBit = shift1 ^ shift0;
			}

			// topBit を 15 bit目(0-indexed) にいれる
			m_NoiseShift &= ~(1 << 15);
			m_NoiseShift |= (topBit << 15);

			m_NoiseShift >>= 1;
		}

		// ClockTimer は 1 APU クロックごとに呼び出されるので出力値の決定もここでやる
		// シフトレジスタのビット0が1なら、チャンネルの出力は0となります。(NES on FPGA)
		// 長さカウンタが0でない ⇔ channel active
		if ((m_NoiseShift & 1) == 0 && m_LengthCounter != 0)
		{
			if (m_DecayEnabled) 
			{
				m_Output = m_DecayHiddenVol;
			}
			else 
			{
				m_Output = m_DecayV;
			}
		}
		else 
		{
			m_Output = 0;
		}
	}

	void NoiseChannel::ClockQuarterFrame()
	{
		// 矩形波のコピペだけど、共通化するのも違う気がするのでコピペのまま……
		// フレームシーケンサによって励起されるとき、 最後のクロック以降チャンネルの4番目のレジスタへの書き込みがあった場合、 カウンタへ$Fをセットし、分周器へエンベロープ周期をセットします
		if (m_DecayResetFlag) 
		{
			m_DecayResetFlag = false;
			m_DecayHiddenVol = 0xf;

			// decay_counter == エンベロープ周期(分周器でつかうもの)
			// この if にはいるときの1回 + else の時が dacay_V 回なので、周期は decay_v+1になるよね(NES on FPGA)
			m_DecayCounter = m_DecayV;
		}
		else 
		{
			// そうでなければ、分周器を励起します。
			// カウンタ = decay_hidden_vol であってる？(たぶんあってると思う)
			// 特定条件でカウンタの値が volume になるからこの名前なのかも。
			if (m_DecayCounter > 0) 
			{
				m_DecayCounter--;
			}
			else 
			{
				m_DecayCounter = m_DecayV;
				// 分周器が励起されるとき、カウンタがゼロでなければデクリメントします
				if (m_DecayHiddenVol > 0) 
				{
					m_DecayHiddenVol--;
				}
				else if (m_DecayLoop) 
				{
					// カウンタが0で、ループフラグがセットされているならカウンタへ$Fをセットします。
					m_DecayHiddenVol = 0xf;
				}
			}
		}
	}

	void NoiseChannel::ClockHalfFrame()
	{
		// 矩形波とちがってスイープユニットはない
		// 長さカウンタのクロック生成(NES on FPGA の l)
		if (m_LengthEnabled && m_LengthCounter > 0) 
		{
			m_LengthCounter--;
		}
	}

	int NoiseChannel::GetOutPut() 
	{
		return m_Output;
	}

	void NoiseChannel::On4015Write(uint8_t value)
	{
		// 3(0-indexed)bit目
		bool channelEnabled = false;
		channelEnabled = (value >> 3) & 1;

		m_ChannelEnabled = channelEnabled;
		if (!m_ChannelEnabled) 
		{
			m_LengthCounter = 0;
		}
	}

	uint8_t NoiseChannel::GetStatusBit()
	{
		return m_LengthCounter != 0 ? 1 : 0;
	}

	void DmcChannel::WriteRegister(uint8_t value, uint16_t addr) 
	{
		uint16_t offset = addr - m_BaseAddr;

		switch (offset) {
		case 0:
			m_DmcIrqEnabled = (value >> 7) & 1;
			m_DmcLoop = (value >> 6) & 1;
			m_FreqTimer = g_DmcFreqTable[value & 0b1111];
			break;
		case 1:
			// デルタカウンタの初期値
			m_Output = value & 0b01111111;
			break;
		case 2:
			// DMCサンプリングを開始するとき、 アドレスカウンタにはレジスタ$4012 * $40 + $C000をセット(NES on FPGA)
			m_AddrLoad = 0xC000 + static_cast<int>(value) * 0x40;
			break;
		case 3:
			// 残りバイトカウンタにはレジスタ$4013 * $10 + 1をセットします(NES on FPGA)
			m_LengthLoad = (static_cast<int>(value) * 0x10) + 1;
			break;
		default:
			break;
		}
	}

	void DmcChannel::On4015Write(uint8_t value)
	{
		if ((value >> 4) & 1) 
		{
			m_Length = m_LengthLoad;
			m_Addr = m_AddrLoad;
		}
		else 
		{
			m_Length = 0;
		}

		// 割り込みクリア
		m_DmcIrqPending = false;
	}

	void DmcChannel::GetStatusBit(uint8_t* pOutValue) 
	{
		uint8_t orValue = 0;
		if (m_Length > 0) 
		{
			orValue |= (1 << 4);
		}
		if (m_DmcIrqPending) 
		{
			orValue |= (1 << 7);
		}

		*pOutValue |= orValue;
	}

	uint64_t DmcChannel::ClockTimer()
	{
		uint64_t retCpuClock = 0;

		if (m_FreqCounter > 0) 
		{
			m_FreqCounter--;
		}
		else 
		{
			m_FreqCounter = m_FreqTimer;

			if (!m_OutputUnitSilent) 
			{
				// サイレンスフラグがクリアされていたら
				if ((m_OutputShift & 1) && m_Output < 0x7e) 
				{
					// デルタカウンタが 126 より小さいなら +2
					m_Output += 2;
				}
				if (!(m_OutputShift & 1) && m_Output > 1) 
				{
					// デルタカウンタが 1 より大きいなら -2
					m_Output -= 2;
				}
			}

			// シフトレジスタに入っている使用済みサンプルを捨てる
			m_BitsInOutputUnit--;
			m_OutputShift >>= 1;

			if (m_BitsInOutputUnit == 0) 
			{
				m_BitsInOutputUnit = 8;
				m_OutputShift = m_SampleBuffer;
				m_OutputUnitSilent = m_IsSampleBufferEmpty;
				m_IsSampleBufferEmpty = true;
			}

			// 必要なら DMA する
			if (m_Length > 0 && m_IsSampleBufferEmpty) 
			{
				retCpuClock = 4;

				m_SampleBuffer = m_pApu->DmaReadFromCpu(m_Addr);
				m_IsSampleBufferEmpty = false;

				m_Addr++;
				if (m_Addr > 0xFFFF) 
				{
					// 0xFFFF を超えてたら 0x8000 に丸める
					m_Addr = 0x8000;
				}

				m_Length--;

				if (m_Length == 0) 
				{
					if (m_DmcLoop) 
					{
						m_Length = m_LengthLoad;
						m_Addr = m_AddrLoad;
					}
					else if (m_DmcIrqEnabled) 
					{
						m_DmcIrqPending = true;
						m_pApu->GenerateCpuInterrupt();
					}
				}
			}
		}

		return retCpuClock;
	}

	int DmcChannel::GetOutPut() 
	{
		return m_Output;
	}

	void Apu::WriteRegister(uint8_t value, uint16_t addr)
	{
		// addr で各チャンネルに振り分け
		if (addr <= 0x4003)
		{
			// 矩形波チャンネル1
			m_SquareWaveChannel1.WriteRegister(value, addr);
		}
		else if (addr <= 0x4007) 
		{
			// 矩形波チャンネル2
			m_SquareWaveChannel2.WriteRegister(value, addr);
		}
		else if (addr <= 0x400B) 
		{
			// 三角波
			m_TriangleWaveChannel.WriteRegister(value, addr);
		}
		else if (addr <= 0x400F) 
		{
			// ノイズ
			m_NoiseChannel.WriteRegister(value, addr);
		}
		else if (addr <= 0x4013) 
		{
			// DMC
			m_DmcChannel.WriteRegister(value, addr);
		}
		else if (addr == 0x4015) 
		{
			// 全チャンネルに書き込みを反映
			m_SquareWaveChannel1.On4015Write(value);
			m_SquareWaveChannel2.On4015Write(value);
			m_TriangleWaveChannel.On4015Write(value);
			m_NoiseChannel.On4015Write(value);
			m_DmcChannel.On4015Write(value);
		}
		else if (addr == 0x4017) 
		{
			// 設定値をメンバに反映 & 各チャンネル駆動
			// value の 7 bit 目が立ってたら 5 step else 4step
			m_SequencerMode = (value & (1 << 7)) == (1 << 7) ? Mode_5Step : Mode_4Step;
			m_IrqEnabled = (value & (1 << 6)) >> 6;

			m_NextSeqPhase = 0;
			// フレームシーケンサのクロックは CPU クロックで計算する。(half clock を考慮するのをサボるため)
			m_SequencerCounter = ClocksToNextSequence;

			if (m_SequencerMode == Mode_5Step) {
				ClockQuarterFrame();
				ClockHalfFrame();
			}

			if (!m_IrqEnabled) {
				m_IrqPending = false;
			}
		}
	}

	uint8_t Apu::ReadRegister4015() 
	{
		uint8_t res = 0;

		uint8_t square1 = m_SquareWaveChannel1.GetStatusBit();
		res |= square1;

		uint8_t square2 = m_SquareWaveChannel2.GetStatusBit();
		res |= (square2 << 1);

		uint8_t triangle = m_TriangleWaveChannel.GetStatusBit();
		res |= (triangle << 2);

		uint8_t noise = m_NoiseChannel.GetStatusBit();
		res |= (noise << 3);

		// DMC だけは 1bit ではないので DMC 側に更新してもらう
		m_DmcChannel.GetStatusBit(&res);

		return res;
	}

	int Apu::GetOutPut() 
	{
		return m_OutputVal;
	}

	uint64_t Apu::Run(uint64_t cpuClock) 
	{
		uint64_t retCpuClock = 0;

		// cpuClock ぶんだけ APU うごかす
		for (int i = 0; i < cpuClock; i++) 
		{
			if (m_CpuClock % 2 == 0) 
			{
				// 1 APU サイクルごとに実行したい処理
				m_SquareWaveChannel1.ClockTimer();
				m_SquareWaveChannel2.ClockTimer();
				m_NoiseChannel.ClockTimer();
			}

			// 三角波 と DMC は 1 CPU クロックごとにタイマーをクロック
			m_TriangleWaveChannel.ClockTimer();
			retCpuClock += m_DmcChannel.ClockTimer();

			// clock frame sequencer
			// フレームシーケンサは CPU クロックベースで動く
			if (m_SequencerCounter > 0) 
			{
				m_SequencerCounter--;
			}
			else 
			{
				bool isHalfFrame, isQuarterFrame, isRaiseIrq;
				GetPhaseStatus(&isQuarterFrame, &isHalfFrame, &isRaiseIrq);

				if (isQuarterFrame) {
					ClockQuarterFrame();
				}
				if (isHalfFrame) {
					ClockHalfFrame();
				}
				if (isRaiseIrq) {
					// TODO: CpuBus を使って IRQ 割り込みを上げる デバッグ
					m_pApuBus->GenerateCpuInterrupt();
					m_IrqPending = true;
				}

				StepSeqPhase();
				m_SequencerCounter = ClocksToNextSequence;
			}

			// 出力値の決定 (1 APU クロックごと)
			if (m_CpuClock % 2 == 0) {
				// TODO: ちゃんとミックスする
				m_OutputVal = 0;
				m_OutputVal += m_SquareWaveChannel1.GetOutPut();
				m_OutputVal += m_SquareWaveChannel2.GetOutPut();
				m_OutputVal += m_TriangleWaveChannel.GetOutPut();
				m_OutputVal += m_NoiseChannel.GetOutPut();
				// TORIAEZU: DMC だけ 7bit なのでほかと同じように4bitに丸める
				m_OutputVal += (m_DmcChannel.GetOutPut() >> 3);
			}

			// 40 or 41 クロックごとにコールバック関数で音を出力
			if (m_AddWaveSampleCounter >= m_AddWaveSampleCounterMax) 
			{
				m_AddWaveSampleCounter = 0;
				m_AddWaveSampleCounterMax == 40 ? 41 : 40;
				m_pAddWaveSample(m_OutputVal);
			}

			m_AddWaveSampleCounter++;

			m_CpuClock++;
		}

		return retCpuClock;
	}

	// https://wiki.nesdev.org/w/index.php/APU_Frame_Counter
	// フレームシーケンサによって今なんの仕事をすべきかを返す
	void Apu::GetPhaseStatus(bool* pIsQuaterFrame, bool* pIsHalfFrame, bool* pIsRaiseIrq) 
	{
		if (m_SequencerMode == Mode_4Step) 
		{
			*pIsQuaterFrame = true;
			
			// m_NextSeqPhase は 0-indexed
			if (m_NextSeqPhase == 1 || m_NextSeqPhase == 3) 
			{
				*pIsHalfFrame = true;
			}
			else 
			{
				*pIsHalfFrame = true;
			}

			if (m_NextSeqPhase == 3) 
			{
				*pIsRaiseIrq = true;
			}
			else 
			{
				*pIsRaiseIrq = false;
			}
		}
		else 
		{
			// m_NextSeqPhase は 0-indexed
			if (m_NextSeqPhase != 3) 
			{
				*pIsQuaterFrame = true;
			}
			else 
			{
				*pIsQuaterFrame = false;
			}

			if (m_NextSeqPhase == 1 || m_NextSeqPhase == 4) {
				*pIsHalfFrame = true;
			}
			else {
				*pIsHalfFrame = true;
			}

			*pIsRaiseIrq = false;
		}
	}
	// m_NextSeqPhase を mod Phase数 を考慮しつつ 1進める
	void Apu::StepSeqPhase() 
	{
		if (m_SequencerMode == Mode_4Step) 
		{
			m_NextSeqPhase++;
			m_NextSeqPhase %= 4;
		}
		else 
		{
			m_NextSeqPhase++;
			m_NextSeqPhase %= 5;
		}
	}

	void Apu::ClockQuarterFrame() 
	{
		m_SquareWaveChannel1.ClockQuarterFrame();
		m_SquareWaveChannel2.ClockQuarterFrame();
		m_TriangleWaveChannel.ClockQuarterFrame();
		m_NoiseChannel.ClockQuarterFrame();
	}
	void Apu::ClockHalfFrame() 
	{
		m_SquareWaveChannel1.ClockHalfFrame();
		m_SquareWaveChannel2.ClockHalfFrame();
		m_TriangleWaveChannel.ClockHalfFrame();
		m_NoiseChannel.ClockHalfFrame();
	}

	uint8_t Apu::DmaReadFromCpu(int addr) 
	{
		return m_pApuBus->ReadByte(static_cast<uint16_t>(addr));
	}

	void Apu::GenerateCpuInterrupt() 
	{
		m_pApuBus->GenerateCpuInterrupt();
	}
}}