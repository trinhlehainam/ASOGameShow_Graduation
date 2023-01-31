#pragma once
#include <memory>

#include "../../Input/IInput.h"

class Entity;
class EntityMng;
class InputCommand;
class RigidBody2D;

class Player
{
public:
	Player(const std::shared_ptr<EntityMng>& entityMng);
	~Player();
public:
	void Init(INPUT_DEVICE deviceId);
	void Update(float deltaTime_s);
	void Render();
	void Destroy();

	std::shared_ptr<Entity> GetEntity() const;
private:
	std::shared_ptr<Entity> m_entity;
	std::shared_ptr<RigidBody2D> m_body;
	std::shared_ptr<IInput> m_input;
	std::shared_ptr<InputCommand> m_inputCommand;

	bool isJumpRequested;
};

