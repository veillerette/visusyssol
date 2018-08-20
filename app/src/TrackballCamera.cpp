#include "../include/TrackballCamera.h"

TrackballCamera::TrackballCamera(float distance, float speed, float angleX, float angleY)
        : _distance(distance), _speed(speed), _angleX(angleX), _angleY(angleY), mouseDone(false) {
}

void TrackballCamera::moveFront(float delta) {
    _distance -= delta * _speed;
}

void TrackballCamera::rotateLeft(float degrees) {
    _angleY -= degrees;
}

void TrackballCamera::rotateUp(float degrees) {
    _angleX -= degrees;
}

void TrackballCamera::setOrigin(glm::mat4 _origin) {
    origin = _origin;
}

void TrackballCamera::setDistance(float distance) {
    _distance = distance;
}

void TrackballCamera::setSpeed(float speed) {
    if (speed != 0) {
        _speed = speed;
    }
}

glm::mat4 TrackballCamera::getViewMatrix() const {
    glm::mat4 res(1.0f);
    res = glm::translate(res, glm::vec3(0.0, 0.0, -_distance));
    res = glm::rotate(res, _angleX, glm::vec3(1.0, 0.0, 0.0));
    res = glm::rotate(res, _angleY, glm::vec3(0.0, 1.0, 0.0));

    return res * (glm::inverse(origin));
}

glm::vec4 TrackballCamera::getPosition() const {
    return glm::inverse(getViewMatrix()) * glm::vec4(0, 0, 0, 1);
}

void TrackballCamera::gestEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN :
            mouseDone = true;
            if (event->button.button == SDL_BUTTON_WHEELUP) {
                moveFront(0.1f);
            } else if (event->button.button == SDL_BUTTON_WHEELDOWN) {
                moveFront(-0.1f);
            }
            break;

        case SDL_MOUSEBUTTONUP :
            mouseDone = false;
            break;

        case SDL_MOUSEMOTION :
            if (mouseDone) {
                rotateLeft(-event->motion.xrel / 400.0f);
                rotateUp(-event->motion.yrel / 400.0f);
            }
            break;
    }
}

float TrackballCamera::get_angleX() const {
    return _angleX;
}

float TrackballCamera::get_angleY() const {
    return _angleY;
}
