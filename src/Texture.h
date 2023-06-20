#pragma once

#include "Utilities.h"

class Texture{
private:
	unsigned int m_RenderID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_CPP;//channels per pixel

public:
	Texture(const std::string &filePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
};
