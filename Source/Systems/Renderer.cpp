#include "Renderer.h"

#include "../Utilities/MacroHelper.h"

#include "../Component/SpriteComponent.h"

GenerateDynamicSingleton(Renderer);

void Renderer::AddRenderer(const std::shared_ptr<SpriteComponent>& renderComponent)
{
	m_instance->m_renderers.push_back(renderComponent);
}

void Renderer::Render()
{
	for (const auto& renderer : m_instance->m_renderers)
		renderer.lock()->Render();
}

Renderer::Renderer() {}
Renderer::~Renderer() {}
