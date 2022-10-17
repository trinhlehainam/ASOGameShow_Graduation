#pragma once
#include "AnimationTransition.h"

struct AnimatorState
{
	std::string animationList;
	std::string animationState;
	std::vector<AnimatorTransition> transitions;
	std::string tag;
};
