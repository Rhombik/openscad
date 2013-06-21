#include "NodeTaskSerialisation.h"

NodeTaskProto::NodeTaskMsg NodeTaskSerialisation::serialise(NodeTask *nodeTask)
{
    NodeTaskProto::NodeTaskMsg nodeTaskMsg;
    nodeTaskMsg.set_id(nodeTask->getId());

    // Since we don't have double dispatch, and because I don't want to add a serialise method to the NodeTask class,
    // we have to check dynamically here
    CGALTask *cgalTask = dynamic_cast<CGALTask*>(nodeTask);
    if(cgalTask != 0) {
        PRINT("Serialising a CGALTask");
        nodeTaskMsg.set_type(NodeTaskProto::NodeTaskMsg_NodeTaskType_CGALTask);
        NodeTaskProto::CGALTaskDetails *cgalTaskDetails = nodeTaskMsg.mutable_cgaltaskdetails();
        cgalTaskDetails->set_op(serialiseOp(cgalTask->getOp()));
        serialisePolyhedrons(cgalTask->getPolys(), cgalTaskDetails->mutable_polyhedrons());
    }

    TransformTask *transformTask = dynamic_cast<TransformTask*>(nodeTask);
    if(transformTask != 0) {
        PRINT("Serialising a TransformTask");
        nodeTaskMsg.set_type(NodeTaskProto::NodeTaskMsg_NodeTaskType_TransformTask);
        NodeTaskProto::TransformTaskDetails *transformTaskDetails = nodeTaskMsg.mutable_transformtaskdetails();
        serialiseMatrix(transformTask->getMatrix(), transformTaskDetails->mutable_matrix());
        serialisePolyhedrons(transformTask->getPolys(), transformTaskDetails->mutable_polyhedrons());
    }

    PolySetTask *polySetTask = dynamic_cast<PolySetTask*>(nodeTask);
    if(polySetTask != 0) {
        PRINT("Serialising a PolySetTask");
        nodeTaskMsg.set_type(NodeTaskProto::NodeTaskMsg_NodeTaskType_PolySetTask);
        NodeTaskProto::PolySetTaskDetails *polySetTaskDetails = nodeTaskMsg.mutable_polysettaskdetails();
        serialisePolySet(polySetTask->getPolySet(), polySetTaskDetails->mutable_polyset());
    }

    HullTask *hullTask = dynamic_cast<HullTask*>(nodeTask);
    if(hullTask != 0) {
        PRINT("Serialising a HullTask");
        nodeTaskMsg.set_type(NodeTaskProto::NodeTaskMsg_NodeTaskType_HullTask);
        NodeTaskProto::HullTaskDetails *hullTaskDetails = nodeTaskMsg.mutable_hulltaskdetails();
        serialisePolyhedrons(hullTask->getPolys(), hullTaskDetails->mutable_polyhedrons());
    }

    return nodeTaskMsg;
}

NodeTask * NodeTaskSerialisation::deserialise(NodeTaskProto::NodeTaskMsg nodeTaskProto)
{
    int id = nodeTaskProto.id();

    switch(nodeTaskProto.type())
    {
        case NodeTaskProto::NodeTaskMsg::CGALTask:
        {
            PRINT("Deserialising a CGALTask");
            CGALEvaluator::CsgOp op = deserialiseOp(nodeTaskProto.cgaltaskdetails().op());
            std::list<CGAL_Nef_polyhedron> polys = deserialisePolyhedrons(nodeTaskProto.cgaltaskdetails().polyhedrons());
            return new CGALTask(id, op, polys);
        }
        case NodeTaskProto::NodeTaskMsg::TransformTask:
        {
            PRINT("Deserialising a TransformTask");
            Transform3d matrix = deserialiseMatrix(nodeTaskProto.transformtaskdetails().matrix());
            std::list<CGAL_Nef_polyhedron> polys = deserialisePolyhedrons(nodeTaskProto.transformtaskdetails().polyhedrons());
            return new TransformTask(id, matrix, polys);
        }
        case NodeTaskProto::NodeTaskMsg::PolySetTask:
        {
            PRINT("Deserialising a PolySetTask");
            PolySet *polySet = deserialisePolySet(nodeTaskProto.polysettaskdetails().polyset());
            return new PolySetTask(id, polySet);
        }
        case NodeTaskProto::NodeTaskMsg::HullTask:
        {
            PRINT("Deserialising a HullTask");
            std::list<CGAL_Nef_polyhedron> polys = deserialisePolyhedrons(nodeTaskProto.hulltaskdetails().polyhedrons());
            return new HullTask(id, polys);
        }
        default:
            PRINT("Unknown message type - serialising to NullTask");
            return new NullTask(id);
    }

}

