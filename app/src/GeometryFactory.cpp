#include "../include/GeometryFactory.h"

#include <iostream>
#include <math.h>
#include <functional>

using namespace std;

using glm::vec3;
using glm::vec2;
using glm::mat4;
using glm::vec4;

Geometry * GeometryFactory :: Geometry_create_test() {
	//cout << "test" << endl;
	Geometry * geo = new Geometry(4, 2);
	//cout << "create" << endl;
	geo->vertices[0] = ShapeVertex(vec3(-0.5, -0.5, 0), vec3(1.0, 0.0, 0), vec2(0, 0));
	geo->vertices[1] = ShapeVertex(vec3(-0.5, 0.5, 0), vec3(0.0, 1.0, 0), vec2(0, 1));
    geo->vertices[2] = ShapeVertex(vec3(0.5, 0.5, 0), vec3(0.0, 0.0, 1.0), vec2(1, 1));
	geo->vertices[3] = ShapeVertex(vec3(0.5, -0.5, 0), vec3(0.5, 1.0, 0.2), vec2(1, 0));
	geo->triangles[0] = 0;
	geo->triangles[1] = 1;
	geo->triangles[2] = 2;
    geo->triangles[3] = 2;
    geo->triangles[4] = 3;
    geo->triangles[5] = 0;
	//cout << "triangles done" << endl;

	geo->fillBuffer();
	return geo;
}

static mat4 rotate_z(float alpha) {
	return mat4(
		vec4(cos(alpha), sin(alpha), 0, 0),
		vec4(-sin(alpha), cos(alpha), 0, 0),
		vec4(0, 0, 1, 0),
		vec4(0, 0, 0, 1)
	);
}

static mat4 rotate_y(float alpha) {
	return mat4(
		vec4(cos(alpha), 0, -sin(alpha), 0),
		vec4(0, 1, 0, 0),
		vec4(sin(alpha), 0, cos(alpha), 0),
		vec4(0, 0, 0, 1)
	);
}

static mat4 rotate_x(float alpha) {
	return mat4(
		vec4(1, 0, 0, 0),
		vec4(0, cos(alpha), sin(alpha), 0),
		vec4(0, -sin(alpha), cos(alpha), 0),
		vec4(0, 0, 0, 1)
	);
}

Geometry * GeometryFactory :: Geometry_create_sphere(int resolution) {
	Geometry * geo = new Geometry(
		(2 * resolution + 1) * (resolution + 1), /* poles + others */
		4 * resolution + 4 * resolution * (resolution - 2) /* triangles at top + others */
	);
	
	std::function<int(int, int)> get_id_vertex = [=](int i, int j) -> int {
		return i * (2 * resolution + 1) + j;
	};
	
	double phi = M_PI / resolution;
	double theta = M_PI / resolution;
	vec4 dot(0, 1, 0, 0);
	vec4 current;
	vec3 coords;
	mat4 rot_phi = rotate_z(-phi);
	mat4 rot_theta = rotate_y(theta);
	int offset;
	
	for(int i = 1; i < resolution; ++i) {
		current = dot = rot_phi * dot;
		for(int j = 0; j < 2 * resolution + 1; ++j) {
			current = rot_theta * current;
			coords = vec3(current.x, current.y, current.z);
			geo->vertices[get_id_vertex(i, j)] = ShapeVertex(
				coords, coords, vec2((float)j / (2 * resolution), (float)i / resolution)
			);
		}
	}
	for(int j = 0; j < 2 * resolution + 1; ++j) {
		geo->vertices[get_id_vertex(0, j)] = ShapeVertex(vec3(0, 1, 0), vec3(0, 1, 0), vec2((float)j / (2 * resolution), 0));
		geo->vertices[get_id_vertex(resolution, j)] = ShapeVertex(vec3(0, -1, 0), vec3(0, -1, 0), vec2((float)j / (2 * resolution), 1));
	}
	
	for(int j = 0; j < 2 * resolution; ++j) {
		geo->triangles[3 * j] = get_id_vertex(0, j);
		geo->triangles[3 * j + 1] = get_id_vertex(1, j);
		geo->triangles[3 * j + 2] = get_id_vertex(1, j + 1);
	}
	for(int i = 1; i < resolution - 1; ++i) {
		for(int j = 0; j < 2 * resolution; ++j) {
			offset = 2 * resolution + 2 * ((i - 1) * 2 * resolution + j);
			geo->triangles[3 * offset]
				= get_id_vertex(i, j);
			geo->triangles[3 * offset + 3] = geo->triangles[3 * offset + 1]
				= get_id_vertex(i, j + 1);
			geo->triangles[3 * offset + 4] = geo->triangles[3 * offset + 2]
				= get_id_vertex(i + 1, j);
			geo->triangles[3 * offset + 5]
				= get_id_vertex(i + 1, j + 1);
		}
	}
	for(int j = 0; j < 2 * resolution; ++j) {
		offset = 2 * resolution + 2 * ((resolution - 2) * 2 * resolution) + j;
		geo->triangles[3 * offset] = get_id_vertex(resolution, j);
		geo->triangles[3 * offset + 1] = get_id_vertex(resolution - 1, j);
		geo->triangles[3 * offset + 2] = get_id_vertex(resolution - 1, j + 1);
	}
	
	geo->fillBuffer();
	
	return geo;
}

