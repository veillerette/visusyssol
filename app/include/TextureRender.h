#ifndef DEF_HEADER_TEXTURERENDER
#define DEF_HEADER_TEXTURERENDER

#include "Render.h"
#include <glimac/Image.hpp>
#include <string>
#include "Shader.h"

class TextureRender : public Render {
public:
    TextureRender(Geometry *_geometry, GLuint dest, const char *uniformValue, Shader *shader);

    ~TextureRender() override;

    bool load(std::string path);

    void render() override;

    void postRender() override;

    static bool loadImageTexture(GLuint dest, const char *path);

private:
    GLuint texture;
    GLuint dest;
    char *uniformValue;
    Shader *shader;
};


#endif
