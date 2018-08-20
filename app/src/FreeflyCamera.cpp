#include "../include/FreeflyCamera.hpp"

FreeflyCamera::FreeflyCamera(glm::vec3 position, float theta, float phi)
        : _position(position), _theta(theta), _phi(phi), mouseDone(false), keys() {
    computeVectors();
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY / 10, SDL_DEFAULT_REPEAT_INTERVAL);
    keys[SDLK_z] = false;
    keys[SDLK_s] = false;
    keys[SDLK_q] = false;
    keys[SDLK_d] = false;
}

glm::vec3 FreeflyCamera::getPosition() {
    return _position;
}

void FreeflyCamera::moveLeft(float t) {
    _position += t * L;
    computeVectors();
}

void FreeflyCamera::moveFront(float t) {
    _position += t * F;
    computeVectors();
}

void FreeflyCamera::lookLeft(float deg) {
    _phi += glm::radians(deg);
    computeVectors();
}

void FreeflyCamera::lookUp(float deg) {
    _theta += glm::radians(deg);
    computeVectors();
}

glm::mat4 FreeflyCamera::getViewMatrix() const {
    return glm::lookAt(_position, _position + F, U);
}

glm::vec4 FreeflyCamera::getPosition() const {
    return glm::vec4(_position.x, _position.y, _position.z, 1);
}

void FreeflyCamera::setSpeed(float speed) const {
    ;
}

void FreeflyCamera::setOrigin(glm::mat4 _origin) {
    (glm::mat4) _origin; // do nothing for this camera
}

void FreeflyCamera::computeVectors() {
    F = glm::vec3(
            glm::cos(_theta) * glm::sin(_phi),
            glm::sin(_theta),
            glm::cos(_theta) * glm::cos(_phi));
    L = glm::vec3(
            glm::sin(_phi + glm::pi<float>() / 2),
            0.0,
            glm::cos(_phi + glm::pi<float>() / 2));
    U = glm::cross(F, L);
}

void FreeflyCamera::gestKey(SDLKey key, std::function<void(SDLKey)> fct) {
    if (keys.find(key) != keys.cend()) {
        fct(key);
    }
}

void FreeflyCamera::gestEvent(SDL_Event *event) {
    switch (event->type) {
        case SDL_KEYUP:
            gestKey(event->key.keysym.sym, [&](auto k) {
                keys[k] = false;
            });
            break;
        case SDL_KEYDOWN:
            gestKey(event->key.keysym.sym, [&](auto k) {
                keys[k] = true;
            });
            break;

        case SDL_MOUSEBUTTONUP :
            mouseDone = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseDone = true;
            break;

        case SDL_MOUSEMOTION :
            if (mouseDone) {
                lookLeft(-event->motion.xrel / 10.0f);
                lookUp(-event->motion.yrel / 10.0f);
            }
            break;
    }

    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP) {
        return;
    }

    std::for_each(keys.cbegin(), keys.cend(), [&](auto pair) {
        if (pair.second) {
            switch (pair.first) {
                case SDLK_z:
                    this->moveFront(0.1f);
                    break;
                case SDLK_s:
                    this->moveFront(-0.1f);
                    break;
                case SDLK_q:
                    this->moveLeft(0.1f);
                    break;
                case SDLK_d:
                    this->moveLeft(-0.1f);
                    break;
            }
        }
    });
}

float FreeflyCamera::get_theta() const {
    return _theta;
}

float FreeflyCamera::get_phi() const {
    return _phi;
}
