#ifndef CGALEVALUATOR_H_
#define CGALEVALUATOR_H_

#include "visitor.h"
#include "CGAL_Nef_polyhedron.h"
#include "PolySetCGALEvaluator.h"
#include "transformnode.h"

#include <string>
#include <map>
#include <list>

class CGALEvaluator : public Visitor
{
public:
	enum CsgOp {CGE_UNION, CGE_INTERSECTION, CGE_DIFFERENCE, CGE_MINKOWSKI};
	CGALEvaluator(const class Tree &tree) : tree(tree), psevaluator(*this) {}
  virtual ~CGALEvaluator() {}

  virtual Response visit(State &state, const AbstractNode &node);
 	virtual Response visit(State &state, const AbstractIntersectionNode &node);
 	virtual Response visit(State &state, const CsgNode &node);
 	virtual Response visit(State &state, const TransformNode &node);
	virtual Response visit(State &state, const AbstractPolyNode &node);
	virtual Response visit(State &state, const CgaladvNode &node);

 	CGAL_Nef_polyhedron evaluateCGALMesh(const AbstractNode &node);
    static CGAL_Nef_polyhedron evaluateCGALMesh(const PolySet &polyset);

	const Tree &getTree() const { return this->tree; }
    static void process(CGAL_Nef_polyhedron &target, const CGAL_Nef_polyhedron &src, CGALEvaluator::CsgOp op);
    static CGAL_Nef_polyhedron transformPoly(CGAL_Nef_polyhedron N, Transform3d matrix);
    static CGAL_Nef_polyhedron applyHull(std::list<CGAL_Nef_polyhedron> polys);

private:
  void addToParent(const State &state, const AbstractNode &node, const CGAL_Nef_polyhedron &N);
  bool isCached(const AbstractNode &node) const;
	CGAL_Nef_polyhedron applyToChildren(const AbstractNode &node, CGALEvaluator::CsgOp op);

	std::string currindent;
  typedef std::pair<const AbstractNode *, CGAL_Nef_polyhedron> ChildItem;
  typedef std::list<ChildItem> ChildList;
	std::map<int, ChildList> visitedchildren;

	const Tree &tree;
	CGAL_Nef_polyhedron root;
public:
	// FIXME: Do we need to make this visible? Used for cache management
 // Note: psevaluator constructor needs this->tree to be initialized first
    PolySetCGALEvaluator psevaluator;
};

#endif
