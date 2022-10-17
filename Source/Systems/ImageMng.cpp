#include "ImageMng.h"

#include <cassert>

#include <DxLib.h>

#include "../Utilities/MacroHelper.h"

GenerateDynamicSingleton(ImageMng);

bool ImageMng::AddImage(const std::string& fileName, const std::string& key)
{
    if (m_instance == nullptr) return false;

    if (m_instance->m_handleMap.count(key)) return false;
    m_instance->m_handleMap[key] = DxLib::LoadGraph(fileName.c_str());
    if (m_instance->m_handleMap[key] == -1) return false;

    return true;
}

void ImageMng::DeleteImage(const std::string& key)
{
    if (m_instance == nullptr) return;

    if (!m_instance->m_handleMap.count(key)) return;
    DxLib::DeleteGraph(m_instance->m_handleMap.at(key));
    m_instance->m_handleMap.erase(key);
}

int ImageMng::GetID(const std::string& key)
{
    if (m_instance == nullptr) return -1;

    return m_instance->m_handleMap.at(key);
}

ImageMng::ImageMng() {}
ImageMng::~ImageMng() {}

