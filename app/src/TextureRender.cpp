#include "../include/TextureRender.h"

bool TextureRender::loadImageTexture(GLuint dest, const char *path) {
    std::unique_ptr<glimac::Image> source = glimac::loadImage(path);
    if (nullptr == source) {
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, dest);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 source->getWidth(), source->getHeight(),
                 0, GL_RGBA, GL_FLOAT, source->getPixels());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

TextureRender::TextureRender(Geometry *_geometry, GLuint _dest, const char * _uniformValue, Shader * _shader) :
	Render(_geometry), dest(_dest), uniformValue((char *)_uniformValue), shader(_shader) {
    glGenTextures(1, &texture);
}

TextureRender::~TextureRender() {
    glDeleteTextures(1, &texture);
}

bool TextureRender::load(const std::string path) {
    if (path.empty()) {
        return false;
    }
    return loadImageTexture(texture, path.c_str());
}

void TextureRender::render() {
	shader->uniformValue(uniformValue, (GLint)(dest - GL_TEXTURE0));
    enable();
    glActiveTexture(dest);
    glBindTexture(GL_TEXTURE_2D, texture);
    disable();
}

void TextureRender::postRender() {
    shader->uniformValue(uniformValue, (GLint)(dest - GL_TEXTURE0));
    enable();
    glActiveTexture(dest);
    glBindTexture(GL_TEXTURE_2D, 0);
    disable();
}