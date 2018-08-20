#include "../include/Geometry.h"

#include <iostream>
#include <math.h>
#include <functional>

using namespace std;

using glm::vec3;
using glm::vec2;
using glm::mat4;
using glm::vec4;

Geometry :: Geometry(unsigned int _size, unsigned int _faces) : size(_size), faces(_faces),
	vertices(new ShapeVertex[_size]), triangles(new unsigned int[_faces * 3]) {
}

Geometry :: ~Geometry() {
	delete [] vertices;
	delete [] triangles;
}

void Geometry :: draw() {
	buffer.drawElements();
}

void Geometry :: fillBuffer() {
	buffer.fillVertices(vertices, size);
	buffer.fillIndexes(triangles, faces * 3);
}

void Geometry::enable() {
	buffer.enableVAO();
}

void Geometry::disable() {
	buffer.disableVAO();
}

/* Color Geometry */

ColorGeometry :: ColorGeometry(unsigned int _size, unsigned int _faces) : Geometry(0, _faces),
	vertices(new ColorShapeVertex[_size]), size(_size) {
}

ColorGeometry :: ColorGeometry(Geometry * geo) : ColorGeometry(geo->size, geo->faces) {
	for(int i = 0; i < geo->size; ++i)
		vertices[i] = ColorShapeVertex(geo->vertices[i].position, geo->vertices[i].normal, geo->vertices[i].texCoords, vec3(0.5, 0.5, 0.5));
	for(int i = 0; i < geo->faces * 3; ++i)
		Geometry :: triangles[i] = geo->triangles[i];
}

ColorGeometry :: ~ColorGeometry() {
	delete [] vertices;
}

void ColorGeometry :: fillBuffer() {
	buffer.fillVertices(vertices, size);
	buffer.fillIndexes(Geometry :: triangles, Geometry :: faces * 3);
}