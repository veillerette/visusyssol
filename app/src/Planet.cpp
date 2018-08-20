#include "../include/Planet.h"

#include "../include/TextureRender.h"

#include <glimac/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>

using namespace std;


Shader *Planet::oneColorShader;
Shader *Planet::planetShader;
Shader *Planet::ringShader;
Shader *Planet::asteroidShader;

Planet::Planet(Geometry *shape, Shader *shader, double _size, double _distance, double _period, double _self_period, bool _view_sat_ring) :
        render_data(shape, shader), size(_size), distance(_distance), period(_period), view_sat_ring(_view_sat_ring),
        computeUniformValues([](Shader *s) {}),
        atmosphere_size(-1), atmosphere(nullptr), angle(0), self_period(_self_period), self_angle(0),
        blended(false),
        ring(nullptr),
        color(glm::vec3(-1,-1,-1)) {
}

Planet::~Planet() {
    delete atmosphere;
    std::for_each(satellites.begin(), satellites.end(), [](auto *s) { delete s; });
}

void Planet::add_texture(const char *path, const char *uniformValue) {
    TextureRender *render = new TextureRender(render_data.shape, GL_TEXTURE0 + (render_data.textures++), uniformValue,
                                              render_data.shader);
    render->load(path);
    render_data.render.push_back(render);
}

void Planet::add_satellite(Planet *planet) {
    satellites.push_back(planet);
}

void Planet::setBlended(bool _blend) {
    blended = _blend;
}

void Planet::setColor(glm::vec3 color) {
    this->color = color;
}

void Planet::put_view_rings() {
	if(view_sat_ring) {
		std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
		    s->put_view_rings();
		    auto *traj_sat = new Planet(
		            GeometryFactory::Geometry_create_torus(s->distance, s->size / 20, 100),
		            Planet::oneColorShader,
		            1, 0, 0.0
		    );
		    view_rings.push_back(traj_sat);
		});
    }
}

void Planet::add_uniformValues(std::function<void(Shader *)> _computeUniformValues) {
    computeUniformValues = _computeUniformValues;
}

void Planet::add_atmosphere(float size, Shader *_atmo_shader, Geometry *shape) {
    atmosphere_size = size;
    atmosphere = new PlanetRenderData(shape, _atmo_shader);
}

static glm::mat4 rotate_y(float alpha) {
    return glm::mat4(
            glm::vec4(cos(alpha), 0, -sin(alpha), 0),
            glm::vec4(0, 1, 0, 0),
            glm::vec4(sin(alpha), 0, cos(alpha), 0),
            glm::vec4(0, 0, 0, 1)
    );
}

pair<glm::mat4, bool> Planet::getMVMatrix(Planet *goal, bool log_scale, glm::mat4 sauv) {
    std::function<double(double)> ajust = log_scale ? [](double v) { return log(v); } : [](double v) { return v; };

    if (this == goal) {
        return pair<glm::mat4, bool>(glm::translate(
                sauv * rotate_y(angle),
                glm::vec3(ajust(distance), 0, 0)), true);
    }
    bool r = false;
    glm::mat4 res = sauv;
    std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
        auto tmp = s->getMVMatrix(goal, log_scale,
                                  glm::translate(
                                          sauv * rotate_y(angle),
                                          glm::vec3(ajust(distance), 0, 0)));
        if (tmp.second && !r) {
            res = tmp.first;
            r = true;
            return;
        }
    });

    return pair<glm::mat4, bool>(res, r);
}

Planet *Planet::intersectRay(Window *window, glm::vec3 &originRay, glm::vec3 &ray, bool log_scale, glm::mat4 sauv) {
    std::function<double(double)> ajust = log_scale ? [](double v) { return log(v); } : [](double v) { return v; };
    glm::vec3 sphereCenter = glm::vec3(glm::translate(
            sauv * rotate_y(angle),
            glm::vec3(ajust(distance), 0, 0)) * rotate_y(self_angle) * glm::vec4(0, 0, 0, 1));
    float r = (float) size;

    if (MousePicking::rayIntersectSphere(sphereCenter, r, originRay, ray)) {
        return this;
    }

    Planet *res = nullptr;

    std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
        if (res != nullptr) {
            return;
        }
        res = s->intersectRay(window, originRay, ray, log_scale,
                              glm::translate(
                                      sauv * rotate_y(angle),
                                      glm::vec3(ajust(distance), 0, 0)));
    });

    return res;
}