Geometry * GeometryFactory :: Geometry_create_torus(double inner_size, double outter_size, int arc_resolution, int circle_resolution) {
	Geometry * geo = new Geometry(
		(arc_resolution + 1) * (circle_resolution + 1),
		arc_resolution * circle_resolution * 2
	);
	std::function<int(int, int)> get_id_vertex = [=](int i, int j) -> int {
		return i * (arc_resolution + 1) + j;
	};
	
	double phi = (2 * M_PI) / circle_resolution;
	double theta = (2 * M_PI) / arc_resolution;
	vec4 dot(0, outter_size, 0, 0);
	vec4 current;
	vec3 coord;
	vec3 normal;
	mat4 rot_phi = rotate_z(phi);
	mat4 rot_theta = rotate_y(theta);
	int i, j;
	int offset;
	
	for(i = 0, current = dot; i < circle_resolution + 1; ++i, current = rot_phi * current) {
		coord = vec3(current.x, current.y, current.z);
		geo->vertices[get_id_vertex(i, 0)] = ShapeVertex(
			coord + vec3(inner_size, 0, 0),
			coord,
			vec2((float)0 / arc_resolution, (float)i / circle_resolution)
		);
	}
	for(j = 1; j < arc_resolution + 1; ++j) {
		for(i = 0; i < circle_resolution + 1; ++i) {
			coord = geo->vertices[get_id_vertex(i, j - 1)].position;
			current = vec4(coord.x, coord.y, coord.z, 1) * rot_theta;
			geo->vertices[get_id_vertex(i, j)].position = vec3(current.x, current.y, current.z);
			
			coord = geo->vertices[get_id_vertex(i, j - 1)].normal;
			current = vec4(coord.x, coord.y, coord.z, 0) * rot_theta;
			geo->vertices[get_id_vertex(i, j)].normal = vec3(current.x, current.y, current.z);
			
			geo->vertices[get_id_vertex(i, j)].texCoords = vec2((float)j / arc_resolution, (float)i / circle_resolution);
		}
	}
	
	for(j = 0; j < arc_resolution; ++j) {
		for(i = 0; i < circle_resolution; ++i) {
			offset = i * arc_resolution + j;
			geo->triangles[6 * offset]
				= get_id_vertex(i, j);
			geo->triangles[6 * offset + 3] = geo->triangles[6 * offset + 1]
				= get_id_vertex(i, j + 1);
			geo->triangles[6 * offset + 4] = geo->triangles[6 * offset + 2]
				= get_id_vertex(i + 1, j);
			geo->triangles[6 * offset + 5]
				= get_id_vertex(i + 1, j + 1);
		}
	}
	
	geo->fillBuffer();
	
	return geo;
}

