#ifndef DEF_HEADER_GEOMETRY
#define DEF_HEADER_GEOMETRY

#include "Buffer.h"

class GeometryFactory;
class ColorGeometry;

class Geometry {
public:
	Geometry(unsigned int size, unsigned int faces);
	~Geometry();
	void draw();
	void enable();
	void disable();
	virtual void fillBuffer();

	/* GeometryFactory */
	/* ces méthodes sont en charge de créer les géométries */

	friend GeometryFactory;
	friend ColorGeometry;
protected:
	ShapeVertex * vertices;
	unsigned int size;
	unsigned int * triangles;
	unsigned int faces;
	Buffer buffer;
};

class ColorGeometry : public Geometry {
public:
	ColorGeometry(unsigned int size, unsigned int faces);
	ColorGeometry(Geometry * geo);
	~ColorGeometry();
	void fillBuffer() override;

	friend GeometryFactory;
private:
	ColorShapeVertex * vertices;
	unsigned int size;
};

#endif
