#pragma once
#include "VertexBuffer.h"

class VertexBufferLayout;

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AdddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void UnBind() const;
private:
	unsigned int m_RendererID;
};