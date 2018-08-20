#include "../include/PlanetFactory.h"

#include "../include/TextureRender.h"

#include <glimac/glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>

using namespace std;

static float rand_float(float a, float b, int div = 1000) {
	if(a == b)
		return a;
	return (((rand() % div) * (b - a)) / div) + a;
}

Planet * PlanetFactory :: Asteroid_ring_create(float in_radius, float radius, float period,
    	int asteroid_count, float min_asteroid_size, float max_asteroid_size,
    	float min_rotation, float max_rotation,
    	int reel_gen, int resolution, int potentiels, float deform) {

	Planet * origin = new Planet(GeometryFactory::Geometry_create_sphere(60), Planet::planetShader, 0, 0, period, 0, false);
	
	Planet * ast = nullptr;
	
	Geometry ** geo = new Geometry * [reel_gen];
	
	for(int i = 0; i < reel_gen; ++i)
		geo[i] = GeometryFactory::Geometry_create_asteroid(resolution, potentiels, deform);
	
	for(int i = 0; i < asteroid_count; ++i) {
		ast = new Planet(
			geo[rand() % reel_gen],
			Planet::asteroidShader,
			rand_float(min_asteroid_size, max_asteroid_size),
			rand_float(in_radius, radius),
			0,
			rand_float(min_rotation, max_rotation)
		);
		ast->angle = rand_float(0, 2 * M_PI);
		origin->add_satellite(ast);
	}
	
	delete[] geo;
	
	return origin;
}

typedef struct FieldAction FieldAction;

struct FieldAction {
    string field;
    function<void(ifstream &)> action;

    FieldAction(string f, function<void(ifstream &)> a) : action(a), field(f) {}
};

static void FieldAction_reader(vector<FieldAction> &f, ifstream &reader,
                               string closing_field = "end",
                               function<bool(ifstream &, string)> default_action = [](ifstream &a, string b) {
                                   cout << "read : " << b << endl;
                                   return false;
                               } /* ignore it */
) {
    string field;
    bool did_action = false;
    do {
        reader >> field;
        did_action = false;
        for (auto it = f.begin(); it != f.end(); ++it) {
            if (field == it->field) {
                it->action(reader);
                did_action = true;
                break;
            }
        }
        if (!did_action)
            if (!default_action(reader, field))
                break;
    } while (field.size() > 0 && field != closing_field);
}

static void FieldAction_reader_switch(vector<FieldAction> &f, ifstream &reader) {
    string field;
    reader >> field;
    for (auto it = f.begin(); it != f.end(); ++it) {
        if (field == it->field) {
            it->action(reader);
            break;
        }
    }
}

void PlanetFactory::Planet_reader_planet(Planet *planet, ifstream &reader) {
    vector<FieldAction> actions;

    /* developper data */
    /*actions.push_back(FieldAction("size", [&](ifstream & reader) {
        reader >> planet->size;
    }));
    actions.push_back(FieldAction("distance", [&](ifstream & reader) {
        reader >> planet->distance;
    }));
    actions.push_back(FieldAction("period", [&](ifstream & reader) {
        reader >> planet->period;
    }));
    actions.push_back(FieldAction("self-period", [&](ifstream & reader) {
        reader >> planet->self_period;
    }));*/

    /* user data */
    actions.push_back(FieldAction("diameter", [&](ifstream &reader) {
        double diameter;
        reader >> diameter;
        diameter *= 30; /* ajust */
        planet->size = diameter / 2;
    }));

    actions.push_back(FieldAction("parent-distance", [&](ifstream &reader) {
        double distance;
        reader >> distance;
        distance *= 3. / 100;
        planet->distance = distance;
    }));

    actions.push_back(FieldAction("orbital-period", [&](ifstream &reader) {
        double days;
        reader >> days;
        planet->period = (2 * M_PI) / (days * 24);
    }));

    actions.push_back(FieldAction("rotation-period", [&](ifstream &reader) {
        double hours;
        reader >> hours;
        planet->self_period = (2 * M_PI) / (hours);
    }));

    actions.push_back(FieldAction("texture", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->add_texture(path.data(), "uTexture_day");
        planet->setColor(glm::vec3(-1,-1,-1));
    }));

    actions.push_back(FieldAction("texture-day", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->add_texture(path.data(), "uTexture_day");
        planet->setColor(glm::vec3(-1,-1,-1));
    }));

    actions.push_back(FieldAction("texture-night", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->add_texture(path.data(), "uTexture_night");
    }));

    actions.push_back(FieldAction("clouds", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->add_texture(path.data(), "uTexture_clouds");
        planet->add_uniformValues([](Shader *shader) {
            shader->uniformValue("timer", (float) (SDL_GetTicks() / -150000.0));
        });
    }));

    actions.push_back(FieldAction("ring", [&](ifstream &reader) {
        float ring_size;
        reader >> ring_size;
        ring_size *= 15;
        planet->addRing();
        planet->ring->size = planet->size * 2;
    }));

    actions.push_back(FieldAction("color", [&](ifstream &reader) {
        float r,g,b;
        reader >> r;
        reader >> g;
        reader >> b;
        planet->setColor(glm::vec3(r,g,b));
    }));

    actions.push_back(FieldAction("ringtexture", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->addRing();
        planet->ring->add_texture(path.data(), "uTexture");
    }));

    actions.push_back(FieldAction("ringalpha", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet->addRing();
        planet->ring->add_texture(path.data(), "uTextureAlpha");
    }));

    actions.push_back(FieldAction("ringangle", [&](ifstream &reader) {
        float ringAngle;
        reader >> ringAngle;
        planet->addRing();
        planet->ring->angle = ringAngle;
    }));

    actions.push_back(FieldAction("atmosphere", [&](ifstream &reader) {
        float at_size;
        reader >> at_size;
        planet->add_atmosphere(at_size, new Shader((const char **) Planet::argv, "3D.vs.glsl", "atmo.fs.glsl"),
                               GeometryFactory::Geometry_create_icosphere(3));
    }));

    actions.push_back(FieldAction("atmosphereColor", [&](ifstream &reader) {
        float r,g,b;
        reader >> r;
        reader >> g;
        reader >> b;
        planet->atmosphere->color = glm::vec3(r,g,b);
    }));

    actions.push_back(FieldAction("satellite", [&](ifstream &reader) {
        planet->add_satellite(PlanetFactory::Planet_reader(reader));
    }));

    FieldAction_reader(actions, reader);
}

