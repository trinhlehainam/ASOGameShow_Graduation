#include "GameScene.h"

#include <DxLib.h>
#include <rapidxml.hpp>

#include "MenuScene.h"
#include "../Systems/Renderer.h"
#include "../Systems/Physics.h"
#include "../Systems/AnimationMng.h"
#include "../Systems/AnimatorControllerMng.h"
#include "../Systems/EntityMng.h"
#include "../TileMap.h"
#include "../Input/IInput.h"
#include "../GameObject/Player.h"
#include "../GameObject/Entity.h"
#include "../Systems/Application.h"

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
}

bool GameScene::Init()
{
	AnimationMng::LoadFromXML("Assets/Animations/animation.xml");
	AnimatorControllerMng::LoadFromXML("Assets/Animators/playerAnimator.xml");

	m_map = std::make_shared<TileMap>(m_entityMng, "Assets/Map/map.xml", "map");
	m_player = std::make_shared<Player>(m_entityMng);

	m_player->Init(INPUT_DEVICE_ID::KEYBOARD);

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
	if(DxLib::CheckHitKey(KEY_INPUT_P))
	{
		Application::PushScene(std::make_unique<MenuScene>());
	}	
	
	m_player->Update(deltaTime_s);
	m_entityMng->Update(deltaTime_s);
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
}

std::unique_ptr<IScene> GameScene::ChangeScene(std::unique_ptr<IScene>)
{
	return std::unique_ptr<IScene>();
}

SCENE_ID GameScene::GetSceneID()
{
	return SCENE_ID::GAME;
}
