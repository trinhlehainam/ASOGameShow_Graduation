#pragma once
#include "IInput.h"

class KeyboardInput : public IInput
{
public:
	KeyboardInput();
	~KeyboardInput();
public:
	void Update() override;
	INPUT_DEVICE_ID GetDeviceID() override;
};

