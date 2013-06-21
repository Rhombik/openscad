#ifndef NODETASK_H
#define NODETASK_H

#include "CGAL_Nef_polyhedron.h"
#include "printutils.h"
#include "node.h"
#include "CGALEvaluator.h"
#include <boost/foreach.hpp>
#include <string>
#include "transformnode.h"
#include "polyset.h"

class NodeTask {
public :

    NodeTask(const int id) : id(id) {}
    virtual ~NodeTask() {}

    const int getId() {return id;}

    virtual CGAL_Nef_polyhedron process() = 0;

private :

    const int id;

};

class CGALTask : public NodeTask {
public:

    CGALTask(const int id, CGALEvaluator::CsgOp op, std::list<CGAL_Nef_polyhedron> polys) :
        NodeTask(id) { this->op = op; this->polys = polys; }
    virtual ~CGALTask() {}

    virtual CGAL_Nef_polyhedron process();
    CGALEvaluator::CsgOp getOp() {return op; }
    std::list<CGAL_Nef_polyhedron> getPolys() { return polys ;}

private:

    CGALEvaluator::CsgOp op;
    std::list<CGAL_Nef_polyhedron> polys;

};

class HullTask : public NodeTask {
public:

    HullTask(const int id, std::list<CGAL_Nef_polyhedron> polys) : NodeTask(id) { this->polys = polys;}
    virtual ~HullTask() {}

    virtual CGAL_Nef_polyhedron process();
    std::list<CGAL_Nef_polyhedron> getPolys() { return polys ;}

private:

    std::list<CGAL_Nef_polyhedron> polys;

};

class TransformTask : public NodeTask {
public:

    TransformTask(const int id, Transform3d matrix, std::list<CGAL_Nef_polyhedron> polys) :
        NodeTask(id) { this->matrix = matrix; this->polys = polys; }
    virtual ~TransformTask() {}

    virtual CGAL_Nef_polyhedron process();
    std::list<CGAL_Nef_polyhedron> getPolys() { return polys; }
    Transform3d getMatrix() { return matrix; }

private:

    Transform3d matrix;
    std::list<CGAL_Nef_polyhedron> polys;

};

class PolySetTask : public NodeTask {
public:

    PolySetTask(const int id, PolySet *polySet) : NodeTask(id) { this->polySet = polySet;}
    virtual ~PolySetTask() {}

    virtual CGAL_Nef_polyhedron process();
    PolySet * getPolySet() { return polySet ;}

private:

    PolySet *polySet;

};

class NullTask : public NodeTask {
public:

    NullTask(const int id) : NodeTask(id) {}
    virtual ~NullTask() {}

    virtual CGAL_Nef_polyhedron process();

private:

};

#endif // NODETASK_H
