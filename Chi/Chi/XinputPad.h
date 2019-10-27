#ifndef XINPUTPAD_H_
#define XINPUTPAD_H_

#include <winerror.h>
#include <Xinput.h>
#include <DirectXMath.h>


enum XboxPad_Button
{
	DPAD_UP = 0x00000001,
	DPAD_DOWN = 0x00000002,
	DPAD_LEFT = 0x00000004,
	DPAD_RIGHT = 0x00000008,
	START = 0x00000010,
	BACK = 0x00000020,
	LEFT_THUMB = 0x00000040,
	RIGHT_THUMB = 0x00000080,
	LEFT_SHOULDER = 0x0100,
	RIGHT_SHOULDER = 0x0200,
	A = 0x1000,
	B = 0x2000,
	X = 0x4000,
	Y = 0x8000,
};

class XboxPad
{
private:
public:
	XINPUT_STATE pad;
//	XINPUT_VIBRATION vibration;
	const SHORT deadZone = 10000;

	bool getState(int padNum)
	{
		if (XInputGetState(padNum, &pad) == ERROR_SUCCESS)
			return true;
		return false;
	}
	
	bool pressedButton(int _button)
	{
		return pad.Gamepad.wButtons & _button;
	}

	DirectX::XMINT2 getThumbL()
	{
		DirectX::XMINT2 ThumbPos = { pad.Gamepad.sThumbLX ,pad.Gamepad.sThumbLY};
		if (ThumbPos.x < deadZone && ThumbPos.x > -deadZone)
			ThumbPos.x = 0;
		if (ThumbPos.y < deadZone && ThumbPos.y > -deadZone)
			ThumbPos.y = 0;
		return ThumbPos;
	}

	DirectX::XMINT2 getThumbR()
	{
		DirectX::XMINT2 ThumbPos = { pad.Gamepad.sThumbRX ,pad.Gamepad.sThumbRY };
		if (ThumbPos.x < deadZone && ThumbPos.x > -deadZone)
			ThumbPos.x = 0;
		if (ThumbPos.y < deadZone && ThumbPos.y > -deadZone)
			ThumbPos.y = 0;
		return ThumbPos;
	}

	
};


#endif XINPUTPAD_H_