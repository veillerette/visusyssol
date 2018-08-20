#pragma once

#include <vector>

#include "common.hpp"

namespace glimac {

class Cube {
	void build(GLfloat radius);
private:
	std::vector<ShapeVertex> m_Vertices;
	GLsizei m_nVertexCount;
	
public:
	Cube(GLfloat radius):
		m_nVertexCount(0) {
		build(radius);
	}
	
	const ShapeVertex *getDataPointer() const {
		return &m_Vertices[0];
	}
	
	GLsizei getVertexCount() const {
		return m_nVertexCount;
	}	
};

}
