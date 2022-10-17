#include "InputCommand.h"

#include <unordered_map>
#include <chrono>
#include <sstream>

#include <rapidxml.hpp>

#include "../Utilities/StringHelper.h"
#include "../Utilities/static_ring.h"
#include "../Input/IInput.h"
#include "../_debug/_DebugConOut.h"

namespace
{
	struct InputInfo
	{
		INPUT_ID id;
		std::chrono::steady_clock::time_point time;
	};
}

#pragma region Pimpl
class InputCommand::Impl
{
public:
	using PatternMap_t = std::unordered_map<std::string, std::vector<INPUT_ID>>;
public:
	explicit Impl(const std::shared_ptr<IInput>& controller);
	~Impl();
private:
	friend InputCommand;

	std::weak_ptr<IInput> m_controller;
	static_ring<InputInfo, 10> m_inputs;
	PatternMap_t m_patternMap;
};

InputCommand::Impl::Impl(const std::shared_ptr<IInput>& controller):m_controller(controller)
{
	m_inputs.set_tail_to_head_loop();
}
InputCommand::Impl::~Impl() {}
#pragma endregion


InputCommand::InputCommand(const std::shared_ptr<IInput>& controller) : m_impl(std::make_unique<Impl>(controller))
{
}

InputCommand::~InputCommand()
{
}

void InputCommand::AddPattern(const std::string& key, std::vector<INPUT_ID> inputIDs)
{
	assert(!m_impl->m_patternMap.count(key));
	m_impl->m_patternMap[key] = std::move(inputIDs);
}

void InputCommand::LoadPatternFromXML(const std::string& file)
{
	rapidxml::xml_document<> doc;
	auto content = StringHelper::LoadFileToStringBuffer(file);
	doc.parse<0>(&content[0]);

	auto pGroup = doc.first_node();

	for (auto pPattern = pGroup->first_node(); pPattern; pPattern = pPattern->next_sibling())
	{
		std::string name{ std::move(pPattern->first_attribute("name")->value()) };
		std::vector<INPUT_ID> keys;
		auto pKey = pPattern->first_node("key");
		std::stringstream keys_ss{ std::move(pKey->value()) };
		std::string key;
		while (keys_ss >> key)
		{
			if (key == "up")
				keys.push_back(INPUT_ID::UP);
			else if (key == "down")
				keys.push_back(INPUT_ID::DOWN);
			else if (key == "left")
				keys.push_back(INPUT_ID::LEFT);
			else if (key == "right")
				keys.push_back(INPUT_ID::RIGHT);
			else if (key == "btn1")
				keys.push_back(INPUT_ID::BTN1);
			else if (key == "btn2")
				keys.push_back(INPUT_ID::BTN2);
			else if (key == "btn3")
				keys.push_back(INPUT_ID::BTN3);
			else if (key == "btn4")
				keys.push_back(INPUT_ID::BTN4);

		}
		m_impl->m_patternMap.emplace(std::move(name), std::move(keys));
	}

	doc.clear();
}

bool InputCommand::IsMatch(const std::string& key)
{
	const auto& pattern = m_impl->m_patternMap.at(key);
	if (pattern.empty()) return false;

	const auto kMatchNum = pattern.size();
	size_t matchIndex = 0;
	for (const auto& input : m_impl->m_inputs)
	{
		matchIndex = input.id == pattern[matchIndex] ? matchIndex + 1 : 0;
		if (matchIndex >= kMatchNum)
			return true;
	}

	return false;
}

bool InputCommand::IsMatch(const std::string& key, float inLastTime_s)
{
	const auto& pattern = m_impl->m_patternMap.at(key);
	if (pattern.empty()) return false;

	const auto kMatchNum = pattern.size();
	auto currentTime = std::chrono::high_resolution_clock::now();
	size_t matchIndex = 0;
	for (const auto& input : m_impl->m_inputs)
	{
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - input.time).count();
		if (deltaTime > inLastTime_s)
			continue;

		matchIndex = input.id == pattern[matchIndex] ? matchIndex + 1 : 0;
		if (matchIndex >= kMatchNum)
			return true;
	}
	
	return false;
}

void InputCommand::Update()
{
	if (m_impl->m_controller.expired()) return;

	auto pressedKeys = m_impl->m_controller.lock()->GetJustPressedKeys();
	if (pressedKeys.empty()) return;

	for (auto key : pressedKeys)
		m_impl->m_inputs.insert(InputInfo{ key, std::chrono::high_resolution_clock::now() });

	TRACE("Inputs Stack : ");
	for (const auto& input : m_impl->m_inputs)
	{
		TRACE("%d  ", static_cast<int>(input.id));
	}
	TRACE("\n");

	for (const auto& pattern : m_impl->m_patternMap)
	{
		TRACE("Pattern %s :", pattern.first.c_str());
		for (const auto& key : pattern.second)
			TRACE("%d  ", static_cast<int>(key));
	}
	TRACE("\n");
}
