#include "TileMap.h"

#include <sstream>

#include <rapidxml.hpp>
#include <DxLib.h>

#include "Utilities/StringHelper.h"

#include "GameObject/Entity.h"
#include "Systems/TextureMng.h"
#include "Systems/EntityMng.h"
#include "Component/TransformComponent.h"
#include "Component/Collider/BoxCollider.h"

TileMap::TileMap(const std::shared_ptr<EntityMng>& entityMng):m_mapImageID(0), m_entityMng(entityMng) {}

TileMap::TileMap(const std::shared_ptr<EntityMng>& entityMng, const std::string& xmlFile, const std::string& key): m_entityMng(entityMng)
{
	LoadMapDataFromXML(xmlFile, key);
}

TileMap::~TileMap()
{
}

void TileMap::LoadMapDataFromXML(const std::string& fileName, const std::string& key)
{
	auto entityMng = m_entityMng.lock();

	rapidxml::xml_document<> doc;
	auto content = StringHelper::LoadFileToStringBuffer(fileName);
	doc.parse<0>(&content[0]);

	auto pMap = doc.first_node();

	// Tile info
	for (auto pAttr = pMap->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
	{
		if (strcmp(pAttr->name(), "width") == 0)
		{
			m_numTile.x = std::atoi(pAttr->value());
		}
		else if (strcmp(pAttr->name(), "height") == 0)
		{
			m_numTile.y = std::atoi(pAttr->value());
		}
		else if (strcmp(pAttr->name(), "tileheight") == 0)
		{
			m_tileSize.x = std::atoi(pAttr->value());
		}
		else if (strcmp(pAttr->name(), "tilewidth") == 0)
		{
			m_tileSize.y = std::atoi(pAttr->value());
		}
	}
	//

	// Source Image Info
	auto pTileSet = pMap->first_node("tileset");
	std::string imageSource;
	for (auto pAttr = pTileSet->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
	{
		if (strcmp(pAttr->name(), "source") == 0)
		{
			imageSource += pAttr->value();
		}
	}

	auto imageContent = StringHelper::LoadFileToStringBuffer(imageSource.c_str());
	rapidxml::xml_document imageDoc;
	imageDoc.parse<0>(&imageContent[0]);
	auto pImageTileset = imageDoc.first_node();
	auto pImageNode = pImageTileset->first_node("image");
	for (auto pAttr = pImageNode->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
	{
		if (strcmp(pAttr->name(), "source") == 0)
		{
			std::string imageFile{ pAttr->value() };

			// Load source image to ImageMng and take image info
			TextureMng::AddImage(imageFile, key);
			m_mapImageID = TextureMng::GetID("map");
			DxLib::GetGraphSize(m_mapImageID, &m_mapImageSize.x, &m_mapImageSize.y);
			m_numColumns = m_mapImageSize.x / m_tileSize.x;
			//
			imageDoc.clear();

			break;
		}
	}
	//

	// Read tile's data (image ID and tile's position) to each layer
	for (auto pLayer = pMap->first_node("layer"); pLayer; pLayer = pLayer->next_sibling())
	{
		if (strcmp(pLayer->name(), "objectgroup") == 0)
			continue;

		auto pData = pLayer->first_node("data");
		std::string layerName{ pLayer->first_attribute("name")->value() };
		std::stringstream data{ std::move(pData->value()) };
		int tilePos = 0;
		while (!data.eof())
		{
			std::string line;
			data >> line;
			int sourceID = 0;
			std::stringstream ssLine{ line };
			while (ssLine >> sourceID)
			{
				if (!(sourceID == 0))
				{
					// SourceID is added 1 by Tile tool to separate with empty (0)
					// --> Minus 1 to move back to right ID
					m_layerMap[layerName].emplace_back(tilePos, sourceID - 1);
				}

				if (ssLine.peek() == ',')
					ssLine.ignore();

				++tilePos;
			}
		}
	}
	//

	rapidxml::xml_node<char>* pObjectgroup = nullptr;
	for (auto pLayer = pMap->first_node("layer"); pLayer; pLayer = pLayer->next_sibling())
	{
		if (strcmp(pLayer->name(), "objectgroup") == 0)
		{
			pObjectgroup = pLayer;
			break;
		}
	}

	for (auto pObject = pObjectgroup->first_node(); pObject; pObject = pObject->next_sibling())
	{
		vec2f origin;
		vec2f size;
		for (auto pAttr = pObject->first_attribute(); pAttr; pAttr = pAttr->next_attribute())
		{
			if (strcmp(pAttr->name(), "x") == 0)
			{
				std::string data{ std::move(pAttr->value()) };
				origin.x = std::stof(data);
			}
			else if (strcmp(pAttr->name(), "y") == 0)
			{
				std::string data{ std::move(pAttr->value()) };
				origin.y = std::stof(data);
			}
			else if (strcmp(pAttr->name(), "width") == 0)
			{
				std::string data{ std::move(pAttr->value()) };
				size.x = std::stof(data);
			}
			else if (strcmp(pAttr->name(), "height") == 0)
			{
				std::string data{ std::move(pAttr->value()) };
				size.y = std::stof(data);
			}
		}

		auto entity = std::make_shared<Entity>(m_entityMng.lock(), "tile-collision");
		entity->AddComponent<TransformComponent>(entity);
		auto transform = entity->GetComponent<TransformComponent>();
		transform->Pos = origin;
		entity->AddComponent<BoxCollider>(entity);
		auto collider = entity->GetComponent<BoxCollider>();
		collider->SetOrigin(origin);
		collider->SetSize(size);
		entityMng->AddEntity(std::move(entity));
	}

	doc.clear();
}

void TileMap::Update(float deltaTime_s)
{
}

void TileMap::Render()
{
	for (const auto& layer : m_layerMap)
		for (const auto& tile : layer.second)
		{
			auto tileWorldPos = GetTileWorldPos(tile.TilePos);
			auto sourcePos = GetTileSourcePos(tile.ID);
			DxLib::DrawRectGraphF(tileWorldPos.x, tileWorldPos.y, sourcePos.x, sourcePos.y, m_tileSize.x, m_tileSize.y, m_mapImageID, 1);
		}
}

vec2f TileMap::GetTileWorldPos(int tilePos)
{
	float x = (tilePos % m_numTile.x) * static_cast<float>(m_tileSize.x);
	float y = (tilePos / m_numTile.x) * static_cast<float>(m_tileSize.y);
	return vec2f{m_worldPos.x + x , m_worldPos.y + y};
}

vec2i TileMap::GetTileSourcePos(int sourceID)
{
	int x = (sourceID % m_numColumns) * m_tileSize.x;
	int y = (sourceID / m_numColumns) * m_tileSize.y;
	return vec2i{ x,y };
}
