#pragma once

#pragma once

class Time
{
private:
	
    unsigned int m_currentTicks_ms = 0;				// milliseconds
    unsigned int m_lastTicks_ms = 0;				// milliseconds
    unsigned int m_frameRate = 60;				// frames per second (FPS)
    float m_deltaTime_s;
    float m_fixedDeltaTime_s;
    float m_timeScale = 1.0f;
    Time() = default;
    ~Time() = default;

    float ProcessDeltaTime();
public:
    static void Create();
    static void Destroy();
    static Time& Instance();

    void UpdateTicks();

    // value range : 0.0 ~ 1.0
    // 0.5 is slower than half of delta time
    void SetTimeScale(const float& time);
    inline float TimeScale() const { return m_timeScale; }

    unsigned int MillisecondsPerFrame(const unsigned int& frameRate);
    void SetFrameRate(const unsigned int& frameRate);
    unsigned int GetFrameRate() const;
    unsigned int GetCurrentTicks_ms() const;
    unsigned int DeltaTime_ms() const;
    void Delay(int time);	

    float GetCurrentTicks_s() const;
    float FixedDeltaTime_s() const;
    float DeltaTime_s() const;	
    void Delay(float time_s);
    
private:
    // Don't allow copy and move semantics
    Time(const Time&);
    Time(Time&&) noexcept;
    void operator =(const Time&);
    void operator =(Time&&) noexcept;

    static Time* m_instance;
};

