#include "Animator.h"

#include <cassert>
#include <bitset>

#include <DxLib.h>

#include "../TransformComponent.h"

#include "../../Systems/AnimatorControllerMng.h"
#include "../../Systems/AnimationMng.h"
#include "../../Math/MathHelper.h"
#include "../../GameObject/Entity.h"


#pragma region Pimpl
class Animator::Impl
{
public:
    Impl(const std::shared_ptr<Entity>& owner);
    ~Impl();

private:
    void Play(const std::string& state);

    bool CheckCondition(const AnimatorCondition& condition);
    void BindNotify(const std::string& stateKey, const std::string& notifyKey, IAnimNotify* Notifier);
private:
    void Update(float deltaTime_s);
    void Notify();
    void Render();

    void UpdateInfinite(float deltaTime_s);
    void UpdateLoop(float deltaTime_s);
    void UpdateSleep(float deltaTime_s);

    using UpdateFunc_t = void (Impl::*)(float);
    UpdateFunc_t updateFunc;

    void ActiveCheckTransition();
    void Sleep();

    using CheckTransitionFunc_t = void (Impl::*)();
    CheckTransitionFunc_t checkTransFunc;
private:
    friend Animator;

    std::weak_ptr<TransformComponent> transform;
    std::weak_ptr<Animator> animator;
    std::unordered_map<std::string, IAnimNotify*> notifiersMap;

    std::string animatorKey;
    std::string currentState;
    std::string currentAnimKey;

    int currentDurationID;
    int currentKeyFrame;
    int notifyKeyFrame;
    int timer_ms;
    int loopCount;
    float playRate;
    bool isFlipped;
};

Animator::Impl::Impl(const std::shared_ptr<Entity>& owner) :
    updateFunc(&Impl::UpdateSleep),
    checkTransFunc(&Impl::Sleep),
    transform(owner->GetComponent<TransformComponent>()),
    animator(owner->GetComponent<Animator>()),
    currentDurationID(0), currentKeyFrame(0), notifyKeyFrame(0), timer_ms(0), loopCount(0), playRate(1.f),
    isFlipped(false)
{
}

Animator::Impl::~Impl()
{
}

void Animator::Impl::Play(const std::string& state)
{
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animator = AnimatorControllerMng::Get(animatorKey);
    if (state == currentState) return;
    currentState = state;
    const auto& animatorState = animator.stateMap.at(currentState);
    auto& AnimMng = AnimationMng::Instance();

    // Check function's arguments before assign to member variables
    assert(AnimMng.HasAnimation(animatorState.animationList, animatorState.animationState));
    //

    auto& animation = AnimMng.GetAnimation(animatorState.animationList, animatorState.animationState);
    currentAnimKey = AnimMng.GetAnimationKey(animatorState.animationList, animatorState.animationState);
    currentDurationID = animation.celBaseId;
    currentKeyFrame = 0;
    timer_ms = AnimMng.GetDuration_ms(currentDurationID) / playRate;

    switch (animation.loop)
    {
    case -1:
        updateFunc = &Impl::UpdateInfinite;
        break;
    default:
        updateFunc = &Impl::UpdateLoop;
        loopCount = animation.loop;
        break;
    }
}

bool Animator::Impl::CheckCondition(const AnimatorCondition& condition)
{
    if (!AnimatorControllerMng::Has(animatorKey)) return false;
    const auto& animator = AnimatorControllerMng::Get(animatorKey);
    switch (condition.compareMode)
    {
    case CONDITION_MODE::IS_TRUE:
        return static_cast<bool>(animator.paramMap.at(condition.paramName).value) == true;
    case CONDITION_MODE::IS_FALSE:
        return static_cast<bool>(animator.paramMap.at(condition.paramName).value) == false;
    case CONDITION_MODE::GREATER:
        return animator.paramMap.at(condition.paramName).value > condition.threshold;
    case CONDITION_MODE::LESS:
        return animator.paramMap.at(condition.paramName).value < condition.threshold;
    case CONDITION_MODE::EQUAL:
        return animator.paramMap.at(condition.paramName).value == condition.threshold;
    case CONDITION_MODE::NOT_EQUAL:
        return animator.paramMap.at(condition.paramName).value != condition.threshold;
    }

    return false;
}

