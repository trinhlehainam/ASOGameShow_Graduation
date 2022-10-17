#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <string>

#include "../Component/IComponent.h"

class EntityMng;

class Entity
{
public:
	Entity();
	explicit Entity(std::string tag);
	explicit Entity(const std::shared_ptr<EntityMng>& entityMng);
	explicit Entity(const std::shared_ptr<EntityMng>& entityMng, std::string tag);
	~Entity();

	void SetActive(bool activeFlag);
	void SetTag(std::string tag);
	void SetEntityMng(const std::shared_ptr<EntityMng>& entityMng);

	std::string GetTag() const;
	int GetEntityID() const;
	bool IsActive() const;

	void Update(float deltaTime_s);
	void Render();

	template<typename T>
	void AddComponent(const std::shared_ptr<Entity>& owner);

	template<typename T>
	bool HasComponent();

	template<typename T>
	std::shared_ptr<T> GetComponent();

private:
	std::vector<std::shared_ptr<IComponent>> m_components;
	// type_info's data deleted DEFAULT constructor
	// type_info can be only instantiated by typeid
	// -> point to type_info after it's created
	std::unordered_map<const type_info*, std::shared_ptr<IComponent>> m_componentMap;
	bool m_isActive;
	std::string m_tag;
	int m_id;
	std::weak_ptr<EntityMng> m_entityMng;

	//TODO : Implement hierarchy game objects
	std::vector<std::weak_ptr<Entity>> m_childs;

private:
	friend EntityMng;
};

