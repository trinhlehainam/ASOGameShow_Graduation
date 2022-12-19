#pragma once
#include <memory>

class IScene;

// Singleton
class Application
{
public:
	static Application& Instance();

	static bool Initialize();
	static void Execute();
	static void Terminate();

	static void ChangeScene(std::unique_ptr<IScene> scene);
	static void ClearScene();
	static void ResetScene(std::unique_ptr<IScene> scene);
	static void PushScene(std::unique_ptr<IScene> scene);
	static void PopScene();
	
private:
	Application();
	~Application();
	
	bool Init();
	void Run();
	void Exit();
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

