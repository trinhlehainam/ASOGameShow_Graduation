#pragma once
#include "../Component/Animation/AnimatorController.h"

class AnimatorControllerMng
{
public:
	static void Create();
	static void Destroy();
	static AnimatorControllerMng& Instance();

public:
	static void LoadFromXML(const std::string& path);
	static AnimatorController& Get(const std::string& animatorControllerKey);
	static bool Has(const std::string& animatorControllerKey);
private:
	AnimatorControllerMng();
	~AnimatorControllerMng();
private:
	// Not allow copy and move
	// Body of these functions are empty
	AnimatorControllerMng(const AnimatorControllerMng&);
	AnimatorControllerMng(AnimatorControllerMng&&) noexcept;
	void operator = (const AnimatorControllerMng&);
	void operator = (AnimatorControllerMng&&) noexcept;
private:
	std::unordered_map<std::string, AnimatorController> m_animatorMap;
	static AnimatorControllerMng* m_instance;
};

