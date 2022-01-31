#pragma once
#include <utility>
#include <stdint.h>
#include "constants.h"
#include "System.h"

namespace nes { namespace detail {
	class CpuBus;
	class ApuBus;
	enum SequencerMode
	{
		Mode_4Step,
		Mode_5Step
	};

	// 矩形波 チャンネル、コメントのアドレスは矩形波チャンネル1のものであるとする
	class SquareWaveChannel {
	public:
		SquareWaveChannel(uint16_t baseAddr, bool isChannel1)
			: m_DutyTable(m_DutyTables[0]) // TORIAEZU: 設定する前に音ならす ROM があってぬるぽ触ってしまうようなら考える
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
			, m_FreqCounter(0)
			, m_DutyCounter(0)
			, m_DecayResetFlag(false)
			, m_BaseAddr(baseAddr)
			, m_DecayHiddenVol(0xf)
			, m_DecayCounter(0)
			, m_SweepCounter(0)
			, m_IsChannel1(isChannel1)
		{}

		void WriteRegister(uint8_t value, uint16_t addr);
		// APU 全体レジスタ($4015, $4017 の書き込みで反映される値)
		void On4015Write(uint8_t value);
		uint8_t GetStatusBit();
		
		// 各 クロック(Apu クラスから呼び出すことを想定)
		void ClockTimer();
		void ClockQuarterFrame();
		void ClockHalfFrame();
		
		// 出力
		int GetOutPut();

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

		// レジスタの値ではないけどエミュレーションにひつよう
		// $4003 書き込みで初期化されるひとたち
		int m_FreqCounter;
		int m_DutyCounter;
		bool m_DecayResetFlag;

		// 矩形波チャンネルレジスタのベースアドレス
		uint16_t m_BaseAddr;

		// Decay 用変数
		// 音量
		int m_DecayHiddenVol;
		// ボリューム/エンベロープ周期
		int m_DecayCounter;

		// sweep 用変数
		int m_SweepCounter;

		// 矩形波チャンネル 1 か？(sweep の挙動が分岐する)
		bool m_IsChannel1;

	private:
		// 内部実装用メソッドたち
		bool IsSweepForcingSilence();

	private:
		// テーブル
		int m_DutyTables[4][8] = {
			{0, 1, 0, 0, 0, 0, 0, 0},
			{0, 1, 1, 0, 0, 0, 0, 0},
			{0, 1, 1, 1, 1, 0, 0, 0},
			{1, 0, 0, 1, 1, 1, 1, 1},
		};
	};

	// 各チャンネルを保持して音をならす APU クラス
	// TODO: 各チャンネルを同一I/Fで扱えるようにする
	class Apu {
	public:
		Apu(detail::ApuBus* pApuBus)
			: m_NextSeqPhase(0)
			, m_SequencerCounter(0)
			, m_SequencerMode(Mode_4Step)
			, m_IrqEnabled(false)
			, m_IrqPending(false)
			, m_CpuClock(0)
			, m_SquareWaveChannel1(0x4000, true)
			, m_SquareWaveChannel2(0x4004, false)
			, m_OutputVal(0)
			, m_pApuBus(pApuBus)
		{}

		// レジスタ 書き込み
		void WriteRegister(uint8_t value, uint16_t addr);
		
		// CPU クロックを受け取ってその分だけ APU を動かす。 APU クロックでなく CPU クロック分であることに注意する。
		void Run(uint64_t cpuClock);

		// レジスタ 読み出し
		uint8_t ReadRegister4015();

		// 出力値
		int GetOutPut();
	private:
		// $4017 書き込みで初期化されるひとたち
		int m_NextSeqPhase;
		int m_SequencerCounter;

		// $4017
		SequencerMode m_SequencerMode;
		bool m_IrqEnabled;
		bool m_IrqPending;

		// クロックたち、 CPU クロックでエミュレートする。 1 APU clock = 2 CPU clock
		uint64_t m_CpuClock;

		// チャンネルたち
		SquareWaveChannel m_SquareWaveChannel1;
		SquareWaveChannel m_SquareWaveChannel2;

		// 出力値
		int m_OutputVal;

		// バス(割り込み用)
		detail::ApuBus* m_pApuBus;

		// 各チャンネルたちを駆動するメソッド
		void ClockQuarterFrame();
		void ClockHalfFrame();

		// 内部実装
		// フレームシーケンサによって今なんの仕事をすべきかを返す
		void GetPhaseStatus(bool* pIsQuaterFrame, bool* pIsHalfFrame, bool* pIsRaiseIrq);
		// m_NextSeqPhase を mod Phase数 を考慮しつつ 1進める
		void StepSeqPhase();

	};

}}