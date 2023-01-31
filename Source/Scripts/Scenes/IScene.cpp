#include "IScene.h"

#include <cassert>

#include <DxLib.h>

#include "../../Systems/EntityMng.h"

IScene::IScene() :
	IsChangeSceneRequested(false),
	m_screenOffsetX(0.0f), m_screenOffsetY(0.0f), m_entityMng(std::make_shared<EntityMng>())
{
	m_entityMng->m_self = m_entityMng;

	SetDrawScreen(DX_SCREEN_BACK);
	GetDrawScreenSize(&m_screenWidth, &m_screenHeight);
	m_screenID = DxLib::MakeScreen(m_screenWidth, m_screenHeight, 1);
	if (m_screenID == -1)
		assert(0);
}

IScene::~IScene()
{
	DeleteGraph(m_screenID);
}

void IScene::SetName(std::string name)
{
	m_name = std::move(name);
}

std::string IScene::GetName() const
{
	return m_name;
}

void IScene::Render()
{
	DxLib::DrawGraphF(m_screenOffsetX, m_screenOffsetY, m_screenID, 0);
}