Planet * PlanetFactory::Asteroid_ring_reader(std::ifstream &reader) {
	vector<FieldAction> actions;
	
	float in_radius = 0;
	float radius = 1;
	float period = 0;
	float asteroid_count = 100;
	float min_asteroid_size = 0;
	float max_asteroid_size = 1;
	float min_rotation = 0;
	float max_rotation = 1;
	int reel_gen = 5;
	int resolution = 3;
	int potentiels = 200;
	float deform = 10;
	
	actions.push_back(FieldAction("diameter", [&](ifstream &reader) {
        reader >> in_radius;
        reader >> radius;
        in_radius *= 3. / 100;
        radius *= 3. / 100;
        in_radius /= 2;
        radius /= 2;
    }));
	actions.push_back(FieldAction("orbital-period", [&](ifstream &reader) {
        double days;
        reader >> days;
        period = (2 * M_PI) / (days * 24);
    }));
    actions.push_back(FieldAction("asteroid-diameter", [&](ifstream &reader) {
        reader >> min_asteroid_size;
        reader >> max_asteroid_size;
        min_asteroid_size *= 30;
        max_asteroid_size *= 30;
        min_asteroid_size /= 2;
        max_asteroid_size /= 2;
    }));
    actions.push_back(FieldAction("asteroid-count", [&](ifstream &reader) {
        reader >> asteroid_count;
    }));
    actions.push_back(FieldAction("asteroid-period", [&](ifstream &reader) {
        reader >> min_rotation;
        min_rotation = (2 * M_PI) / (min_rotation);
        reader >> max_rotation;
        max_rotation = (2 * M_PI) / (max_rotation);
    }));
    actions.push_back(FieldAction("resolution", [&](ifstream &reader) {
        reader >> resolution;
    }));
    actions.push_back(FieldAction("cabossages", [&](ifstream &reader) {
        reader >> potentiels;
    }));
    actions.push_back(FieldAction("deformation", [&](ifstream &reader) {
        reader >> deform;
    }));
	
	FieldAction_reader(actions, reader);
	
	reel_gen = 3 * max(1, (int)(log(asteroid_count) / log(2)));
	
	return Asteroid_ring_create(in_radius, radius, period,
		asteroid_count, min_asteroid_size, max_asteroid_size,
		min_rotation, max_rotation,
		reel_gen, resolution, potentiels, deform);
}

Planet *PlanetFactory::Planet_reader_create(ifstream &reader) {
    vector<FieldAction> actions;
    Planet *planet = nullptr;
    actions.push_back(FieldAction("Planet", [&](ifstream &reader) {
        planet = new Planet(
                GeometryFactory::Geometry_create_sphere(60),
                Planet::planetShader
        );
        Planet_reader_planet(planet, reader);
    }));
    actions.push_back(FieldAction("Sun", [&](ifstream &reader) {
        planet = new Planet(
                GeometryFactory::Geometry_create_icosphere(3),
                new Shader((const char **) Planet::argv, "3D.vs.glsl", "sun.fs.glsl")
        );
        Planet_reader_planet(planet, reader);
        planet->size /= 30;
    }));
    actions.push_back(FieldAction("Asteroid-ring", [&](ifstream &reader) {
        planet = Asteroid_ring_reader(reader);
    }));
    actions.push_back(FieldAction("Obj", [&](ifstream &reader) {
        string path;
        reader >> path;
        planet = new Planet(
                GeometryFactory::Geometry_load_obj(path.c_str()),
                Planet::planetShader
        );
        Planet_reader_planet(planet, reader);
    }));
    FieldAction_reader_switch(actions, reader);
    return planet;
}

Planet *PlanetFactory::Planet_reader(ifstream &reader) {
    vector<FieldAction> actions;
    Planet *planet = nullptr;
    actions.push_back(FieldAction("create", [&](ifstream &reader) {
        planet = Planet_reader_create(reader);
    }));
    actions.push_back(FieldAction("load", [&](ifstream &reader) {
        string path;
        reader >> path;
        //std::cout << "load planet " << path << std::endl;
        planet = Planet_load(path.data());
    }));
    FieldAction_reader_switch(actions, reader);
    return planet;
}

Planet *PlanetFactory::Planet_load(const char *path) {
    ifstream reader;
    Planet *planet = nullptr;

    reader.open(path, ios::in);
    //cout << "BEGIN READ" << endl;
    planet = Planet_reader(reader);
    //cout << "END READ" << endl;
    reader.close();
    return planet;
}
