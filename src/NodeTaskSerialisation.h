#ifndef NODETASKSERIALISATION_H
#define NODETASKSERIALISATION_H

#import "NodeTask.h"
#import "NodeTaskProto.pb.h"
#import "CGALEvaluator.h"
#include "polyset.h"
#include <CGAL/IO/Nef_polyhedron_iostream_3.h>
#include "boost/foreach.hpp"
#include <iostream>
#include "printutils.h"
#include "linalg.h"

typedef google::protobuf::RepeatedPtrField<NodeTaskProto::Polyhedron> ProtoPolys;

// Defines static serialisation and deserialisation methods for NodeTasks
class NodeTaskSerialisation {
public:
    static NodeTaskProto::NodeTaskMsg serialise(NodeTask *nodeTask);
    static NodeTask * deserialise(NodeTaskProto::NodeTaskMsg nodeTaskProto);
private:
    static NodeTaskProto::CGALTaskDetails_OpType serialiseOp(CGALEvaluator::CsgOp op);
    static CGALEvaluator::CsgOp deserialiseOp(NodeTaskProto::CGALTaskDetails_OpType protoOp);

    static void serialiseMatrix(Transform3d matrix, NodeTaskProto::Matrix *protoMatrix);
    static Transform3d deserialiseMatrix(NodeTaskProto::Matrix protoMatrix);

    static void serialisePolyhedrons(std::list<CGAL_Nef_polyhedron> polys, ProtoPolys *protoPolys);
    static std::list<CGAL_Nef_polyhedron> deserialisePolyhedrons(ProtoPolys protoPolys);

    static void serialisePolySet(PolySet *polySet, NodeTaskProto::PolySet *protoPolySet);
    static PolySet * deserialisePolySet(NodeTaskProto::PolySet protoPolySet);
};

#endif // NODETASKSERIALISATION_H
