#ifndef POLYSET_H_
#define POLYSET_H_

#include "grid.h"
#include "linalg.h"
#include <vector>
#include <string>

class PolySet
{
public:
	typedef std::vector<Vector3d> Polygon;
	std::vector<Polygon> polygons;
	std::vector<Polygon> borders;
	Grid3d<void*> grid;

	bool is2d;
	int convexity;

	PolySet();
	~PolySet();

	bool empty() const { return polygons.size() == 0; }
	void append_poly();
	void append_vertex(double x, double y, double z = 0.0);
	void insert_vertex(double x, double y, double z = 0.0);
	size_t memsize() const;
	
	BoundingBox getBoundingBox() const;
	std::string dump() const;
};


#endif