void
Planet::draw(Window *window, Camera *camera, bool anim, double speed, bool log_scale, bool recursive, glm::mat4 sauv) {
    std::function<double(double)> ajust = log_scale ? [](double v) { return log(v); } : [](double v) { return v; };
    window->handlerTab();

    glm::mat4 ProjMatrix = window->getProjectionMatrix(), MVMatrix, ViewMatrix, NormalMatrix, MVPMatrix;
    ViewMatrix = camera->getViewMatrix();
    MVMatrix = sauv;

    MVMatrix = glm::scale(
            glm::translate(
                    sauv * rotate_y(angle),
                    glm::vec3(ajust(distance), 0, 0)),
            glm::vec3(ajust(size), ajust(size), ajust(size))) * rotate_y(self_angle);
    MVMatrix = ViewMatrix * MVMatrix;
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
    MVPMatrix = ProjMatrix * MVMatrix;

    if (!blended) {
        glDisable(GL_BLEND);
    } else {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    render_data.shader->use();
    //render_data.shader->uniformVector("uLightDir_vs", glm::vec3(ViewMatrix * glm::vec4(-glm::vec3(camera->getPosition(glm::vec4(0,0,0,1))), 0)));
    render_data.shader->uniformVector("uLightPos_vs", glm::vec3(ViewMatrix * glm::vec4(0, 0, -5.0, 1.0)));

    // if it's a colored planet instead of textured planet.
    render_data.shader->uniformVector("uKa", color);
    render_data.shader->uniformMatrix("uMVPMatrix", MVPMatrix);
    render_data.shader->uniformMatrix("uMVMatrix", MVMatrix);
    render_data.shader->uniformMatrix("uNormalMatrix", NormalMatrix);

    render_data.shader->uniformValue("uTime", (GLint) SDL_GetTicks());

    for (Render *&render : render_data.render)
        render->render();
    computeUniformValues(render_data.shader);

    render_data.shape->draw();

    for (Render *&render : render_data.render)
        render->postRender();

    if (atmosphere) {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        MVMatrix = glm::scale(
                        glm::translate(
                                sauv * rotate_y(angle),
                                glm::vec3(ajust(distance), 0, 0)),
                glm::vec3(ajust(size*atmosphere_size), ajust(size*atmosphere_size), ajust(size*atmosphere_size)));
        MVMatrix = ViewMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        MVPMatrix = ProjMatrix * MVMatrix;
        atmosphere->shader->use();
        atmosphere->shader->uniformVector("uLightDir_vs", glm::vec3(ViewMatrix * glm::vec4(0.0, 0.0, -5.0, 1.0)));
        atmosphere->shader->uniformVector("uColor", atmosphere->color);
        atmosphere->shader->uniformMatrix("uMVPMatrix", MVPMatrix);
        atmosphere->shader->uniformMatrix("uMVMatrix", MVMatrix);
        atmosphere->shader->uniformMatrix("uNormalMatrix", NormalMatrix);
        atmosphere->shape->draw();
    }

    if(ring) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        MVMatrix = glm::scale(
                glm::rotate(
                glm::translate(
                        sauv * rotate_y(angle),
                        glm::vec3(ajust(distance), 0, 0)), (float)(ring->angle*M_PI), glm::vec3(1,0,0)),
                glm::vec3(ajust(size*2.1), ajust(size*2.1), ajust(size*1.8)));
        MVMatrix = ViewMatrix * MVMatrix;
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        MVPMatrix = ProjMatrix * MVMatrix;
        ring->render_data.shader->use();
        ring->render_data.shader->uniformVector("uLightPos_vs", glm::vec3(ViewMatrix * glm::vec4(0, 0, -5.0, 1.0)));
        ring->render_data.shader->uniformMatrix("uMVPMatrix", MVPMatrix);
        ring->render_data.shader->uniformMatrix("uMVMatrix", MVMatrix);
        ring->render_data.shader->uniformMatrix("uNormalMatrix", NormalMatrix);
        for (Render *&render : ring->render_data.render)
            render->render();
        computeUniformValues(ring->render_data.shader);
        ring->render_data.shape->draw();

        for (Render *&render : ring->render_data.render)
            render->postRender();

        glDisable(GL_BLEND);
    }



    if (recursive) {
        std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
            s->draw(window, camera, anim, speed, log_scale, true,
                    glm::translate(
                            sauv * rotate_y(angle),
                            glm::vec3(ajust(distance), 0, 0)));
        });
        std::for_each(view_rings.begin(), view_rings.end(), [&](auto *s) {
            s->draw(window, camera, anim, speed, log_scale, false, glm::translate(
                    sauv * rotate_y(angle),
                    glm::vec3(ajust(distance), 0, 0)));
        });
    }

    if (anim) {
        angle += period * speed;
        self_angle += self_period * speed;
    }
}

double Planet::recursive_size() {
    double r_size = size + distance;
    std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
        r_size = std::max(r_size, s->recursive_size());
    });
    return r_size;
}

void Planet::scale(double sc) {
    //std::cout << "before : size = " << size << ", distance = " << distance << std::endl;
    size *= sc;
    distance *= sc;
    //std::cout << "size = " << size << ", distance = " << distance << std::endl;
    std::for_each(satellites.begin(), satellites.end(), [&](auto *s) {
        s->scale(sc);
    });
}

void Planet::addRing() {
    if (ring == nullptr) {
        ring = new Planet(
                GeometryFactory::Geometry_create_ring(0.6, 1.0, 230),
                Planet::ringShader,
                size*1.3, 0, 0
        );
    }
}

double Planet::getSize() const {
    return size;
}

