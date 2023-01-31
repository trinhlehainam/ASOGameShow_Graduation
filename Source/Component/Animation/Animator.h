#pragma once
#include "../IComponent.h"

#include <string>

#include "IAnimNotify.h"

class Animator : public IComponent
{
public:
	explicit Animator(const std::shared_ptr<Entity>& owner);
	~Animator();

	void AddAnimatorController(const std::string& animatorControllerKey);

	void SetFloat(const std::string& name, float value);
	void SetBool(const std::string& name, bool flag);
	void SetInteger(const std::string& name, int value);

	float GetFloat(const std::string& name);
	bool GetBool(const std::string& name);
	int GetInteger(const std::string& name);

	void SetFlip(bool isFlipped);
	void SetPlayRate(float playRate);

	void Play(const std::string& animatorState);
	void Notify();

	void BindNotify(const std::string& stateKey, const std::string& notifyKey, IAnimNotify* Notifier);
	
public:
	void Init() override;
	void Update(float deltaTime_s) override;
	void Render() override;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