Geometry * GeometryFactory :: Geometry_create_geodesic() {
	Geometry * geo = new Geometry(
		12,
		20
	);
	
	std::function<int(int, int, int)> get_id_vertex = [=](int u, int i, int s) -> int {
		return (u % 3) * 4 + (i & 1) * 2 + (s & 1);
	};
	std::function<vec3(vec3)> shift_right = [=](vec3 v) -> vec3 {
		return vec3(v.z, v.x, v.y);
	};
	int offset;
	
	for(int i = 0; i < 2; ++i)
		for(int s = 0; s < 2; ++s)
			geo->vertices[get_id_vertex(0, i, s)].position = vec3(s ? -2 : 2, i ? -1 : 1, 0);
	for(int u = 1; u < 3; ++u)
		for(int i = 0; i < 2; ++i)
			for(int s = 0; s < 2; ++s)
				geo->vertices[get_id_vertex(u, i, s)].position
					= shift_right(geo->vertices[get_id_vertex(u - 1, i, s)].position);
	for(int u = 0; u < 3; ++u)
		for(int i = 0; i < 2; ++i)
			for(int s = 0; s < 2; ++s) {
				geo->vertices[get_id_vertex(u, i, s)].normal = geo->vertices[get_id_vertex(u, i, s)].position;
				geo->vertices[get_id_vertex(u, i, s)].texCoords = vec2(0.0, 0.0);
			}
	
	for(int i = 0; i < 2; ++i)
		for(int j = 0; j < 2; ++j)
			for(int s = 0; s < 2; ++s) {
				offset = (i * 4 + j * 2 + s) * 3;
				geo->triangles[offset] = get_id_vertex(0, i, s);
				geo->triangles[offset + 1] = get_id_vertex(1, j, i);
				geo->triangles[offset + 2] = get_id_vertex(2, s, j);
			}
	for(int u = 0; u < 3; ++u)
		for(int s = 0; s < 2; ++s)
			for(int i = 0; i < 2; ++i) {
				offset = (8 + u * 4 + s * 2 + i) * 3;
				geo->triangles[offset] = get_id_vertex(u, 1, s);
				geo->triangles[offset + 1] = get_id_vertex(u, 2, s);
				geo->triangles[offset + 2] = get_id_vertex(u + 2, s, i);
			}
	
	geo->fillBuffer();
	
	return geo;
}

typedef struct SawList SawList;
struct SawList {
	int voisin;
	int sub_res;
	SawList * next;
	SawList(int _voisin, int _sub_res, SawList * _next = nullptr) :
		voisin(_voisin), sub_res(_sub_res), next(_next) {}
	~SawList() { delete next; };
};

static int SawList_get(SawList ** tab, int v1, int v2) {
	SawList * l = tab[min(v1, v2)];
	int v = max(v1, v2);
	for(; l; l = l->next)
		if(l->voisin == v)
			return l->sub_res;
	return -1;
}

static void SawList_add(SawList ** tab, int v1, int v2, int sub) {
	int v = min(v1, v2);
	int vs = max(v1, v2);
	if(SawList_get(tab, v, vs) == -1)
		tab[v] = new SawList(vs, sub, tab[v]);
}

static vec3 vec3_mid(vec3 v1, vec3 v2) {
	return vec3((v1.x + v2.x) / 2, (v1.y + v2.y) / 2, (v1.z + v2.z) / 2);
}

static vec2 vec2_mid(vec2 v1, vec2 v2) {
	return vec2((v1.x + v2.x) / 2, (v1.y + v2.y) / 2);
}

static ShapeVertex SV_mid(ShapeVertex & v1, ShapeVertex & v2) {
	return ShapeVertex(vec3_mid(v1.position, v2.position),
		vec3_mid(v1.normal, v2.normal),
		vec2_mid(v1.texCoords, v2.texCoords));
}

