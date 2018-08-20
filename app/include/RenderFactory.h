#ifndef DEF_HEADER_RENDER_FACTORY
#define DEF_HEADER_RENDER_FACTORY

#include "Render.h"
#include "Geometry.h"

#include <glimac/glm.hpp>

class TextureRender : public Render {
public:
	TextureRender(Geometry * geo, const char * texture_path);
private:
	virtual void do_render(Geometry * geometry);
	
	GLuint texture;
};

class AtomosphereRender : public Render {
public:
	AtomosphereRender(Geometry * geo, float opacity = 0.5, vec3 color = vec3(1., 1., 1.), double size = 0.1);
private:
	virtual void do_render(Geometry * geometry);
	
	float opacity;
	vec3 color;
	double size;
};

class StarRender : public Render {
public:
	StarRender(Geometry * geo, double intensity = 1., vec3 color = vec3(1., 1., 1.));
private:
	virtual void do_render(Geometry * geometry);
	
	double intensity;
	vec3 color;
};

#endif