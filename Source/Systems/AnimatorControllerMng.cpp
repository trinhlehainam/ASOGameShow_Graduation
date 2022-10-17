#include "AnimatorControllerMng.h"

#include <cassert>

#include <rapidxml.hpp>

#include "../Utilities/MacroHelper.h"
#include "../Utilities/StringHelper.h"

GenerateDynamicSingleton(AnimatorControllerMng);

void AnimatorControllerMng::LoadFromXML(const std::string& path)
{
	rapidxml::xml_document<> doc;
	auto content = StringHelper::LoadFileToStringBuffer(path);
	doc.parse<0>(&content[0]);

	auto pAnimController = doc.first_node();

	// Load animator attribute
	std::string animatorName;
	std::string entryState;
	for (auto pAttr = pAnimController->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
	{
		if (strcmp(pAttr->name(), "name") == 0)
			animatorName = std::move(pAttr->value());
		else if (strcmp(pAttr->name(), "entryState") == 0)
			entryState = std::move(pAttr->value());
	}
	auto& animatorController = m_instance->m_animatorMap[animatorName];
	animatorController.entryState = std::move(entryState);
	//

	// Load Parameters
	for (auto pParameter = pAnimController->first_node("parameter"); pParameter; pParameter = pParameter->next_sibling())
	{
		if (strcmp(pParameter->name(), "parameter") != 0)
			break;

		std::string paraName;
		ANIMATOR_PARAMETER_TYPE type{};
		float value = 0.0f;
		for (auto pAttr = pParameter->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
		{
			if (strcmp(pAttr->name(), "name") == 0)
				paraName = std::move(pAttr->value());
			else if (strcmp(pAttr->name(), "type") == 0)
				type = static_cast<ANIMATOR_PARAMETER_TYPE>(std::atoi(pAttr->value()));
			else if (strcmp(pAttr->name(), "value") == 0)
				value = std::strtof(std::move(pAttr->value()), 0);
		}
		animatorController.paramMap.emplace(paraName, AnimatorParameter{ type,value });
	}
	//

	for (auto pState = pAnimController->first_node("animatorState"); pState; pState = pState->next_sibling())
	{
		// Load AnimationState
		std::string stateName;
		std::string animList;
		std::string animState;
		for (auto pAttr = pState->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
		{
			if (strcmp(pAttr->name(), "name") == 0)
				stateName = std::move(pAttr->value());
			else if (strcmp(pAttr->name(), "animationList") == 0)
				animList = std::move(pAttr->value());
			else if (strcmp(pAttr->name(), "animationState") == 0)
				animState = std::move(pAttr->value());
		}
		AnimatorState state{};
		state.animationList = std::move(animList);
		state.animationState = std::move(animState);
		state.tag = stateName;
		//

		// Load Transitions
		for (auto pTransition = pState->first_node(); pTransition; pTransition = pTransition->next_sibling())
		{
			std::string transName;
			std::string currentState;
			std::string destState;
			for (auto pAttr = pTransition->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
			{
				if (strcmp(pAttr->name(), "name") == 0)
					transName = std::move(pAttr->value());
				else if (strcmp(pAttr->name(), "currentState") == 0)
					currentState = std::move(pAttr->value());
				else if (strcmp(pAttr->name(), "destinationState") == 0)
					destState = std::move(pAttr->value());
			}

			AnimatorTransition transition{};
			transition.tag = transName == "" ? currentState + "->" + destState : transName;
			transition.currentState = std::move(currentState);
			transition.destinationState = std::move(destState);

			// Load Conditions
			for (auto pCondition = pTransition->first_node(); pCondition; pCondition = pCondition->next_sibling())
			{
				CONDITION_MODE mode{};
				std::string paraName;
				float threshold = 0.0f;
				for (auto pAttr = pCondition->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
				{
					if (strcmp(pAttr->name(), "mode") == 0)
						mode = static_cast<CONDITION_MODE>(std::atoi(pAttr->value()));
					else if (strcmp(pAttr->name(), "paraName") == 0)
						paraName = std::move(pAttr->value());
					else if (strcmp(pAttr->name(), "threshold") == 0)
						threshold = std::strtof(std::move(pAttr->value()), 0);
				}
				transition.AddCondition(mode, paraName, threshold);
			}
			state.transitions.push_back(transition);
		}
		animatorController.stateMap.emplace(stateName, state);
	}

	doc.clear();
}

AnimatorController& AnimatorControllerMng::Get(const std::string& key)
{
	return m_instance->m_animatorMap.at(key);
}

bool AnimatorControllerMng::Has(const std::string& key)
{
	return m_instance->m_animatorMap.count(key);
}

AnimatorControllerMng::AnimatorControllerMng() {}
AnimatorControllerMng::~AnimatorControllerMng() {}
