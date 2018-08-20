#include "../include/MousePicking.h"

MousePicking::MousePicking(Window *_window, Camera *_camera) : window(_window), camera(_camera) {
}

MousePicking::~MousePicking() {
}

void MousePicking::setCamera(Camera *_camera) {
    if (nullptr != _camera) {
        camera = _camera;
    }
}

glm::vec4 MousePicking::homogenousClipCoord(unsigned int x, unsigned int y) {
    float tmpX = (2.f * x) / window->getWidth() - 1.f;
    float tmpY = 1.f - 2.f * y / window->getHeight();
    return glm::vec4(tmpX, tmpY, -1, 1);
}

glm::vec3 MousePicking::rayPicking(unsigned int x, unsigned int y) {
    glm::vec4 clipCoords = homogenousClipCoord(x, y);
    glm::vec4 tmp = glm::vec4(glm::vec2(glm::inverse(window->getProjectionMatrix()) * clipCoords), -1, 0);
    return glm::normalize(glm::vec3(glm::inverse(camera->getViewMatrix()) * tmp));
}

bool MousePicking::rayIntersectSphere(glm::vec3 &sphereCenter, float radius, glm::vec3 &originRay, glm::vec3 ray) {
    glm::vec3 tmp = (originRay - sphereCenter);
    float r = (pow((glm::dot(ray, tmp)), 2))
              - pow(glm::l2Norm(tmp),2) + pow(radius, 2);
    return r >= 0;
}