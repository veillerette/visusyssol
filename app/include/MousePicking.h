#ifndef DEF_HEADER_MOUSEPICKING_H
#define DEF_HEADER_MOUSEPICKING_H

#include "Window.h"
#include "Camera.h"
#include "Planet.h"

class MousePicking {
public:
    MousePicking(Window *_window, Camera *_camera);

    ~MousePicking();

    glm::vec3 rayPicking(unsigned int x, unsigned int y);

    void setCamera(Camera *_camera);

    static bool rayIntersectSphere(glm::vec3 &sphereCenter, float radius, glm::vec3 &originRay, glm::vec3 ray);

private:
    Window *window;
    Camera *camera;

    glm::vec4 homogenousClipCoord(unsigned int x, unsigned int y);
};

#endif //DEF_HEADER_MOUSEPICKING_H