void GeometryFactory :: Geometry_subdivide(Geometry ** geo, int divs) {
	if(divs <= 0)
		return;
	Geometry * ggeo = *geo;
	Geometry * res = new Geometry(
		ggeo->size * 4,
		ggeo->faces * 4
	);
	SawList ** see = new SawList * [ggeo->size];
	for(int i = 0; i < ggeo->size; ++i) {
		see[i] = nullptr;
	}
	int v;
	int v1, v2;
	int count = 0;
	int offset;
	for(int i = 0; i < ggeo->size; ++i) {
		res->vertices[i] = ggeo->vertices[i];
		++count;
	}	
	
	for(int i = 0; i < ggeo->faces; ++i) {
		for(int j = 0; j < 3; ++j) {
			v1 = ggeo->triangles[i * 3 + j];
			v2 = ggeo->triangles[i * 3 + ((j + 1) % 3)];
			if((v = SawList_get(see, v1, v2)) == -1) {
				res->vertices[count] = SV_mid(res->vertices[v1], res->vertices[v2]);
				SawList_add(see, v1, v2, count);
				++count;
			}
		}
		for(int j = 0; j < 3; ++j) {
			offset = i * 12 + j * 3;
			res->triangles[offset] = ggeo->triangles[i * 3 + j];
			res->triangles[offset + 1] = SawList_get(see,
				ggeo->triangles[i * 3 + j], ggeo->triangles[i * 3 + ((j + 1) % 3)]);
			res->triangles[offset + 2] = SawList_get(see,
				ggeo->triangles[i * 3 + j], ggeo->triangles[i * 3 + ((j + 2) % 3)]);
		}
		offset = i * 12 + 9;
		res->triangles[offset] = SawList_get(see,
			ggeo->triangles[i * 3], ggeo->triangles[i * 3 + 1]);
		res->triangles[offset + 1] = SawList_get(see,
			ggeo->triangles[i * 3 + 1], ggeo->triangles[i * 3 + 2]);
		res->triangles[offset + 2] = SawList_get(see,
			ggeo->triangles[i * 3 + 2], ggeo->triangles[i * 3]);
	}
	res->size = count;
	
	delete ggeo;
	delete [] see;
	*geo = res;
	
	(*geo)->fillBuffer();
	
	Geometry_subdivide(geo, divs - 1);
}

static vec3 vec3_renormalize(vec3 v) {
	float n = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return vec3(v.x / n, v.y / n, v.z / n);
}

Geometry * GeometryFactory :: Geometry_create_icosphere(int divs) {
	Geometry * geo = Geometry_create_geodesic();
	Geometry_subdivide(&geo, 1);
	/* manage texture here ? */
	Geometry_subdivide(&geo, divs - 1);
	for(int i = 0; i < geo->size; ++i) {
		geo->vertices[i].position = vec3_renormalize(geo->vertices[i].position);
		geo->vertices[i].normal = vec3_renormalize(geo->vertices[i].normal);
	}
	geo->fillBuffer();
	return geo;
}

typedef struct Potentiel Potentiel;
struct Potentiel {
	vec3 coord;
	float power;
	Potentiel() : Potentiel(vec3(0, 0, 0), 0) {}
	Potentiel(vec3 _coord, float _power) : coord(_coord), power(_power) {}
};

static float rand_float(float a, float b, int div = 1000) {
	return (((rand() % div) * (b - a)) / div) + a;
}

static Potentiel rand_Potentiel(float power_min = -1, float power_max = 1) {
	float r = rand_float(0.25, 1.5);
	float theta = rand_float(0, 2 * M_PI);
	float phi = rand_float(0, 2 * M_PI);
	float power = rand_float(power_min, power_max);
	return Potentiel(vec3(r * cos(theta) * cos(phi), r * cos(theta) * sin(phi), r * sin(theta)), power);
}

static inline float pow2(float val) {
	return val * val;
}

static inline float det(vec2 a, vec2 b) {
	return a.x * b.y - a.y * b.x;
}

static inline vec3 cross(vec3 a, vec3 b) {
	return vec3(
		det(vec2(a.y, a.z), vec2(b.y, b.z)),
		-det(vec2(a.x, a.z), vec2(b.x, b.z)),
		det(vec2(a.x, a.y), vec2(b.x, b.y))
	);
}

