#pragma once
#include <memory>
#include <string>
#include <vector>

#include "InputID.h"

class IInput;

class InputCommand
{
public:
	explicit InputCommand(const std::shared_ptr<IInput>& controller);
	~InputCommand();

	void AddPattern(const std::string& key, std::vector<INPUT_ID> inputIDs);
	void LoadPatternFromXML(const std::string& file);

	bool IsMatch(const std::string& patternKey);
	bool IsMatch(const std::string& patternKey, float inLastTime_s);

public:
	void Update();
private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

