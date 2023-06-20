#include "VertexArray.h"

VertexArray::VertexArray(){
	GLCall(glGenVertexArrays(1, &m_RenderID));
	GLCall(glBindVertexArray(m_RenderID));
}

VertexArray::~VertexArray(){
	GLCall(glDeleteVertexArrays(1, &m_RenderID));
}

void VertexArray::Bind() const{
	GLCall(glBindVertexArray(m_RenderID));
}

void VertexArray::Unbind() const{
	GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){

	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for(int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalize, layout.GetStride(), (const void*)offset));
		offset += element.GetElementSize();
	}
}
