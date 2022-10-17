#pragma once

enum class ANIMATOR_PARAMETER_TYPE
{
	FLOAT = 0,
	BOOL = 1,
	INTEGER = 2
};

struct AnimatorParameter
{
	ANIMATOR_PARAMETER_TYPE type;
	float value;
};