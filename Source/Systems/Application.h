#pragma once
#include <memory>

// Singleton
class Application
{
public:
	static Application& Instance();
	~Application();

	bool Init();
	void Run();
	void Exit();

private:
	Application();
private:
	// Don't allow copy and move semantics
	Application(const Application&);
	Application(Application&&) noexcept;
	void operator = (const Application&);
	void operator = (Application&&) noexcept;
	//
private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