void Animator::Impl::BindNotify(const std::string& stateKey, const std::string& notifyKey, IAnimNotify* Notifier)
{
    assert(Notifier);
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animatorController = AnimatorControllerMng::Get(animatorKey);
    const auto& animatorState = animatorController.stateMap.at(stateKey);
    auto& animMng = AnimationMng::Instance();
    const auto& notifiers = animMng.GetNotifiers(animatorState.animationList, animatorState.animationState);

    for (const auto& notify : notifiers)
    {
        if (strcmp(notify.name, notifyKey.c_str()) == 0)
        {
            notifiersMap.emplace(notifyKey, Notifier);
            notifyKeyFrame = notify.keyframe;
        }
    }

    notifiersMap.emplace(notifyKey, Notifier);
}

void Animator::Impl::Update(float deltaTime_s)
{
    (this->*checkTransFunc)();

    (this->*updateFunc)(deltaTime_s);

    Notify();
}

void Animator::Impl::Notify()
{
    if (!notifiersMap.count(currentAnimKey)) return;

    notifiersMap.at(currentAnimKey)->Notify(animator.lock().get());
}

void Animator::Impl::Render()
{
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animator = AnimatorControllerMng::Get(animatorKey);
    auto& animMng = AnimationMng::Instance();
    const auto& state = animator.stateMap.at(currentState);
    const auto& trans = transform.lock();
    const auto& animation = animMng.GetAnimation(state.animationList, state.animationState);
    auto sourceX = (currentDurationID % animation.texColumns) * animation.celWidth;
    auto sourceY = (currentDurationID / animation.texColumns) * animation.celHeight;

    // TODO: Implement pivot(rotation center) variable
    DxLib::DrawRectRotaGraphFast3F(trans->Pos.x, trans->Pos.y,
                                   sourceX, sourceY, animation.celWidth, animation.celHeight,
                                   animation.celWidth / 2.f, animation.celHeight / 2.f,
                                   trans->Scale.x, trans->Scale.y, trans->Rotation, animation.texId, 1,
                                   isFlipped);
}

void Animator::Impl::UpdateInfinite(float deltaTime_s)
{
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animatorController = AnimatorControllerMng::Get(animatorKey);
    const auto& animatorState = animatorController.stateMap.at(currentState);
    auto& animMng = AnimationMng::Instance();
    const auto& animation = animMng.GetAnimation(animatorState.animationList, animatorState.animationState);
    currentAnimKey = animMng.GetAnimationKey(animatorState.animationList, animatorState.animationState);

    if (timer_ms <= 0)
    {
        currentDurationID = (currentDurationID - animation.celBaseId + 1) % animation.celCount + animation.celBaseId;
        currentKeyFrame = (currentKeyFrame + 1) % animation.celCount;
        timer_ms = animMng.GetDuration_ms(currentDurationID) / playRate;
    }

    timer_ms -= static_cast<int>(deltaTime_s / MathHelper::kMsToSecond);
}

void Animator::Impl::UpdateLoop(float deltaTime_s)
{
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animatorController = AnimatorControllerMng::Get(animatorKey);
    const auto& animatorState = animatorController.stateMap.at(currentState);
    auto& animMng = AnimationMng::Instance();
    const auto& animation = animMng.GetAnimation(animatorState.animationList, animatorState.animationState);
    currentAnimKey = animMng.GetAnimationKey(animatorState.animationList, animatorState.animationState);

    if (timer_ms <= 0)
    {
        ++currentDurationID;
        timer_ms = animMng.GetDuration_ms(currentDurationID) / playRate;
    }

    if (currentDurationID >= (animation.celBaseId + animation.celCount))
    {
        --loopCount;
        currentDurationID = animation.celBaseId;
        timer_ms = animMng.GetDuration_ms(currentDurationID) / playRate;
    }

    if (loopCount < 0)
    {
        // Last durationId of current animation
        currentDurationID = animation.celBaseId + animation.celCount - 1;
        updateFunc = &Impl::UpdateSleep;
        return;
    }

    timer_ms -= static_cast<int>(deltaTime_s / MathHelper::kMsToSecond);
}

