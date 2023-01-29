#include "Time.h"

#include <DxLib.h>

#include "../Utilities/MacroHelper.h"
#include "../Math/MathHelper.h"

namespace
{
    constexpr unsigned int second_to_millisecond = 1000;
}

GenerateDynamicSingleton(Time);

float Time::ProcessDeltaTime()
{
    return DxLib::GetNowCount() - m_lastTicks_ms;
}

void Time::SetTimeScale(const float& time)
{
    m_timeScale = time;
}

unsigned int Time::MillisecondsPerFrame(const unsigned int& frameRate)
{
    return second_to_millisecond / frameRate;
}

void Time::UpdateTicks()
{
    int timeToWait = MillisecondsPerFrame(m_frameRate) - ProcessDeltaTime();
    if (timeToWait > 0 && timeToWait <= MillisecondsPerFrame(m_frameRate)) Delay(timeToWait);
    m_timeScale = MathHelper::clamp(m_timeScale, 0.0f, 1.0f);
    m_fixedDeltaTime_s = ProcessDeltaTime() / static_cast<float>(second_to_millisecond);
    m_deltaTime_s = m_fixedDeltaTime_s * m_timeScale;
    m_lastTicks_ms = DxLib::GetNowCount();
}

void Time::SetFrameRate(const unsigned int& frameRate)
{
    m_frameRate = frameRate;
    m_fixedDeltaTime_s = MillisecondsPerFrame(m_frameRate) / static_cast<float>(second_to_millisecond);
}

unsigned int Time::GetFrameRate() const
{
    return m_frameRate;
}

float Time::DeltaTime_s() const
{
    return m_deltaTime_s;
}

unsigned int Time::GetCurrentTicks_ms() const
{
    return DxLib::GetNowCount();
}

unsigned int Time::DeltaTime_ms() const
{
    return DeltaTime_s() * second_to_millisecond;
}

float Time::GetCurrentTicks_s() const
{
    return DxLib::GetNowCount() / static_cast<float>(second_to_millisecond);
}

float Time::FixedDeltaTime_s() const
{
    return m_fixedDeltaTime_s;
}

void Time::Delay(int time)
{
    DxLib::WaitTimer(time);
}

void Time::Delay(float time_s)
{
    DxLib::WaitTimer(time_s * second_to_millisecond);
}
