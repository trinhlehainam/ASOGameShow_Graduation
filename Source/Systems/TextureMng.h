#pragma once
#include <string>
#include <unordered_map>

// Singleton
class TextureMng
{
public:
	static void Create();
	static void Destroy();
	static TextureMng& Instance();

	static bool AddImage(const std::string& fileName, const std::string& key);
	static void DeleteImage(const std::string& key);
	static int GetID(const std::string& key);
	
private:
	TextureMng();
	~TextureMng();
	
private:
	// Don't allow copy and move semantics
	TextureMng(const TextureMng&);
	TextureMng(TextureMng&&) noexcept;
	void operator = (const TextureMng&);
	void operator = (TextureMng&&) noexcept;
	//

private:
	std::unordered_map<std::string, int> m_handleMap;

	static TextureMng* m_instance;
};

