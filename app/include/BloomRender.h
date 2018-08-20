#ifndef VISUSYSSOL_BLOOMRENDER_H
#define VISUSYSSOL_BLOOMRENDER_H

#include "Render.h"
#include <glimac/Image.hpp>
#include <string>
#include "Shader.h"
#include "Window.h"

class BloomRender{
public:
    BloomRender(Window *_window, bool msaa, const char *argv[], unsigned int _iter);
    ~BloomRender();
    void render(std::function<void()> sceneRender);

private:
    unsigned int iter;
    Window *window;
    const GLuint colors[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    GLuint fbo, tex[2], rbo;
    GLuint msfbo, mstex[2], msrbo;
    GLuint fboblur[2], texblur[2];
    Geometry *quad;
    Shader *screenShader;
    Shader *screenFinal;
    const int textureWidth = 455;
    const int textureHeight = 256;
    bool msaa;

    void init();
    void initBlur();
    void genLinearTexture(GLuint tex, GLint format, GLuint i, int width, int height);
    void genMSLinearTexture(GLuint tex, GLint format, GLuint i, int width, int height);

};

#endif //VISUSYSSOL_BLOOMRENDER_H