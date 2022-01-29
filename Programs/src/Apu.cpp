#pragma once
#include <cassert>
#include <utility>
#include "Apu.h"

namespace {
	int g_DutyTables[4][8] = {
		{0, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 0, 0},
		{1, 0, 0, 1, 1, 1, 1, 1},
	};

	int g_LengthTable[32] = { 0 };

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
		}
	};
	Initializer g_Initializer;

	constexpr int ClocksToNextSequence = 7457;
}

namespace nes { namespace detail {
	void SquareWaveChannel::WriteRegister(uint8_t value, uint16_t addr)
	{
		uint16_t offset = addr - m_BaseAddr;

		switch (offset) {
		case 0 :
			m_DutyTable = g_DutyTables[value >> 6];
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
		bool channelEnabled = value & 1;

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
		else if (addr == 0x4015) 
		{
			// 全チャンネルに書き込みを反映
			m_SquareWaveChannel1.On4015Write(value);
			m_SquareWaveChannel2.On4015Write(value);
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

		// TODO: 他チャンネルの status bit 取得 and res 更新

		return res;
	}

	int Apu::GetOutPut() 
	{
		return m_OutputVal;
	}

	void Apu::Run(uint64_t cpuClock) 
	{
		// cpuClock ぶんだけ APU うごかす
		for (int i = 0; i < cpuClock; i++) 
		{
			if (m_CpuClock % 2 == 0) 
			{
				// 1 APU サイクルごとに実行したい処理
				m_SquareWaveChannel1.ClockTimer();
				m_SquareWaveChannel2.ClockTimer();
			}

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
					ClockHalfFrame;
				}
				if (isRaiseIrq) {
					// TODO: CpuBus を使って IRQ 割り込みを上げる
					m_IrqPending = true;
				}

				StepSeqPhase();
				m_SequencerCounter = ClocksToNextSequence;
			}

			// 出力値の決定 (1 APU クロックごと)
			if (m_CpuClock % 2 == 0) {
				m_OutputVal = 0;
				m_OutputVal += m_SquareWaveChannel1.GetOutPut();
				m_OutputVal += m_SquareWaveChannel2.GetOutPut();
				// TODO: 他チャンネルをミックス
			}

			// TODO: 特定条件でコールバック関数で音を出力

			m_CpuClock++;
		}
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
	}
	void Apu::ClockHalfFrame() 
	{
		m_SquareWaveChannel1.ClockHalfFrame();
		m_SquareWaveChannel2.ClockHalfFrame();
	}
}}