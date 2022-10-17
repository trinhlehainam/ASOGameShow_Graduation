#pragma once
#include <memory>
#include <vector>

class SpriteComponent;

class Renderer
{
public:
	static void Create();
	static void Destroy();
	static Renderer& Instance();

	static void AddRenderer(const std::shared_ptr<SpriteComponent>& renderComponent);
	static void Render();
private:
	Renderer();
	~Renderer();

	// Don't allow copy and move semantics
	Renderer(const Renderer&);
	Renderer(Renderer&&) noexcept;
	void operator = (const Renderer&);
	void operator = (Renderer&&) noexcept;
private:
	std::vector<std::weak_ptr<SpriteComponent>> m_renderers;
	static Renderer* m_instance;
};

