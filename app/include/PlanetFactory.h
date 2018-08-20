#ifndef DEF_HEADER_PLANET_FACTORY
#define DEF_HEADER_PLANET_FACTORY

#include "Planet.h"

class PlanetFactory {
public:
	static Planet * Asteroid_ring_create(float in_radius, float radius, float period,
    	int asteroid_count, float min_asteroid_size, float max_asteroid_size,
    	float min_rotation = 0, float max_rotation = 0,
    	int reel_gen = 3, int resolution = 3, int potentiels = 200, float deform = 10);
    	
    static Planet * Planet_load(const char *path);

private:
	static Planet * Asteroid_ring_reader(std::ifstream &reader);
	
    static Planet * Planet_reader(std::ifstream &reader);

    static Planet * Planet_reader_create(std::ifstream &reader);
    
    static void Planet_reader_planet(Planet *planet, std::ifstream &reader);
};

#endif