#include "NodeTaskVisitor.h"

Response NodeTaskVisitor::visit(State &state, const AbstractNode &node)
{
    PRINTB("Visiting node %d: AbstractNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        NodeTask* nodeTask = new CGALTask(node.index(), CGALEvaluator::CGE_UNION, gatherPolysInRightOrder(node));
        addTask(nodeTask, state.parent());
    }
    return ContinueTraversal;
}

Response NodeTaskVisitor::visit(State &state, const AbstractIntersectionNode &node)
{
    PRINTB("Visiting node %d: AbstractIntersectionNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        NodeTask* nodeTask = new CGALTask(node.index(), CGALEvaluator::CGE_INTERSECTION, gatherPolysInRightOrder(node));
        addTask(nodeTask, state.parent());
    }
    return ContinueTraversal;
}

Response NodeTaskVisitor::visit(State &state, const CsgNode &node)
{
    PRINTB("Visiting node %d: CsgNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        CGALEvaluator::CsgOp op;
        switch (node.type) {
        case CSG_TYPE_UNION:
            op = CGALEvaluator::CGE_UNION;
            break;
        case CSG_TYPE_DIFFERENCE:
            op = CGALEvaluator::CGE_DIFFERENCE;
            break;
        case CSG_TYPE_INTERSECTION:
            op = CGALEvaluator::CGE_INTERSECTION;
            break;
        default:
            assert(false);
        }
        NodeTask* nodeTask = new CGALTask(node.index(), op, gatherPolysInRightOrder(node));
        addTask(nodeTask, state.parent());
    }
    return ContinueTraversal;
}

Response NodeTaskVisitor::visit(State &state, const CgaladvNode &node)
{
    PRINTB("Visiting node %d: CgaladvNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        CGALEvaluator::CsgOp op;
        NodeTask *nodeTask;
        switch (node.type) {
            case MINKOWSKI:
                nodeTask = new CGALTask(node.index(), CGALEvaluator::CGE_MINKOWSKI, gatherPolysInRightOrder(node));
                break;
            case GLIDE:
                nodeTask = new NullTask(node.index());
                break;
            case SUBDIV:
                nodeTask = new NullTask(node.index());
                break;
            case HULL:
            nodeTask = new HullTask(node.index(), gatherPolysInRightOrder(node));
                break;
            addTask(nodeTask, state.parent());
        }
    }
    return ContinueTraversal;
}

Response NodeTaskVisitor::visit(State &state, const TransformNode &node)
{
    PRINTB("Visiting node %d: TransformNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        NodeTask* nodeTask = new TransformTask(node.index(), node.matrix, gatherPolysInRightOrder(node));
        addTask(nodeTask, state.parent());
    }
    return ContinueTraversal;
}

// Nodes of this type can be evaluated straight away
Response NodeTaskVisitor::visit(State &state, const AbstractPolyNode &node)
{
    PRINTB("Visiting node %d: AbstractPolyNode:%s", node.index() % node.toString());
    if (alreadyProcessed(node)) return PruneTraversal;
    if (childNodesReady(node)) {
        shared_ptr<PolySet> polyset = cgalevaluator.psevaluator.getPolySet(node, false);
        PolySet *polySetCopy = new PolySet(*polyset);
        NodeTask* nodeTask = new PolySetTask(node.index(), polySetCopy);
        addTask(nodeTask, state.parent());
    }
    return ContinueTraversal;
}

// Helper methods
void NodeTaskVisitor::addTask(NodeTask* task, const AbstractNode * parentNode)
{
    // Add the task
    tasks.push_back(task);

    // Mark this node as processed (along with its parent for later result processing)
    bool hasParent = parentNode != NULL;
    int parentId = hasParent ? parentNode->index() : -1;
    processedNodes[task->getId()] = std::make_pair(hasParent, parentId);
}

void NodeTaskVisitor::addTaskResult(const int id, CGAL_Nef_polyhedron poly)
{
    if (processedNodes[id].first){
        childNodeTaskResults[processedNodes[id].second][id] = poly;
    }
}

bool NodeTaskVisitor::childNodesReady(const AbstractNode &node)
{
    return node.children.size() == childNodeTaskResults[node.index()].size();
}

bool NodeTaskVisitor::alreadyProcessed(const AbstractNode &node)
{
    return processedNodes.count(node.index()) > 0;
}

// Gather the child results in the order in the parent, rather than the arbitary order they may have been calculated
std::list<CGAL_Nef_polyhedron> NodeTaskVisitor::gatherPolysInRightOrder(const AbstractNode &node)
{
    std::list<CGAL_Nef_polyhedron> polys;
    BOOST_FOREACH(AbstractNode *childNode, node.getChildren()) {
        polys.push_back(childNodeTaskResults[node.index()][childNode->index()]);
    }
    return polys;
}

