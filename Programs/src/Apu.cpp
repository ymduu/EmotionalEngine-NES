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

			// TODO: length Counter 更新
			// m_LengthCounter = lengthtable[]

			m_FreqCounter = m_FreqTimer;
			m_DutyCounter = 0;
			m_DecayResetFlag = true;
			break;
		}
		default:
			break;
		}
	}

	void SquareWaveChannel::On4015Write(bool channelEnabled)
	{
		m_ChannelEnabled = channelEnabled;
		if (!m_ChannelEnabled) {
			m_LengthCounter = 0;
		}
	}

	// 割り込みに関する処理は APU 全体のほうでやることに注意
	void SquareWaveChannel::On4017Write(SequencerMode mode)
	{
		m_SequencerMode = mode;
		m_NextSeqPhase = 0;
		// フレームシーケンサのクロックは CPU クロックで計算する。(half clock を考慮するのをサボるため)
		// Every APU Cycle の実装では毎回2引くことに注意する。
		m_SequencerCounter = ClocksToNextSequence;

		if(m_SequencerMode == Mode_5Step)
		{
			// TODO: QuaterFrame と HalfFrame 相当の処理よびだし 
		}
	}


}}