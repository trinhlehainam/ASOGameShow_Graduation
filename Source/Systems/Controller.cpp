#include "Controller.h"

#include <DxLib.h>

#include "../Input/JoypadXInput.h"
#include "../Input/KeyboardInput.h"


Controller::Controller()
{
    m_inputMap.emplace(INPUT_DEVICE::KEYBOARD, std::make_shared<KeyboardInput>());
    m_inputMap.emplace(INPUT_DEVICE::JOYPAD, std::make_shared<JoypadXInput>());
}

Controller::~Controller()
{
}

void Controller::Update()
{
    for (const auto& input : m_inputMap)
        input.second->Update();

    if (DxLib::CheckHitKeyAll() == 0)
        return;

    if (DxLib::CheckHitKeyAll(DX_CHECKINPUT_KEY))
        m_lastedInput = m_inputMap.at(INPUT_DEVICE::KEYBOARD);
    else if (DxLib::CheckHitKeyAll(DX_CHECKINPUT_PAD))
        m_lastedInput = m_inputMap.at(INPUT_DEVICE::JOYPAD);
}

bool Controller::IsPressed(INPUT_ID id) const
{
   if (m_lastedInput.expired())
       return false;

    return m_lastedInput.lock()->IsPressed(id);
}

bool Controller::IsJustPressed(INPUT_ID id) const
{
   if (m_lastedInput.expired())
       return false;

    return m_lastedInput.lock()->IsJustPressed(id);
}

bool Controller::IsReleased(INPUT_ID id) const
{
   if (m_lastedInput.expired())
       return false;

    return m_lastedInput.lock()->IsReleased(id);
}

bool Controller::IsJustReleased(INPUT_ID id) const
{
   if (m_lastedInput.expired())
       return false;

    return m_lastedInput.lock()->IsJustReleased(id);
}

bool Controller::IsAnyKeyPressed() const
{
   if (m_lastedInput.expired())
       return false;

    return m_lastedInput.lock()->IsAnyKeyPressed();
}

std::weak_ptr<IInput> Controller::GetLastedUsedInputDevice()
{
    return m_lastedInput;
}

std::weak_ptr<IInput> Controller::GetInputDevice(INPUT_DEVICE InputDevice)
{
    return m_inputMap.at(InputDevice);
}