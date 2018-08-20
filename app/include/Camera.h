#ifndef DEF_HEADER_CAMERA
#define DEF_HEADER_CAMERA

#include <glimac/glm.hpp>
#include <SDL/SDL.h>

class Camera {
public:
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::vec4 getPosition() const = 0;
	virtual void gestEvent(SDL_Event *event) = 0;
};

#endif