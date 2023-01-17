#pragma once
#include "IInput.h"

#include <DxLib.h>

class JoypadXInput: public IInput
{
public:
	JoypadXInput();
	~JoypadXInput();
public:
	void Update() override;
	INPUT_DEVICE GetDeviceID() override;
private:
	XINPUT_STATE m_joypadInputs;
};

