#include <cassert>
#include <cstdlib>
#include <string.h>

#include "Pad.h"

namespace {
	const uint8_t BUTTON_A		= 1;
	const uint8_t BUTTON_B		= 2;
	const uint8_t BUTTON_SELECT = 4;
	const uint8_t BUTTON_START	= 8;
	const uint8_t BUTTON_UP		= 16;
	const uint8_t BUTTON_DOWN	= 32;
	const uint8_t BUTTON_LEFT	= 64;
	const uint8_t BUTTON_RIGHT	= 128;

}
namespace nes { namespace detail {
	void Pad::PushButton(PadButton button)
	{
		switch (button)
		{
		case nes::PadButton::A:
			m_ButtonStatus |= BUTTON_A;
			break;
		case nes::PadButton::B:
			m_ButtonStatus |= BUTTON_B;
			break;
		case nes::PadButton::SELECT:
			m_ButtonStatus |= BUTTON_SELECT;
			break;
		case nes::PadButton::START:
			m_ButtonStatus |= BUTTON_START;
			break;
		case nes::PadButton::UP:
			m_ButtonStatus |= BUTTON_UP;
			break;
		case nes::PadButton::DOWN:
			m_ButtonStatus |= BUTTON_DOWN;
			break;
		case nes::PadButton::LEFT:
			m_ButtonStatus |= BUTTON_LEFT;
			break;
		case nes::PadButton::RIGHT:
			m_ButtonStatus |= BUTTON_RIGHT;
			break;
		default:
			break;
		}
	}

	void Pad::ReleaseButton(PadButton button)
	{
		switch (button)
		{
		case nes::PadButton::A:
			m_ButtonStatus &= ~BUTTON_A;
			break;
		case nes::PadButton::B:
			m_ButtonStatus &= ~BUTTON_B;
			break;
		case nes::PadButton::SELECT:
			m_ButtonStatus &= ~BUTTON_SELECT;
			break;
		case nes::PadButton::START:
			m_ButtonStatus &= ~BUTTON_START;
			break;
		case nes::PadButton::UP:
			m_ButtonStatus &= ~BUTTON_UP;
			break;
		case nes::PadButton::DOWN:
			m_ButtonStatus &= ~BUTTON_DOWN;
			break;
		case nes::PadButton::LEFT:
			m_ButtonStatus &= ~BUTTON_LEFT;
			break;
		case nes::PadButton::RIGHT:
			m_ButtonStatus &= ~BUTTON_RIGHT;
			break;
		default:
			break;
		}
	}

	uint8_t Pad::ReadPad()
	{
		uint8_t ret = (m_ButtonStatus >> m_ReadIdx) & 1;
		// strobe disabled ‚È‚Æ‚«‚¾‚¯ŸˆÈ~‚Ì“ü—Í‚ª‚æ‚ß‚é
		if (!m_IsStrobeEnable)
		{
			m_ReadIdx++;
			m_ReadIdx %= 8;
		}

		return ret;
	}

	void Pad::SetStrobe(bool flag)
	{
		m_IsStrobeEnable = flag;
		if (flag)
		{
			m_ReadIdx = 0;
		}
	}

}}