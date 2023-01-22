#pragma once
#include "vec2.h"

struct HitInfo
{
    vec2f Normal;
    float CollisionTime;
    bool IsHit;
};