void Animator::Impl::UpdateSleep(float deltaTime_s)
{
}

void Animator::Impl::ActiveCheckTransition()
{
    if (!AnimatorControllerMng::Has(animatorKey)) return;
    const auto& animatorController = AnimatorControllerMng::Get(animatorKey);
    // Check conditions
    const auto& state = animatorController.stateMap.at(currentState);
    for (const auto& transition : state.transitions)
    {
        bool checkFlag = true;
        for (const auto& condition : transition.conditions)
            checkFlag = CheckCondition(condition) && checkFlag;

        if (checkFlag)
            Play(transition.destinationState);
    }
    //

    checkTransFunc = &Impl::Sleep;
}

void Animator::Impl::Sleep()
{
}

#pragma endregion


Animator::Animator(const std::shared_ptr<Entity>& owner):
    IComponent(owner), m_impl(std::make_unique<Impl>(owner))
{
}

Animator::~Animator()
{
}

void Animator::AddAnimatorController(const std::string& key)
{
    assert(AnimatorControllerMng::Has(key));

    m_impl->animatorKey = key;
    auto entryState = AnimatorControllerMng::Get(key).entryState;
    Play(entryState);
    m_impl->currentState = std::move(entryState);
}

void Animator::SetFloat(const std::string& name, float value)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::FLOAT);
    animatorController.paramMap.at(name).value = value;

    m_impl->checkTransFunc = &Impl::ActiveCheckTransition;
}

void Animator::SetBool(const std::string& name, bool flag)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::BOOL);
    animatorController.paramMap.at(name).value = static_cast<float>(flag);

    m_impl->checkTransFunc = &Impl::ActiveCheckTransition;
}

void Animator::SetInteger(const std::string& name, int value)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::INTEGER);
    animatorController.paramMap.at(name).value = static_cast<float>(value);

    m_impl->checkTransFunc = &Impl::ActiveCheckTransition;
}

float Animator::GetFloat(const std::string& name)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    const auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::FLOAT);
    return animatorController.paramMap.at(name).value;
}

bool Animator::GetBool(const std::string& name)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    const auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::BOOL);
    return static_cast<bool>(animatorController.paramMap.at(name).value);
}

int Animator::GetInteger(const std::string& name)
{
    assert(AnimatorControllerMng::Has(m_impl->animatorKey));
    const auto& animatorController = AnimatorControllerMng::Get(m_impl->animatorKey);
    assert(animatorController.HasParameter(name));
    assert(animatorController.paramMap.at(name).type == ANIMATOR_PARAMETER_TYPE::INTEGER);
    return static_cast<int>(animatorController.paramMap.at(name).value);
}

void Animator::SetFlip(bool isFlipped)
{
    m_impl->isFlipped = isFlipped;
}

void Animator::SetPlayRate(float playRate)
{
    assert(playRate != 0.f);
    m_impl->playRate = playRate;
}

void Animator::Play(const std::string& animatorState)
{
    m_impl->Play(animatorState);
}

void Animator::Notify()
{
    m_impl->Notify();
}

void Animator::BindNotify(const std::string& stateKey, const std::string& notifyKey, IAnimNotify* Notifier)
{
    m_impl->BindNotify(stateKey, notifyKey, Notifier);
}

void Animator::Init()
{
}

void Animator::Update(float deltaTime_s)
{
    m_impl->Update(deltaTime_s);
}

void Animator::Render()
{
    m_impl->Render();
}
