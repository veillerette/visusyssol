#include "../include/Window.h"
#include "../include/Geometry.h"
#include "../include/Shader.h"
#include "../include/TrackballCamera.h"
#include "../include/TextureRender.h"
#include "../include/Planet.h"
#include "../include/PlanetFactory.h"
#include "../include/FreeflyCamera.hpp"
#include "../include/BloomRender.h"
#include "../include/MousePicking.h"
#include "../include/TextRendering.h"

#include <iostream>

char **Planet::argv;


int main(int argc, const char *argv[]) {
    Window window(argv, 1600, 900, "Visualisation du Sytème Solaire - M2 - Trancho/Veillerette", 60);
    Planet::planetShader = new Shader(argv, "3D.vs.glsl", "directionallight.fs.glsl");
    Planet::asteroidShader = new Shader(argv, "3D_color.vs.glsl", "Colored_directionallight.fs.glsl");
    auto *skyboxShader = new Shader(argv, "3D.vs.glsl", "skybox.fs.glsl");
    Planet::oneColorShader = Shader::oneColorShader(argv, glm::vec3(0.7, 0.7, 0.7));
    Planet::argv = (char **) argv;
    Planet::ringShader = new Shader(argv, "3D.vs.glsl", "ring.fs.glsl");

    Planet sky(
            GeometryFactory::Geometry_create_skybox(),
            skyboxShader,
            100, 0, 0
    );
    sky.add_texture("assets/textures/2k_stars.jpg", "uTexture");

    Planet::planetShader->use();
    Planet::planetShader->uniformValue("uShininess", 0.4f);
    Planet::planetShader->uniformVector("uKd", glm::vec3(0.5075, 0.5075, 0.5075));
    Planet::planetShader->uniformVector("uKs", glm::vec3(0.508273, 0.508273, 0.508273));
    Planet::planetShader->uniformVector("uLightIntensity", glm::vec3(0.1922, 0.1922, 0.1922));

    Planet::asteroidShader->use();
    Planet::asteroidShader->uniformValue("uShininess", 0.4f);
    Planet::asteroidShader->uniformVector("uKd", glm::vec3(0.5075, 0.5075, 0.5075));
    Planet::asteroidShader->uniformVector("uKs", glm::vec3(0.508273, 0.508273, 0.508273));
    Planet::asteroidShader->uniformVector("uLightIntensity", glm::vec3(0.1922, 0.1922, 0.1922));

    Planet *test = PlanetFactory::Planet_load("assets/solar_system/sun.planet");

    double size = test->recursive_size();
    std::cout << "r_size = " << size << std::endl;
    test->scale(10 / test->recursive_size());
    test->put_view_rings();

    TrackballCamera trackball(1.2);
    FreeflyCamera freefly = FreeflyCamera();


    MousePicking mousePicking(&window, &trackball);
    BloomRender bloomRenderer(&window, true, argv, 1); // set 2nd arg to false to desactivate anti-aliasing (MSAA 4x)

    bool do_animation = true;
    bool log_scale = false;
    double speed = 0.2;
    bool selectCamera = false;
    Camera *camera = &trackball;
    bool activateBloom = true;

    Planet *select = test;
    trackball.setSpeed((float) (select->getSize() * 0.7 / test->getSize()));

    TextRendering textRendering(window.getWidth(), window.getHeight(), "assets/textures/Arial.bmp", argv);

    window.run(
            [&](SDL_Event *event) -> bool { /* Actions and events gestion */
                switch (event->type) {
                    case SDL_KEYUP:
                        switch (event->key.keysym.sym) {
                            case SDLK_SPACE:
                                do_animation = !do_animation;
                                break;
                            case SDLK_RIGHT:
                                speed *= 1.5;
                                break;
                            case SDLK_LEFT:
                                speed /= 1.5;
                                break;
                            case SDLK_c:
                                selectCamera = !selectCamera;
                                if (selectCamera) {
                                    freefly = FreeflyCamera(glm::vec3(trackball.getPosition()));
                                    camera = &freefly;
                                } else {
                                    camera = &trackball;
                                }
                                mousePicking.setCamera(camera);
                                break;
                            case SDLK_b:
                                activateBloom = !activateBloom;
                                break;

                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        if (event->button.button == SDL_BUTTON_LEFT) {
                            glm::vec3 ray = mousePicking.rayPicking(event->button.x, event->button.y);
                            glm::vec3 posCamera = glm::vec3(camera->getPosition());
                            Planet *cursor = test->intersectRay(&window, posCamera, ray);
                            if (nullptr != cursor && select != cursor) {
                                select = cursor;
                                selectCamera = false;
                                camera = &trackball;
                                mousePicking.setCamera(camera);
                                trackball.setDistance((float) select->getSize() * 4);
                                trackball.setSpeed((float) (select->getSize() * 0.7 / test->getSize()));
                            }
                        }
                        break;
                }

                if (!selectCamera) {
                    trackball.gestEvent(event);
                } else {
                    freefly.gestEvent(event);
                }

                return true;
            },
            [&]() { /* Drawing stuff */
                trackball.setOrigin(test->getMVMatrix(select).first);
                auto renderScene = [&]() {
                    test->draw(&window, camera, do_animation, speed);
                    sky.draw(&window, camera, false, 0, false, false,
                             glm::translate(glm::mat4(1.0f), glm::vec3(camera->getPosition())));
                };
                if (activateBloom) {
                    bloomRenderer.render(renderScene);
                } else {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glEnable(GL_DEPTH_TEST);
                    renderScene();
                }

                textRendering.drawText("Bloom:" + TextRendering::to_string(activateBloom), 10, 30, 30);
                if(do_animation) {
                    textRendering.drawText("Vitesse:x" + TextRendering::customPrecision(speed * 10), 10, 0, 30);
                } else {
                    textRendering.drawText("Vitesse:pause", 10, 0, 30);
                }
            }
    );
    //delete test;
    return 0;
}
