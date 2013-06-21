#include "NodeTask.h"

// For the purposes of debugging
CGAL_Nef_polyhedron CGALTask::process()
{
    PRINTB("Processing CGALTask:%s", getId());

    CGAL_Nef_polyhedron result;
    BOOST_FOREACH(CGAL_Nef_polyhedron poly, polys) {
        // Initialize N on first iteration with first expected geometric object
        if (result.isNull() && !result.isEmpty()) {
            result = poly.copy();
        }
        else {
            CGALEvaluator::process(result, poly, op);
        }
    }
    return result;
}

CGAL_Nef_polyhedron TransformTask::process()
{
    PRINTB("Processing TransformTask:%s", getId());
    CGAL_Nef_polyhedron result;
    BOOST_FOREACH(CGAL_Nef_polyhedron poly, polys) {
        // Initialize N on first iteration with first expected geometric object
        if (result.isNull() && !result.isEmpty()) {
            result = poly.copy();
        }
        else {
            CGALEvaluator::process(result, poly, CGALEvaluator::CGE_UNION);
        }
    }
    return CGALEvaluator::transformPoly(result, matrix);
}


// This one is going to need a lot of refactoring of PolysetEval, as it's all mixed up
CGAL_Nef_polyhedron PolySetTask::process()
{
    PRINTB("Processing AbstractPolyTask:%s", getId());
    return CGALEvaluator::evaluateCGALMesh(*polySet);
}

CGAL_Nef_polyhedron HullTask::process()
{
    PRINTB("Processing HullTask:%s", getId());
    return CGALEvaluator::applyHull(polys);
}

// Some operations aren't supported
CGAL_Nef_polyhedron NullTask::process()
{
    PRINTB("Processing NullTask:%s", getId());
    CGAL_Nef_polyhedron result;
    return result;
}
