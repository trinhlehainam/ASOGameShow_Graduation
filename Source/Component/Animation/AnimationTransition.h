#pragma once
#include <vector>

#include "AnimatorCondition.h"

struct AnimatorTransition
{
	std::vector<AnimatorCondition> conditions;
	std::string currentState;
	std::string destinationState;
	std::string tag;
	bool isMute;

	void AddCondition(CONDITION_MODE mode, const std::string& paraName, float threshold);
};