NodeTaskProto::CGALTaskDetails_OpType NodeTaskSerialisation::serialiseOp(CGALEvaluator::CsgOp op)
{
    switch(op) {
        case CGALEvaluator::CGE_DIFFERENCE:
            return NodeTaskProto::CGALTaskDetails_OpType_CGE_DIFFERENCE;

        case CGALEvaluator::CGE_INTERSECTION:
            return NodeTaskProto::CGALTaskDetails_OpType_CGE_INTERSECTION;

        case CGALEvaluator::CGE_UNION:
            return NodeTaskProto::CGALTaskDetails_OpType_CGE_UNION;

        case CGALEvaluator::CGE_MINKOWSKI:
            return NodeTaskProto::CGALTaskDetails_OpType_CGE_MINKOWSKI;
        default:
            return NodeTaskProto::CGALTaskDetails_OpType_CGE_UNION;
    }
}

CGALEvaluator::CsgOp NodeTaskSerialisation::deserialiseOp(NodeTaskProto::CGALTaskDetails_OpType protoOp)
{
    switch(protoOp) {
        case NodeTaskProto::CGALTaskDetails_OpType_CGE_DIFFERENCE:
            return CGALEvaluator::CGE_DIFFERENCE;

        case NodeTaskProto::CGALTaskDetails_OpType_CGE_INTERSECTION:
            return CGALEvaluator::CGE_INTERSECTION;

        case NodeTaskProto::CGALTaskDetails_OpType_CGE_UNION:
            return CGALEvaluator::CGE_UNION;

        case NodeTaskProto::CGALTaskDetails_OpType_CGE_MINKOWSKI:
            return CGALEvaluator::CGE_MINKOWSKI;

        default:
            return CGALEvaluator::CGE_UNION;
    }
}

void NodeTaskSerialisation::serialiseMatrix(Transform3d matrix, NodeTaskProto::Matrix *protoMatrix)
{
    protoMatrix->set_rows(matrix.matrix().rows());
    protoMatrix->set_cols(matrix.matrix().cols());

    for (int i=0;i<matrix.matrix().rows();i++) {
      for (int j=0;j<matrix.matrix().cols();j++) {
        protoMatrix->add_data(matrix(i,j));
      }
    }
}

Transform3d NodeTaskSerialisation::deserialiseMatrix(NodeTaskProto::Matrix protoMatrix)
{
    Transform3d matrix;
    int rows = protoMatrix.rows();
    int cols = protoMatrix.cols();

    for (int i=0;i<rows;i++) {
      for (int j=0;j<cols;j++) {
        matrix(i,j) = protoMatrix.data(rows*i+j);
      }
    }
    return matrix;
}

void NodeTaskSerialisation::serialisePolyhedrons(std::list<CGAL_Nef_polyhedron> polyhedrons, ProtoPolys *protoPolyhedrons)
{

    BOOST_FOREACH(CGAL_Nef_polyhedron polyhedron, polyhedrons) {
        std::stringstream polyhedronStream;
        NodeTaskProto::Polyhedron *protoPolyhedron = protoPolyhedrons->Add();

        if(polyhedron.isNull() || polyhedron.isEmpty()) {
            continue;
        }
        if(polyhedron.dim == 2) {
            protoPolyhedron->set_is2d(true);
            polyhedronStream << *polyhedron.p2;
        }
        else {
            protoPolyhedron->set_is2d(false);
            polyhedronStream << *polyhedron.p3;
        }

        protoPolyhedron->set_stream(polyhedronStream.str());
    }

}

