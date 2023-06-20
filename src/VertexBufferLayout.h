#pragma once

#include "glad/glad.h"
#include <vector>

#include "Utilities.h"

struct VertexBufferElement	{
	unsigned int type;
	unsigned int count;
	unsigned char normalize;

	unsigned int GetElementSize() const{
		switch(type)	{
			case GL_FLOAT: return count * sizeof(float);
			case GL_UNSIGNED_INT: return count * sizeof(unsigned int);
			case GL_UNSIGNED_BYTE: return count * sizeof(unsigned char);
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout{
private:
	std::vector<VertexBufferElement>	m_Elements;
	unsigned int						m_Stride;
public:
	VertexBufferLayout();
	~VertexBufferLayout();

	template <typename T>
	void add(unsigned int count){
#ifdef __linux__
		throw;
#else
		static_assert(false);
#endif
	}

	inline const std::vector<VertexBufferElement> GetElements() const{ return m_Elements; }
	inline unsigned int GetStride() const{ return m_Stride; }
};

template <>
inline void VertexBufferLayout::add<float>(unsigned int count){
	m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
	m_Stride += m_Elements[m_Elements.size() - 1].GetElementSize();
}

template <>
inline void VertexBufferLayout::add<unsigned int>(unsigned int count){
	m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
	m_Stride += m_Elements[m_Elements.size() - 1].GetElementSize();
}

template <>
inline void VertexBufferLayout::add<unsigned char>(unsigned int count){
	m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
	m_Stride += m_Elements[m_Elements.size() - 1].GetElementSize();
}