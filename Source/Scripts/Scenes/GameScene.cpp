#include "GameScene.h"

#include <DxLib.h>
#include <rapidxml.hpp>

#include "MenuScene.h"
#include "../../Systems/Renderer.h"
#include "../../Systems/Physics.h"
#include "../../Systems/EntityMng.h"
#include "../../TileMap.h"
#include "../../Input/IInput.h"
#include "../GameObjects/Player.h"
#include "../../GameObject/Entity.h"
#include "../../Systems/Application.h"
#include "../../Systems/Controller.h"
#include "../../Systems/Time.h"

namespace
{
	constexpr int kScreenWidth = 1024;
	constexpr int kScreenHeight = 768;
}

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	m_entityMng->ClearData();
	Physics::ClearData();
}

bool GameScene::Init()
{

	m_map = std::make_shared<TileMap>(m_entityMng, "Assets/Map/map.xml", "map");
	m_player = std::make_shared<Player>(m_entityMng);

	m_player->Init(INPUT_DEVICE::KEYBOARD);

	for (const auto& entity : m_entityMng->GetAllEntities())
	{
		if (!entity->HasComponent<ICollider>()) continue;
		auto collider = entity->GetComponent<ICollider>();
		Physics::AddCollider(collider);
	}

	for (const auto& entity : m_entityMng->GetAllEntities())
	{
		if (!entity->HasComponent<SpriteComponent>()) continue;
		auto renderer = entity->GetComponent<SpriteComponent>();
		Renderer::AddRenderer(renderer);
	}

	RenderToOwnScreen();

    return true;
}

void GameScene::Update(float deltaTime_s)
{
	const auto Controller = Application::GetController();

	if(Controller && Controller->IsJustPressed(INPUT_ID::BTN4))
	{
		Application::PushScene(std::make_unique<MenuScene>());
	}	

	const auto& time = Time::Instance();

	// Player Input
	m_player->Update(deltaTime_s);
	
	// Apply Physics Force and Momentum
	Physics::ApplyForce();
	
	// Update all Entities Components
	m_entityMng->Update(time.DeltaTime_s());
	
	// Physics Collision and Resolution
	Physics::PlatformResolution();
	Physics::Update();
}

void GameScene::Render()
{
	IScene::Render();
}

void GameScene::RenderToOwnScreen()
{
	DxLib::SetDrawScreen(m_screenID);
	DxLib::ClearDrawScreen();

	m_map->Render();
	m_entityMng->Render();
	m_player->Render();
	Physics::Render();
}

std::unique_ptr<IScene> GameScene::ChangeScene(std::unique_ptr<IScene>)
{
	return std::unique_ptr<IScene>();
}

SCENE_ID GameScene::GetSceneID()
{
	return SCENE_ID::GAME;
}
