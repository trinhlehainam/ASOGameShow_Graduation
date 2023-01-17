#pragma once
#include <bitset>
#include <array>
#include <vector>

#include "InputID.h"

namespace
{
	constexpr int kMaxInputStates = 2;
}

enum class INPUT_DEVICE
{
	KEYBOARD,
	JOYPAD
};

class IInput
{
public:
	//								<Dxlib Input ID, Max_Input_Num>
	using ArrayInputs_t = std::array<int, static_cast<size_t>(INPUT_ID::MAX)>;
	using InputState_t = std::bitset<static_cast<size_t>(INPUT_ID::MAX)>;
public:
	IInput();
	explicit IInput(ArrayInputs_t);
	virtual ~IInput();
public:
	virtual void Update() = 0;
	virtual INPUT_DEVICE GetDeviceID() = 0;
public:
	void SetInput(INPUT_ID id, int DX_KEY_INPUT);

	std::vector<INPUT_ID> GetPressedKeys() const;
	std::vector<INPUT_ID> GetJustPressedKeys() const;

	bool IsPressed(INPUT_ID id) const;
	bool IsJustPressed(INPUT_ID id) const;
	bool IsReleased(INPUT_ID id) const;
	bool IsJustReleased(INPUT_ID id) const;
	bool IsAnyKeyPressed() const;

protected:
	void SetInputState(INPUT_ID id, bool state);
protected:
	ArrayInputs_t m_inputs;
	std::array<InputState_t, kMaxInputStates> m_inputStates;
	int m_currentState;

private:
	int PreviousState() const;
};

