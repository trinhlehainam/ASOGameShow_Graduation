#pragma once
#include <string>

#include "ConditionMode.h"

struct AnimatorCondition
{
	CONDITION_MODE compareMode;
	std::string paramName;
	float threshold;
};