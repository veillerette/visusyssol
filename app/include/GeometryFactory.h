#ifndef DEF_HEADER_GEOMETRY_FACTORY
#define DEF_HEADER_GEOMETRY_FACTORY

#include "Geometry.h"

class GeometryFactory {
public:
	static Geometry * Geometry_create_test();
	static Geometry * Geometry_create_sphere(int resolution = 16);
	/* inner_size est la distance entre l'origine et le centre du tube et outter_size le rayon du tube */
	static Geometry * Geometry_create_torus(double inner_size = 0.5, double outter_size = 0.5, int arc_resolution = 32, int circle_resolution = 16);
	static Geometry * Geometry_create_geodesic();
	static void Geometry_subdivide(Geometry ** geo, int divs);
	static Geometry * Geometry_create_icosphere(int divs = 3);
	static void compute_normals(Geometry * geo);
	static Geometry * Geometry_create_asteroid(int divs = 3, int potentiels = 100, float deform = 10);
	static Geometry * Geometry_create_box();
	static Geometry * Geometry_create_quad();
	static Geometry * Geometry_create_quad_letter(unsigned int size, const char *msg);
	static Geometry * Geometry_create_skybox();
	static Geometry * Geometry_load_obj(const char * path);
    static Geometry * Geometry_create_ring(double inner, double outter, unsigned int res);
};

#endif