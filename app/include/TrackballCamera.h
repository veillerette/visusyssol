#ifndef DEF_HEADER_TRACKBALL
#define DEF_HEADER_TRACKBALL


#include <glimac/glm.hpp>
#include <SDL/SDL.h>

#include "Camera.h"

class TrackballCamera : public Camera {
private:
    float _distance;
    float _angleX;
    float _angleY;
    float _speed;

    bool mouseDone;
    glm::mat4 origin;

public:

    TrackballCamera(float distance, float speed = 1.f, float angleX = glm::pi<float>() / 10, float angleY = 0);

    void moveFront(float delta);

    void rotateLeft(float degrees);

    float get_angleX() const;

    float get_angleY() const;

    void rotateUp(float degrees);

    void setOrigin(glm::mat4 _origin);

    void setDistance(float distance);

    void setSpeed(float speed);

    glm::mat4 getViewMatrix() const override;

    glm::vec4 getPosition() const override;

    void gestEvent(SDL_Event *event) override;
};

#endif