#include "Renderer.h"

#include <iostream>

void Renderer::Clear(){
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, float lw) const{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glLineWidth(lw));										//设置线宽
	GLCall(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawLinesStripe(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, float lw) const{
	shader.Bind();
	va.Bind();
	ib.Bind();

	GLCall(glLineWidth(lw));										//设置线宽
	GLCall(glDrawElements(GL_LINE_STRIP, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
