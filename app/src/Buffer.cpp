#include "../include/Buffer.h"

Buffer::Buffer() {
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ibo);
	color = false;
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &ibo);
}

void Buffer::fillVertices(const ShapeVertex *vertices, const unsigned int n) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertex) * n, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	color = false;
}

void Buffer::fillVertices(const ColorShapeVertex *vertices, const unsigned int n) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ColorShapeVertex) * n, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	color = true;
}

void Buffer::constructVertexArray() {
	enableVAO();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glEnableVertexAttribArray(ID_POSITION);
	glEnableVertexAttribArray(ID_NORMAL);
	glEnableVertexAttribArray(ID_TEXTURE);
	if(color)
		glEnableVertexAttribArray(ID_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	if(color) {
		glVertexAttribPointer(ID_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ColorShapeVertex), (const GLvoid*)offsetof(ColorShapeVertex, position));
		glVertexAttribPointer(ID_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ColorShapeVertex),  (const GLvoid*)offsetof(ColorShapeVertex, normal));
		glVertexAttribPointer(ID_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(ColorShapeVertex),  (const GLvoid*)offsetof(ColorShapeVertex, texCoords));
		glVertexAttribPointer(ID_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(ColorShapeVertex),  (const GLvoid*)offsetof(ColorShapeVertex, color));
	} else {
		glVertexAttribPointer(ID_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (const GLvoid*)offsetof(ShapeVertex, position));
		glVertexAttribPointer(ID_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),  (const GLvoid*)offsetof(ShapeVertex, normal));
		glVertexAttribPointer(ID_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),  (const GLvoid*)offsetof(ShapeVertex, texCoords));
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	disableVAO();
}

void Buffer::fillIndexes(const uint32_t *indexes, const unsigned int n) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n * sizeof(uint32_t), indexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    this->number = n;

    constructVertexArray();
}

void Buffer::drawElements() {
	enableVAO();
	glDrawElements(GL_TRIANGLES, this->number, GL_UNSIGNED_INT, nullptr);
	disableVAO();
}

void Buffer::enableVAO() {
    glBindVertexArray(vao);
}

void Buffer::disableVAO() {
    glBindVertexArray(0);
}