std::list<CGAL_Nef_polyhedron> NodeTaskSerialisation::deserialisePolyhedrons(ProtoPolys protoPolys)
{
    std::list<CGAL_Nef_polyhedron> polys;

    BOOST_FOREACH(NodeTaskProto::Polyhedron poly, protoPolys) {
        std::stringstream polyStream(poly.stream());
        if(poly.is2d()) {
            CGAL_Nef_polyhedron2 *poly2 = new CGAL_Nef_polyhedron2();
            polyStream >> *poly2;
            polys.push_back(CGAL_Nef_polyhedron(poly2));

        }
        else {
            CGAL_Nef_polyhedron3 *poly3 = new CGAL_Nef_polyhedron3();
            polyStream >> *poly3;
            polys.push_back(CGAL_Nef_polyhedron(poly3));
        }
    }

    return polys;
}

void NodeTaskSerialisation::serialisePolySet(PolySet *polySet, NodeTaskProto::PolySet *protoPolySet)
{
    protoPolySet->set_is2d(polySet->is2d);
    protoPolySet->set_convexivity(polySet->convexity);
    // TODO Possibly need to look into double rounding issues here
    protoPolySet->set_resolution(polySet->grid.res);

    // Do the polys
    for (int i = 0; i < polySet->polygons.size(); i++) {
        NodeTaskProto::Polygon *protoPolygon = protoPolySet->add_polygons();
        for (int j = 0; j < polySet->polygons[i].size(); j++) {
            NodeTaskProto::Vector *protoVector = protoPolygon->add_vectors();
            protoVector->add_data(polySet->polygons[i][j][0]);
            protoVector->add_data(polySet->polygons[i][j][1]);
            protoVector->add_data(polySet->polygons[i][j][2]);
        }
    }

    for (int i = 0; i < polySet->borders.size(); i++) {
        NodeTaskProto::Polygon *protoPolygon = protoPolySet->add_borders();
        for (int j = 0; j < polySet->borders[i].size(); j++) {
            NodeTaskProto::Vector *protoVector = protoPolygon->add_vectors();
            protoVector->add_data(polySet->borders[i][j][0]);
            protoVector->add_data(polySet->borders[i][j][1]);
            protoVector->add_data(polySet->borders[i][j][2]);
        }
    }
}

PolySet * NodeTaskSerialisation::deserialisePolySet(NodeTaskProto::PolySet protoPolySet)
{
    PolySet *polySet = new PolySet();
    polySet->is2d = protoPolySet.is2d();
    polySet->convexity = protoPolySet.convexivity();
    // TODO Just setting with GRID_FINE
    polySet->grid = Grid3d<void*>(GRID_FINE);

    // Do the polys
    for (int i = 0; i < protoPolySet.polygons().size(); i++) {
        std::vector<Vector3d> polygon;
        for (int j = 0; j < protoPolySet.polygons(i).vectors().size(); j++) {
            double x = protoPolySet.polygons(i).vectors(j).data(0);
            double y = protoPolySet.polygons(i).vectors(j).data(1);
            double z = protoPolySet.polygons(i).vectors(j).data(2);
            polygon.push_back(Vector3d(x, y, z));
        }
        polySet->polygons.push_back(polygon);
    }

    for (int i = 0; i < protoPolySet.borders().size(); i++) {
        std::vector<Vector3d> polygon;
        for (int j = 0; j < protoPolySet.borders(i).vectors().size(); j++) {
            double x = protoPolySet.borders(i).vectors(j).data(0);
            double y = protoPolySet.borders(i).vectors(j).data(1);
            double z = protoPolySet.borders(i).vectors(j).data(2);
            polygon.push_back(Vector3d(x, y, z));
        }
        polySet->borders.push_back(polygon);
    }

    return polySet;
}

