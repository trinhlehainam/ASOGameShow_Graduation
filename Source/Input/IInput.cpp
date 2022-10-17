#include "IInput.h"

#include <DxLib.h>


IInput::IInput():
	m_inputs{}, m_currentState(0)
{
}

IInput::IInput(ArrayInputs_t inputs):m_inputs(std::move(inputs)), m_currentState(0)
{
}

IInput::~IInput()
{
}

void IInput::SetInput(INPUT_ID id, int DX_KEY_INPUT)
{
	m_inputs[static_cast<size_t>(id)] = DX_KEY_INPUT;
}

void IInput::SetInputState(INPUT_ID id, bool state)
{
	m_inputStates[m_currentState].set(static_cast<size_t>(id), state);
}

std::vector<INPUT_ID> IInput::GetPressedKeys() const
{
	std::vector<INPUT_ID> ret;
	for (auto id : INPUT_ID())
		if (IsPressed(id))
			ret.push_back(id);
	return ret;
}

std::vector<INPUT_ID> IInput::GetJustPressedKeys() const
{
	std::vector<INPUT_ID> ret;
	for (auto id : INPUT_ID())
		if (IsJustPressed(id))
			ret.push_back(id);
	return ret;
}

bool IInput::IsPressed(INPUT_ID id) const
{
	return m_inputStates[m_currentState][static_cast<size_t>(id)] == 1;
}

bool IInput::IsJustPressed(INPUT_ID id) const
{
	return m_inputStates[m_currentState][static_cast<size_t>(id)] == 1 &&
		m_inputStates[PreviousState()][static_cast<size_t>(id)] == 0;
}

bool IInput::IsReleased(INPUT_ID id) const
{
	return m_inputStates[m_currentState][static_cast<size_t>(id)] == 0;
}

bool IInput::IsJustReleased(INPUT_ID id) const
{
	return m_inputStates[m_currentState][static_cast<size_t>(id)] == 0 &&
		m_inputStates[PreviousState()][static_cast<size_t>(id)] == 1;
}

bool IInput::IsAnyKeyPressed() const
{
	return m_inputStates[m_currentState].any();
}

int IInput::PreviousState() const
{
	return ((m_currentState - 1 ) + kMaxInputStates) % kMaxInputStates;
}
