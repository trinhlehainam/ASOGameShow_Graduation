#pragma once
#include <string>
#include <unordered_map>

#include "AnimatorState.h"
#include "AnimatorParameter.h"

struct AnimatorController
{
	std::unordered_map<std::string, AnimatorParameter> paramMap;
	std::unordered_map<std::string, AnimatorState> stateMap;
	std::string entryState;

	bool HasParameter(const std::string& paramKey) const;
};

