#ifndef DEF_HEADER_SCENE
#define DEF_HEADER_SCENE

#include "Camera.h"

class Scene {
public:
	Scene();
	~Scene();
	friend Scene Scene_load(const char * root_planet_path);
private:
	Camera camera;
	Planet * root;
};

#endif