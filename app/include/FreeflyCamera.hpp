#ifndef DEF_HEADER_FREEFLY
#define DEF_HEADER_FREEFLY

#include <glimac/glm.hpp>
#include <unordered_map>
#include <algorithm>
#include <functional>

#include "Camera.h"

class FreeflyCamera : public Camera {
public:
	FreeflyCamera(glm::vec3 position = glm::vec3(0, 2, -0.1), float theta = -glm::pi<float>()/8, float phi = glm::pi<float>());

	glm::vec3 getPosition();

	void moveLeft(float t);

	void moveFront(float t);

	float get_theta() const;

	float get_phi() const;

	void lookLeft(float deg);

	void lookUp(float deg);

	glm::mat4 getViewMatrix() const override;
	glm::vec4 getPosition() const override;
    void gestEvent(SDL_Event *event) override;
	void setOrigin(glm::mat4 _origin);
	void setSpeed(float speed) const;

private:
	glm::vec3 _position;
	float _theta;
	float _phi;

	glm::vec3 F;
	glm::vec3 L;
	glm::vec3 U;

	bool mouseDone;
	std::unordered_map<SDLKey, bool> keys;
    void gestKey(SDLKey key, std::function<void(SDLKey)> fct);

	void computeVectors();
};


#endif