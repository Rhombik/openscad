#include "console.h"
#include "node.h"
#include "module.h"
#include "context.h"
#include "value.h"
#include "export.h"
#include "builtin.h"
#include "nodedumper.h"
#include "printutils.h"
#include "handle_dep.h"
#include "parsersettings.h"
#include "Tree.h"

#include <string>
#include <vector>
#include <fstream>

#include "CGAL_Nef_polyhedron.h"
#include "CGALEvaluator.h"
#include "NodeTaskVisitor.h"

#include <sstream>

#include <boost/foreach.hpp>
#include "NodeTaskSerialisation.h"
#include "NodeTaskProto.pb.h"

// Extra headers
#include "CSGTermEvaluator.h"
#include "csgtermnormalizer.h"
#include "csgterm.h"
#include "polyset.h"

// End of extra headers

std::string currentdir;

using std::string;
using std::vector;

void exportToStl (CGAL_Nef_polyhedron poly, std::string filename)
{
    std::ofstream fstream(filename);
    export_stl(&poly, fstream);
    fstream.close();
}

void report_func(const class AbstractNode*, void *vp, int mark)
{
    int v = (int)((mark*100.0) / progress_report_count);
    int percent = v < 100 ? v : 99;
    PRINTB("Progress:%d", percent);
}

void (*rf)(const class AbstractNode *node, void *userdata, int mark) = report_func;

void set_report_func(void (*f)(const class AbstractNode *node, void *userdata, int mark)) {
    rf = f;
}

int convertToStl(char *input, char *output, int thingmy) {

    PRINT("Starting conversion process");
    // Causes CGAL errors to abort directly instead of throwing exceptions
    // (which we don't catch). This gives us stack traces without rerunning in gdb.
    CGAL::set_error_behaviour(CGAL::ABORT);

    Builtins::instance()->initialize();
    const char *input_file = input;
    const char *stl_output_file = output;

    parser_init("Application dir");

    // Initialize global visitors
    NodeCache nodecache;
    NodeDumper dumper(nodecache);
    Tree tree;

    CGALEvaluator cgalevaluator(tree);

    Context root_ctx;
    register_builtin(root_ctx);

    Module *root_module;
    ModuleInstantiation root_inst;
    AbstractNode *root_node;

    handle_dep(input_file);

    std::ifstream ifs(input_file);
    if (!ifs.is_open()) {
        fprintf(stderr, "Can't open input file '%s'!\n", input_file);
        exit(1);
    }
    std::string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::string parentpath(input_file);
    PRINT("Starting to parse");
    root_module = parse(text.c_str(), parentpath.c_str(), false);
    
    if (!root_module) {
       PRINT("Parsing failed, please check input - particularly for invisible characters/HTML if you have copied and pasted.");
       return 1;
    }
    else {
       PRINT("Parsing complete");
    }
    
    root_module->handleDependencies();

    AbstractNode::resetIndexCounter();
    PRINT("Starting to evaluate");
    root_node = root_module->evaluate(&root_ctx, &root_inst);
    PRINT("Evaluating complete");
    tree.setRoot(root_node);
    progress_report_prep(root_node, rf, NULL);

    PRINT("Evaluating Mesh");
    CGAL_Nef_polyhedron resultPoly;
    resultPoly = cgalevaluator.evaluateCGALMesh(*root_node);

    if(thingmy != 0) {
        fprintf(stderr, "Assertion failure in CGAL. h.has_on(p).\n");
        exit(1);
    }

    PRINT("Evaluating MESH complete");


    if (resultPoly.dim != 3) {
        PRINT("Current top level object is not a 3D object.\n");
        return 1;
    }
    if (!resultPoly.p3->is_simple()) {
        PRINT("Object isn't a valid 2-manifold! Modify your design.\n");
        return 1;
    }
    std::ofstream fstream(stl_output_file);
    if (!fstream.is_open()) {
        PRINTB("Can't open file \"%s\" for export", stl_output_file);
    }
    else {
        PRINT("Starting to convert to STL");
        export_stl(&resultPoly, fstream);
        PRINT("Converting to STL complete");
        fstream.close();
    }

    delete root_node;
    return 0;
}

int main() {
    convertToStl("examples/example001.scad", "result.stl");
}
