#ifndef DEF_HEADER_BUFFER
#define DEF_HEADER_BUFFER

#include <iostream>
#include <glimac/glm.hpp>
#include <GL/glew.h>

using glm::vec3;
using glm::vec2;

typedef struct ShapeVertex ShapeVertex;
struct ShapeVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    ShapeVertex(vec3 p, vec3 n, vec2 t) : position(p), normal(n), texCoords(t) {}
    ShapeVertex() : ShapeVertex(vec3(0, 0, 0), vec3(0, 0, 0), vec2(0, 0)) {}
};

typedef struct ColorShapeVertex ColorShapeVertex;
struct ColorShapeVertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
    vec3 color;
    ColorShapeVertex(vec3 p, vec3 n, vec2 t, vec3 c) : position(p), normal(n), texCoords(t), color(c) {}
    ColorShapeVertex() : ColorShapeVertex(vec3(0, 0, 0), vec3(0, 0, 0), vec2(0, 0), vec3(0, 0, 0)) {}
};

class Buffer {
public:
    static const unsigned int ID_POSITION = 0;
    static const unsigned int ID_NORMAL = 1;
    static const unsigned int ID_TEXTURE = 2;
    static const unsigned int ID_COLOR = 3;

    Buffer();

    ~Buffer();

    void drawElements();

    /* Fill the buffer with vertices (position, normal, texture coord) */
    void fillVertices(const ShapeVertex *vertices, const unsigned int n);
    
    void fillVertices(const ColorShapeVertex *vertices, const unsigned int n);

    /* Fill the buffer with the indexes of the shape */
    void fillIndexes(const uint32_t *indexes, const unsigned int n);

    /* enable and disable current VAO, useful to send rendering things */
    void enableVAO();

    void disableVAO();

    //friend GeometryFactory;

private:
    GLuint vbo;
    GLuint vao;
    GLuint ibo;
    unsigned int number;
    
    bool color;

    void constructVertexArray();
};

#endif
