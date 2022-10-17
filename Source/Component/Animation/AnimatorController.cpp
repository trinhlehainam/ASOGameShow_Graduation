#include "AnimatorController.h"

bool AnimatorController::HasParameter(const std::string& paramKey) const
{
	return paramMap.count(paramKey);
}
