#pragma once
#include <memory>
#include <unordered_map>

#include "../Input/IInput.h"

class Controller
{
public:
    Controller();
    ~Controller();

    void Update();
    
	bool IsPressed(INPUT_ID id) const;
	bool IsJustPressed(INPUT_ID id) const;
	bool IsReleased(INPUT_ID id) const;
	bool IsJustReleased(INPUT_ID id) const;
	bool IsAnyKeyPressed() const;
	
    std::weak_ptr<IInput> GetLastedUsedInputDevice();
    std::weak_ptr<IInput> GetInputDevice(INPUT_DEVICE InputDevice);
private:
    std::unordered_map<INPUT_DEVICE, std::shared_ptr<IInput>> m_inputMap;
    std::weak_ptr<IInput> m_lastedInput;
};
