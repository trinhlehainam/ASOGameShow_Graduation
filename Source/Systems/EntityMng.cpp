#include "EntityMng.h"

#include "../GameObject/Entity.h"

EntityMng::EntityMng():m_currentID(0)
{
}

EntityMng::~EntityMng()
{
}

bool EntityMng::HasEntity(int entityID) const
{
	return m_entityMap.count(entityID);
}

std::shared_ptr<Entity> EntityMng::AddEntity(std::string tag)
{
	auto entity = std::make_shared<Entity>(m_self.lock(),std::move(tag));
	entity->m_id = m_currentID;
	m_entities.push_back(entity);
	m_entityMap.emplace(m_currentID, entity);
	++m_currentID;
	return entity ;
}

void EntityMng::AddEntity(const std::shared_ptr<Entity>& entity)
{
	if (HasEntity(entity->GetEntityID())) return;
	entity->m_id = m_currentID;
	m_entities.push_back(entity);
	m_entityMap.emplace(m_currentID, entity);
	++m_currentID;
}

const std::vector<std::shared_ptr<Entity>>& EntityMng::GetAllEntities() const
{
	return m_entities;
}

void EntityMng::Update(float deltaTime_s)
{
	for (const auto& entity : m_entities)
		entity->Update(deltaTime_s);
}

void EntityMng::Render()
{
	for (const auto& entity : m_entities)
		entity->Render();
}