static inline float dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float det(vec3 a, vec3 b, vec3 c) {
	vec3 d = cross(b, c);
	return dot(a, d);
}

static inline float sign(float val) {
	return val < 0 ? -1 : 1;
}

static inline float norm(vec3 v) {
	return sqrt(dot(v, v));
}

static inline vec3 normalize(vec3 v) {
	float n = norm(v);
	if(n < 1e-9)
		return vec3(0, 0, 0);
	return vec3(v.x / n, v.y / n, v.z / n);
}

typedef struct VertexSum VertexSum;
struct VertexSum {
	vec3 v;
	int count;
	VertexSum() : v(vec3(0, 0, 0)), count(0) {}
	void add(vec3 a) {
		v = v + a;
		++count;
	};
	vec3 compute() {return count ? normalize(v) : vec3(0, 0, 0);}
};

void GeometryFactory :: compute_normals(Geometry * geo) {
	VertexSum * vs = new VertexSum[geo->size];
	for(int i = 0; i < geo->faces; ++i) {
		float s = sign(det(
			geo->vertices[geo->triangles[3 * i + 0]].position,
			geo->vertices[geo->triangles[3 * i + 1]].position,
			geo->vertices[geo->triangles[3 * i + 2]].position
		));
		for(int j = 0; j < 3; ++j) {
			vec3 v = normalize(cross(
				geo->vertices[geo->triangles[3 * i + ((j + 1) % 3)]].position - geo->vertices[geo->triangles[3 * i + j]].position,
				geo->vertices[geo->triangles[3 * i + ((j + 2) % 3)]].position - geo->vertices[geo->triangles[3 * i + j]].position
			));
			//cout << "v(" << i * 3 + j << ") = " << v.x << ", " << v.y << ", " << v.z << endl;
			v = vec3(v.x * s, v.y * s, v.z * s);
			vs[geo->triangles[3 * i + j]].add(v);
		}
	}
	for(int i = 0; i < geo->size; ++i) {
		geo->vertices[i].normal = vs[i].compute();
		
	}
	delete [] vs;
};

static vec3 vec3_from_potentiels(vec3 coord, Potentiel * pot, int nb, float deform) {
	//cout << "in : " << coord.x << ", " << coord.y << ", " << coord.z << endl;
	float d;
	vec3 tmp = vec3(0, 0, 0);
	for(int i = 0; i < nb; ++i) {
		d = 1 / (deform + pow2(pot[i].coord.x - coord.x) + pow2(pot[i].coord.y - coord.y) + pow2(pot[i].coord.z - coord.z));
		tmp = tmp + vec3(deform * pot[i].power * d * (pot[i].coord.x - coord.x),
			deform * pot[i].power * d * (pot[i].coord.y - coord.y),
			deform * pot[i].power * d * (pot[i].coord.z - coord.z));
	}
	coord = coord + tmp;
	//cout << "out : " << coord.x << ", " << coord.y << ", " << coord.z << endl;
	return coord;
}

static vec3 compute_color_from_potentiels(vec3 coord, Potentiel * pot, int nb, float deform) {
	float d;
	vec3 tmp = vec3(0, 0, 0);
	for(int i = 0; i < nb; ++i) {
		d = 1 / (deform + pow2(pot[i].coord.x - coord.x) + pow2(pot[i].coord.y - coord.y) + pow2(pot[i].coord.z - coord.z));
		tmp = tmp + vec3(deform * pot[i].power * d * (pot[i].coord.x - coord.x),
			deform * pot[i].power * d * (pot[i].coord.y - coord.y),
			deform * pot[i].power * d * (pot[i].coord.z - coord.z));
	}
	coord = tmp;
	coord = vec3(coord.x * sign(coord.x), coord.y * sign(coord.y), coord.z * sign(coord.z));
	return coord;
}

