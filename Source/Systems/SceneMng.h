#pragma once
#include <memory>

// Singleton
class SceneMng
{
public:
	static SceneMng& Instance();
	~SceneMng();

	bool Init();
	void Run();
	void Exit();

private:
	SceneMng();
private:
	// Don't allow copy and move semantics
	SceneMng(const SceneMng&);
	SceneMng(SceneMng&&) noexcept;
	void operator = (const SceneMng&);
	void operator = (SceneMng&&) noexcept;
	//
private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

