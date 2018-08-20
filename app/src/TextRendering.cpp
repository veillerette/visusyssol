#include "../include/TextRendering.h"
#include "../include/GeometryFactory.h"


TextRendering::TextRendering(int screenWidth, int screenHeight, const char *bitmapPath, const char *argv[])
    : map(), width(screenWidth), height(screenHeight) {
    glGenTextures(1, &tex);
    if (!TextureRender::loadImageTexture(tex, bitmapPath)) {
        std::cerr << "Unable to load Bitmap font texture" << std::endl;
        exit(EXIT_FAILURE);
    }
    shader = new Shader(argv, "text.vs.glsl", "text.fs.glsl");

}

TextRendering::~TextRendering() {
    glDeleteTextures(1, &tex);
    delete shader;

    for(auto p : map) {
        delete p.second;
    }
}

Geometry *TextRendering::getGeo(std::pair<std::string, unsigned int> pair) {
    if (map.find(pair) != map.cend()) {
        return map[pair];
    }
    Geometry *geo = GeometryFactory::Geometry_create_quad_letter(pair.second, pair.first.c_str());
    map[pair] = geo;
    return geo;
}

void TextRendering::drawText(std::string text, int x, int y, unsigned int size) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    auto *geo = getGeo(std::pair<std::string, unsigned int>(text, size));
    shader->use();
    shader->uniformValue("uTextureFont", 0);
    shader->uniformValue("uX", x);
    shader->uniformValue("uY", y);
    shader->uniformValue("W", (GLint) width);
    shader->uniformValue("H", (GLint) height);

    glEnable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    geo->enable();
    geo->draw();
    geo->disable();

    glDisable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}



std::string TextRendering::to_string(bool value) {
    if(value) {
        return "true";
    }
    return "false";
}