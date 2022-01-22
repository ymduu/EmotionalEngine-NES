#pragma once
#include <utility>
#include <stdint.h>
#include "constants.h"
#include "System.h"

namespace nes { namespace detail {
	class CpuBus;
	enum SequencerMode
	{
		Mode_4Step,
		Mode_5Step
	};

	// 矩形波 チャンネル、コメントのアドレスは矩形波チャンネル1のものであるとする
	class SquareWaveChannel {
	public:
		SquareWaveChannel(uint16_t baseAddr)
			: m_DutyTable(nullptr) // TORIAEZU: 設定する前に音ならす ROM があってぬるぽ触ってしまうようなら考える
			, m_DecayLoop(false)
			, m_LengthEnabled(false)
			, m_DecayEnabled(false)
			, m_DecayV(0)
			, m_SweepTimer(0)
			, m_SweepNegate(false)
			, m_SweepShift(0)
			, m_SweepReload(0)
			, m_SweepEnabled(0)
			, m_FreqTimer(0)
			, m_LengthCounter(0)
			, m_ChannelEnabled(false)
			, m_SequencerMode(Mode_4Step)
			, m_IrqEnabled(false)
			, m_FreqCounter(0)
			, m_DutyCounter(0)
			, m_DecayResetFlag(false)
			, m_NextSeqPhase(0)
			, m_SequencerCounter(0)
			, m_CpuClock(0)
			, m_ApuClock(0)
			, m_BaseAddr(baseAddr)
		{}

		void WriteRegister(uint8_t value, uint16_t addr);
		// APU 全体レジスタ($4015, $4017 の書き込みで反映される値)
		void On4015Write(bool channelEnabled);
		void On4017Write(SequencerMode mode);

	private:
		// エミュレーション用変数たち、読み取り専用の値はレジスタからフェッチするだけでいいはずだが、どれが読み取り専用かわからんので全部変数にしちゃう、レジスタ書き込み時に設定することにする
		
		// $4000
		// Duty Table 配列先頭を指すポインタ
		int* m_DutyTable;
		bool m_DecayLoop;
		bool m_LengthEnabled;
		bool m_DecayEnabled;
		int m_DecayV;
	
		// $4001
		int m_SweepTimer;
		bool m_SweepNegate;
		int m_SweepShift;
		bool m_SweepReload;
		bool m_SweepEnabled;

		// $4002, $4003
		int m_FreqTimer;
		int m_LengthCounter;

		// $4015、チャンネルによってことなる bit を見ることに注意
		bool m_ChannelEnabled;

		// $4017
		SequencerMode m_SequencerMode;
		bool m_IrqEnabled;

		// レジスタの値ではないけどエミュレーションにひつよう
		// $4003 書き込みで初期化されるひとたち
		int m_FreqCounter;
		int m_DutyCounter;
		bool m_DecayResetFlag;

		// $4017 書き込みで初期化されるひとたち
		int m_NextSeqPhase;
		int m_SequencerCounter;

		// クロックたち、 CPU クロックと APU クロックの両方をもつ(CPU クロックで動く人もいるので)
		uint64_t m_CpuClock;
		uint64_t m_ApuClock;

		// 矩形波チャンネルレジスタのベースアドレス
		uint16_t m_BaseAddr;
	};

}}