#include "AnimationTransition.h"

void AnimatorTransition::AddCondition(CONDITION_MODE mode, const std::string& paraName, float threshold)
{
	conditions.push_back(AnimatorCondition{ mode,paraName,threshold });
}
