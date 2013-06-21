#ifndef NODETASKVISITOR_H_
#define NODETASKVISITOR_H_

#include "visitor.h"
#include "node.h"
#include "csgnode.h"
#include "state.h"
#include "transformnode.h"
#include "cgaladvnode.h"
#include "printutils.h"
#include "CGAL_Nef_polyhedron.h"
#include "NodeTask.h"
#include "CGALCache.h"
#include "Tree.h"
#include "CGALEvaluator.h"
#include "cgaladvnode.h"
#include "transformnode.h"
#include "polyset.h"

#include <map>
#include <list>
#include <boost/foreach.hpp>

class NodeTaskVisitor : public Visitor
{
public:

    NodeTaskVisitor(CGALEvaluator cgalevaluator) : cgalevaluator(cgalevaluator) {}
    virtual ~NodeTaskVisitor() {}

    // The visitor interaface
    virtual Response visit(State &state, const AbstractNode &node);
    virtual Response visit(State &state, const AbstractIntersectionNode &node);
    virtual Response visit(State &state, const CsgNode &node);
    virtual Response visit(State &state, const TransformNode &node);
    virtual Response visit(State &state, const AbstractPolyNode &node);
    virtual Response visit(State &state, const CgaladvNode &node);

    // Accessors
    void addTaskResult(const int id, CGAL_Nef_polyhedron poly);
    NodeTask* popTask() { NodeTask* task = tasks.front(); tasks.pop_front(); return task; }
    int getNumberOfTasks() { return tasks.size(); }

private:

    // These will be filled up externally in arbitrary order, by a task processor, rather than as we visit the tree
    typedef std::map<int, CGAL_Nef_polyhedron> TaskResults;
    std::map<int, TaskResults> childNodeTaskResults;

    // This is the list of tasks we've determined can be done
    std::list<NodeTask*> tasks;

    // This is the map of nodes that have already had tasks created for them, plus their parent node id (if there is one)
    typedef std::pair<bool, int> HasParent;
    std::map<int, HasParent> processedNodes;

    // Helper methods
    bool isCached(const AbstractNode &node) const;
    void addTask(NodeTask* task, const AbstractNode *parentNode);
    bool childNodesReady(const AbstractNode &node);
    bool alreadyProcessed(const AbstractNode &node);
    std::list<CGAL_Nef_polyhedron> gatherPolysInRightOrder(const AbstractNode &node);
    CGALEvaluator cgalevaluator;

};

#endif
