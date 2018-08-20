#ifndef DEF_HEADER_PLANET
#define DEF_HEADER_PLANET

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "Geometry.h"
#include "GeometryFactory.h"
#include "Render.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "MousePicking.h"

typedef struct PlanetRenderData PlanetRenderData;

struct PlanetRenderData {
    Geometry *shape;
    std::vector<Render *> render;
    Shader *shader;
    GLuint textures;
    glm::vec3 color;

    PlanetRenderData(Geometry *_shape, Shader *_shader) :
            shape(_shape), shader(_shader), textures(0) {}

    ~PlanetRenderData() {
        delete shape;
        std::for_each(render.begin(), render.end(), [](auto *r) { delete r; });
    }
};

class PlanetFactory;

class Planet {
public:
    Planet(Geometry *shape, Shader *shader, double size = 1, double distance = 0, double period = 0,
           double self_period = 0, bool view_sat_ring = true);

    ~Planet();

    void add_texture(const char *path, const char *uniformValue);

    void add_uniformValues(std::function<void(Shader *)> computeUniformValues);

    void add_satellite(Planet *planet);

    void add_atmosphere(float size, Shader *atmo_shader, Geometry *shape);

    std::pair<glm::mat4, bool> getMVMatrix(Planet *goal, bool log_scale = false, glm::mat4 sauv = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0)));

    Planet *intersectRay(Window *window, glm::vec3 &originRay, glm::vec3 &ray, bool log_scale = false,
                         glm::mat4 sauv = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0)));

    void draw(Window *window, Camera *camera, bool anim = true, double speed = 1, bool log_scale = false,
              bool recursive = true,
              glm::mat4 sauv = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -5.0))
              );

    double recursive_size();

    void setBlended(bool _blend);

    double getSize() const;

    void setColor(glm::vec3 color);

    void scale(double size);

    void put_view_rings();

    void addRing();

    friend PlanetFactory;

    static Shader *oneColorShader;
    static Shader *planetShader;
    static Shader *ringShader;
    static Shader *asteroidShader;
    static char **argv;
private:
    PlanetRenderData render_data;
    std::vector<Planet *> satellites;
    std::vector<Planet *> view_rings;
    Planet *ring;
    std::function<void(Shader *)> computeUniformValues;

    float atmosphere_size;
    PlanetRenderData *atmosphere;
    bool view_sat_ring;

    double size;
    double distance;
    double period;
    glm::vec3 color;
    float angle;
    double self_period;
    float self_angle;
    bool blended;
};

#endif
