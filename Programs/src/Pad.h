#pragma once
#include <stdint.h>
#include <Nes_Pad.h>
#include "constants.h"

namespace nes { namespace detail {

	class Pad
	{
	public:
		Pad()
			:m_IsStrobeEnable(true)
			,m_ButtonStatus(0)
			,m_ReadIdx(0)
		{}

		void PushButton(PadButton button);
		void ReleaseButton(PadButton button);
		// 1回読みだすたびに読みだすインデックスをずらす
		uint8_t ReadPad();
		void SetStrobe(bool flag);

	private:
		bool m_IsStrobeEnable;
		uint8_t m_ButtonStatus;
		int m_ReadIdx;
	};
}}