static inline vec3 max(vec3 a, vec3 b) {
	return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

static inline vec3 min(vec3 a, vec3 b) {
	return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}

static inline vec3 color_adjust(vec3 color, vec3 mini, vec3 maxi) {
	color = vec3((color.x - mini.x) / (maxi.x - mini.x), (color.y - mini.y) / (maxi.y - mini.y), (color.z - mini.z) / (maxi.z - mini.z));
	float av = max(color.x, max(color.y, color.z));
	//cout << "res : " << av << endl;
	return vec3(av, av, av);
}

static inline vec3 vec3_div(vec3 v, float s) {
	if(s == 0)
		return vec3(0, 0, 0);
	return vec3(v.x / s, v.y / s, v.z / s);
}

Geometry * GeometryFactory :: Geometry_create_asteroid(int divs, int potentiels, float deform) {
	Geometry * model = Geometry_create_icosphere(divs);
	Potentiel * pot = new Potentiel[potentiels];
	deform /= potentiels;
	float max_norm = 0;
	
	for(int i = 0; i < potentiels; ++i)
		pot[i] = rand_Potentiel();
	for(int i = 0; i < model->size; ++i) {
		model->vertices[i].position = vec3_from_potentiels(model->vertices[i].position, pot, potentiels, deform);
		max_norm = max(norm(model->vertices[i].position), max_norm);
	}
	for(int i = 0; i < model->size; ++i) {
		model->vertices[i].position = vec3_div(model->vertices[i].position, max_norm);
	}
	compute_normals(model);
	
	ColorGeometry * geo = new ColorGeometry(model);
	vec3 mini, maxi;
	for(int i = 0; i < model->size; ++i) {
		geo->vertices[i].color = compute_color_from_potentiels(model->vertices[i].position, pot, potentiels, deform);
		//cout << "color " << geo->vertices[i].color.x << ", " << geo->vertices[i].color.y << ", " << geo->vertices[i].color.z << endl;
		if(i == 0) {
			mini = geo->vertices[i].color;
			maxi = geo->vertices[i].color;
		} else {
			mini = min(mini, geo->vertices[i].color);
			maxi = max(maxi, geo->vertices[i].color);
		}
	}
	for(int i = 0; i < model->size; ++i) {
		geo->vertices[i].color = color_adjust(geo->vertices[i].color, mini, maxi);
	}
	geo->fillBuffer();
	delete [] pot;
	delete model;
	return geo;
}

#include <glimac/tiny_obj_loader.h>

Geometry * GeometryFactory :: Geometry_load_obj(const char * path) {
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string str = LoadObj(shapes, materials, path);
	if(str.length() != 0) {
		std::cerr << "Unable to load " << path << " : " << str <<  std::endl;
		exit(EXIT_FAILURE);
	}

	unsigned int n = (unsigned int)shapes[0].mesh.positions.size()/3;
	Geometry *geo = new Geometry(n, (unsigned int)shapes[0].mesh.indices.size()/3);

	tinyobj::mesh_t mesh = shapes[0].mesh;
	
	
    float m = 999;
    float M = -500;
    
    for(auto elem:mesh.positions) {
    	if(elem < m) {
    		m = elem;
    	} else if(elem > M) {
    		M = elem;
    	}
    }
    
    float bias = std::max(abs(m), abs(M));
	for(unsigned int i = 0; i < n; ++i) {
		geo->vertices[i] = ShapeVertex(
				vec3(mesh.positions[3*i]/bias, mesh.positions[3*i+1]/bias, mesh.positions[3*i+2]/bias),
				vec3(-mesh.normals[3*i], -mesh.normals[3*i+1], -mesh.normals[3*i+2]),
				vec2(0, 0));
	}

	for(unsigned int i = 0; i < mesh.indices.size(); ++i) {
		geo->triangles[i] = mesh.indices[i];
	}
	geo->fillBuffer();

	return geo;
}

Geometry* GeometryFactory::Geometry_create_box() {
    auto * geo = new Geometry(12, 12);
	geo->vertices[0] = ShapeVertex(vec3(-0.5,-0.5,0.5), vec3(0,0,0), vec2(0.33,0.25));
	geo->vertices[1] = ShapeVertex(vec3(0.5,-0.5,0.5), vec3(0,0,0), vec2(0.66,0.25));
	geo->vertices[2] = ShapeVertex(vec3(-0.5,0.5,0.5), vec3(0,0,0), vec2(0.33,0.5));
	geo->vertices[3] = ShapeVertex(vec3(0.5,0.5,0.5), vec3(0,0,0), vec2(0.66,0.5));

	geo->vertices[4] = ShapeVertex(vec3(-0.5,-0.5,-0.5), vec3(0,0,0), vec2(0.33,1));
	geo->vertices[5] = ShapeVertex(vec3(0.5,-0.5,-0.5), vec3(0,0,0), vec2(0.66,1));
	geo->vertices[6] = ShapeVertex(vec3(-0.5,0.5,-0.5), vec3(0,0,0), vec2(0.33,0.75));
	geo->vertices[7] = ShapeVertex(vec3(0.5,0.5,-0.5), vec3(0,0,0), vec2(0.66,0.75));

	geo->vertices[8] = ShapeVertex(vec3(0.5,0.5,-0.5), vec3(0,0,0), vec2(1,0.5));
	geo->vertices[9] = ShapeVertex(vec3(0.5,-0.5,-0.5), vec3(0,0,0), vec2(1,0.25));
	geo->vertices[10] = ShapeVertex(vec3(-0.5,0.5,-0.5), vec3(0,0,0), vec2(0,0.25));
	geo->vertices[11] = ShapeVertex(vec3(-0.5,-0.5,-0.5), vec3(0,0,0), vec2(0,0.5));

	unsigned int faces[36] = {
			0, 1, 2, 1, 2, 3,
			2, 3, 6, 3, 6, 7,
			3, 8, 1, 1, 8, 9,
			0, 11, 2, 2, 11, 10,
			6, 7, 4, 7, 4, 5,
			0, 1, 4, 4, 1, 5
	};
	memcpy(geo->triangles, faces, sizeof(unsigned int) * 36);

    geo->fillBuffer();

    return geo;
}

Geometry * GeometryFactory::Geometry_create_skybox() {
	Geometry * geo = new Geometry(14, 12);
	
	vec4 dot(0.5, 0.5, 0.5, 0);
	vec4 current;
	int i;
	
	for(i = 0, current = dot; i < 5; ++i, current = rotate_z(M_PI / 2) * current)
		geo->vertices[i] = ShapeVertex(vec3(current.x, current.y, current.z), vec3(0,0,0), vec2((float)i / 4, 1. / 3));
	for(i = 0, current = rotate_y(M_PI / 2) * dot; i < 5; ++i, current = rotate_z(M_PI / 2) * current)
		geo->vertices[5 + i] = ShapeVertex(vec3(current.x, current.y, current.z), vec3(0,0,0), vec2((float)i / 4, 2. / 3));
	for(i = 0, current = dot; i < 2; ++i, current = rotate_z(-M_PI / 2) * current)
		geo->vertices[10 + i] = ShapeVertex(vec3(current.x, current.y, current.z), vec3(0,0,0), vec2((float)(i + 1) / 4, 0));
	for(i = 0, current = rotate_y(M_PI / 2) * dot; i < 2; ++i, current = rotate_z(-M_PI / 2) * current)
		geo->vertices[12 + i] = ShapeVertex(vec3(current.x, current.y, current.z), vec3(0,0,0), vec2((float)(i + 1) / 4, 1));
	
	for(i = 0; i < 4; ++i) {
		geo->triangles[6 * i] = i;
		geo->triangles[6 * i + 4] = geo->triangles[6 * i + 1] = i + 1;
		geo->triangles[6 * i + 3] = geo->triangles[6 * i + 2] = i + 5;
		geo->triangles[6 * i + 5] = i + 6;
	}
	geo->triangles[6 * 4] = 1;
	geo->triangles[24 + 4] = geo->triangles[24 + 1] = 2;
	geo->triangles[24 + 3] = geo->triangles[24 + 2] = 10;
	geo->triangles[24 + 5] = 11;
	
	geo->triangles[6 * 5] = 6;
	geo->triangles[30 + 4] = geo->triangles[30 + 1] = 7;
	geo->triangles[30 + 3] = geo->triangles[30 + 2] = 12;
	geo->triangles[30 + 5] = 13;
	
	geo->fillBuffer();
	
	return geo;
}

Geometry* GeometryFactory::Geometry_create_quad() {
	Geometry *geo = new Geometry(4, 2);
    geo->vertices[0] = ShapeVertex(vec3(-1, -1, 0), vec3(0,0,0), vec2(0, 0));
	geo->vertices[1] = ShapeVertex(vec3(-1, 1,0), vec3(0,0,0), vec2(0, 1));
	geo->vertices[2] = ShapeVertex(vec3(1, -1, 0), vec3(0,0,0), vec2(1, 0));
	geo->vertices[3] = ShapeVertex(vec3(1, 1, 0), vec3(0,0,0), vec2(1, 1));

	unsigned int faces[6] = {
			0, 1, 2, 3, 2, 1
	};
	memcpy(geo->triangles, faces, sizeof(unsigned int) * 6);

	geo->fillBuffer();

	return geo;
}

Geometry * GeometryFactory::Geometry_create_quad_letter(unsigned int size, const char *msg) {
    unsigned int n = strlen(msg);
	Geometry *geo = new Geometry(4*n, 2*n);
    unsigned int esp = (unsigned int)(size * 0.5);

	int first[6] = {
			0
	};

	for(int i = 0; i < n; i++) {
		char c = msg[i];
		float ux = (c%16)/16.f;
		float uy = (c/16)/16.f;
		geo->vertices[4*i] = ShapeVertex(vec3(i*esp, size, 0), vec3(0, 0, 0), vec2(ux, uy));
		geo->vertices[4*i+1] = ShapeVertex(vec3(i*esp, 0, 0), vec3(0, 0, 0), vec2(ux, (uy+1.f/16)));
		geo->vertices[4*i+2] = ShapeVertex(vec3(i*esp+esp, size, 0), vec3(0, 0, 0), vec2(ux+1.f/32.f, uy));
		geo->vertices[4*i+3] = ShapeVertex(vec3(i*esp+esp, 0, 0), vec3(0, 0, 0), vec2(ux+1.f/32.f, (uy+1.f/16.f)));

		first[0] = 4*i;
		first[1] = 4*i+1;
		first[2] = 4*i+2;
		first[3] = 4*i+1;
		first[4] = 4*i+2;
		first[5] = 4*i+3;

		memcpy(geo->triangles+6*i, first, sizeof(unsigned int) * 6);
	}

	geo->fillBuffer();

	return geo;
}

Geometry *GeometryFactory::Geometry_create_ring(double inner, double outter, unsigned int res) {
	Geometry *geo = new Geometry(res*2, res*2);
	double phi = 2 * M_PI / (res/2);

	for(int i = 0; i < res; i++) {
		geo->vertices[2*i] = ShapeVertex(vec3((outter) * cos(phi * i), (outter) * sin(phi * i), 0), vec3(1,0,0), vec2(0.0, phi * i / (2*M_PI)));
		geo->vertices[2*i+1] = ShapeVertex(vec3((inner) * cos(phi * i), (inner) * sin(phi * i), 0), vec3(1,0,0), vec2(1.0, phi * i / (2*M_PI)));
	}

	for(unsigned int i = 0; i < res; i++) {
		geo->triangles[6*i] = 2*i;
		geo->triangles[6*i+1] = (2*i+1)%(2*res);
		geo->triangles[6*i+2] = (2*i+3)%(2*res);
		geo->triangles[6*i+3] = 2*i;
		geo->triangles[6*i+4] = (2*i+3)%(2*res);
		geo->triangles[6*i+5] = (2*i+2)%(2*res);
	}

	geo->fillBuffer();

	return geo;
}
