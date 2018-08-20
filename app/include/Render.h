#ifndef DEF_HEADER_RENDER
#define DEF_HEADER_RENDER

#include "Geometry.h"
#include <GL/glew.h>

class Render {
public:
    Render(Geometry *geometry);

    virtual ~Render();

    virtual void render() = 0;

    virtual void postRender() = 0;

    void enable();

    void disable();

private:
    Geometry *geometry; /* pointer on the used geometry */
};

#endif