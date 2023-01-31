#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct Animation
{
    int texId;
    int texColumns;

    int celWidth, celHeight;
    int celBaseId;
    int celCount;
    int loop;
};

struct Notifier
{
    const char* name;

    int keyframe;
};

// Singleton
class AnimationMng
{
public:
    using AnimationMap_t = std::unordered_map<std::string, Animation>;
public:
    static void Create();
    static void Destroy();
    static AnimationMng& Instance();

    static bool LoadFromXML(const std::string& file);
    static bool HasAnimationList(const std::string& listKey);
    static bool HasAnimation(const std::string& listKey, const std::string& state);
    static const Animation& GetAnimation(const std::string& listKey, const std::string& state);
    static std::string GetAnimationKey(const std::string& listKey, const std::string& state);
    static int GetDuration_ms(int durationIndex);
    static bool HasNotifiers(const std::string& listKey, const std::string& state);
    static const std::vector<Notifier>& GetNotifiers(const std::string& listKey, const std::string& state);
private:
    AnimationMng();
    ~AnimationMng();
private:
    // Not allow copy and move
    // Body of these functions are empty
    AnimationMng(const AnimationMng&);
    AnimationMng(AnimationMng&&) noexcept;
    void operator = (const AnimationMng&);
    void operator = (AnimationMng&&) noexcept;

private:
    std::unordered_map<std::string, AnimationMap_t> m_listMap;
    std::unordered_map<std::string, std::vector<Notifier>> m_notifiersMap;
    std::vector<int> m_durations_ms;
    static AnimationMng* m_instance;
};

