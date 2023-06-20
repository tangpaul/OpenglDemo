#pragma once

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Utilities.h"

class Renderer{

public:
	void Clear();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, float lw = 1.0f) const;
	void DrawLinesStripe(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, float lw = 1.0f) const;
};