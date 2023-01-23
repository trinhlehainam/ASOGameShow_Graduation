#include "EntityMng.h"

#include "../GameObject/Entity.h"

namespace
{
    template <typename T>
    void ProcessRemoveEntities(std::vector<std::shared_ptr<T>>& container)
    {
        container.erase(std::remove_if(container.begin(), container.end(), [](std::shared_ptr<T>& entity)
                        {
                            return !entity->IsActive();
                        }),
                        container.end());
    }
}

EntityMng::EntityMng(): m_currentID(0), m_isRemoveEntities(false)
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
    auto entity = std::make_shared<Entity>(m_self.lock(), std::move(tag));
    entity->m_id = m_currentID;
    m_entities.push_back(entity);
    m_entityMap.emplace(m_currentID, entity);
    ++m_currentID;
    return entity;
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

    if (m_isRemoveEntities)
    {
        ProcessRemoveEntities(m_entities);
        m_isRemoveEntities = false;
    }
}

void EntityMng::Render()
{
    for (const auto& entity : m_entities)
        entity->Render();
}

void EntityMng::RequestDestroy()
{
    m_isRemoveEntities = true;
}

void EntityMng::ClearData()
{
    m_entities.clear();
}
