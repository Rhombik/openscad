# Environment variables which can be set to specify library locations:
#   MPIRDIR
#   MPFRDIR
#   BOOSTDIR
#   CGALDIR
#   EIGENDIR
#   GLEWDIR
#   OPENCSGDIR
#   OPENSCAD_LIBRARIES
#
# Please see the 'Buildling' sections of the OpenSCAD user manual 
# for updated tips & workarounds.
#
# http://en.wikibooks.org/wiki/OpenSCAD_User_Manual

isEmpty(QT_VERSION) {
  error("Please use qmake for Qt 4 (probably qmake-qt4)")
}

# Populate VERSION, VERSION_YEAR, VERSION_MONTH, VERSION_DATE from system date
include(version.pri)

debug: DEFINES += DEBUG
QT     -= gui core
LIBS   -= -lQtGui -lQtCore

EMCC = $$(EMCC)
!isEmpty(EMCC) {
    QMAKE_CC = emcc
    QMAKE_CXX = em++
    QMAKE_LINK = emcc
    QMAKE_CXXFLAGS += -std=c++03
    TEMPLATE = lib
}
else {
    QMAKE_CXXFLAGS += -std=c++11
    TEMPLATE = app
}

QMAKE_CXXFLAGS += -U__SSE2_MATH__ -U__GNUG__
DEFINES += CGAL_HAS_NO_THREADS BOOST_NO_NATIVE_LONG_DOUBLE_FP_CLASSIFY BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS

MPFR_DIR = $$(MPFRDIR)
MPFR_LIB_DIR = $$(MPFRLIBDIR)
!isEmpty(MPFR_DIR) {
    QMAKE_INCDIR += $$MPFR_DIR/
    QMAKE_LIBDIR += $$MPFR_LIB_DIR
    message("MPFR inc location: $$MPFR_DIR")
    message("MPFR lib location: $$MPFR_LIB_DIR")
}

GMP_DIR = $$(GMPDIR)
GMP_LIB_DIR = $$(GMPLIBDIR)
!isEmpty(GMP_DIR) {
    QMAKE_INCDIR += $$GMP_DIR/
    QMAKE_LIBDIR += $$GMP_LIB_DIR
    message("GMP inc location: $$GMP_DIR")
    message("GMP lib location: $$GMP_LIB_DIR")
}

PROTO_DIR = $$(PROTODIR)
PROTO_LIB_DIR = $$(PROTOLIBDIR)
!isEmpty(PROTO_DIR) {
    QMAKE_INCDIR += $$PROTO_DIR/
    QMAKE_LIBDIR += $$PROTO_LIB_DIR
    message("Proto inc location: $$PROTO_DIR")
    message("Proto lib location: $$PROTO_LIB_DIR")
}
else {
    LIBS += -L/usr/local/lib/ -lprotobuf-lite
}

INCLUDEPATH += src
TARGET = console

# See Dec 2011 OpenSCAD mailing list, re: CGAL/GCC bugs.
*g++* {
  QMAKE_CXXFLAGS *= -fno-strict-aliasing
}

*clang* {
	# disable enormous amount of warnings about CGAL
	QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
	QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-variable
	QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-function
	QMAKE_CXXFLAGS_WARN_ON += -Wno-c++11-extensions
	# might want to actually turn this on once in a while
	QMAKE_CXXFLAGS_WARN_ON += -Wno-sign-compare
}

# Application configuration
CONFIG += cgal
CONFIG += opencsg
CONFIG += boost
CONFIG += eigen

OBJECTS_DIR = objects
MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

include(flex.pri)
include(bison.pri)
include(cgal.pri)
include(eigen.pri)
include(boost.pri)

LEXSOURCES += src/lexer.l
YACCSOURCES += src/parser.y

HEADERS += src/parsersettings.h \
           src/builtin.h \
           src/context.h \
           src/csgterm.h \
           src/csgtermnormalizer.h \
           src/dxfdata.h \
           src/dxfdim.h \
           src/dxftess.h \
           src/export.h \
           src/expression.h \
           src/function.h \
           src/grid.h \
           src/highlighter.h \
           src/module.h \
           src/node.h \
           src/csgnode.h \
           src/linearextrudenode.h \
           src/rotateextrudenode.h \
           src/projectionnode.h \
           src/cgaladvnode.h \
           src/importnode.h \
           src/transformnode.h \
           src/colornode.h \
           src/rendernode.h \
           src/console.h \
           src/handle_dep.h \
           src/polyset.h \
           src/printutils.h \
           src/value.h \
           src/progress.h \
           src/visitor.h \
           src/state.h \
           src/traverser.h \
           src/nodecache.h \
           src/nodedumper.h \
           src/ModuleCache.h \
           src/PolySetCache.h \
           src/PolySetEvaluator.h \
           src/CSGTermEvaluator.h \
           src/Tree.h \
           src/mathc99.h \
           src/memory.h \
           src/linalg.h \
           src/stl-utils.h \
           src/svg.h \
           src/NodeTask.h \
           src/NodeTaskVisitor.h \
           src/NodeTaskSerialisation.h \
           src/NodeTaskProto.pb.h \
    src/myallocator.h

SOURCES += src/mathc99.cc \
           src/linalg.cc \
           src/handle_dep.cc \
           src/value.cc \
           src/expr.cc \
           src/func.cc \
           src/module.cc \
           src/node.cc \
           src/context.cc \
           src/csgterm.cc \
           src/csgtermnormalizer.cc \
           src/polyset.cc \
           src/csgops.cc \
           src/transform.cc \
           src/color.cc \
           src/primitives.cc \
           src/projection.cc \
           src/cgaladv.cc \
           src/surface.cc \
           src/control.cc \
           src/render.cc \
           src/dxfdata.cc \
           src/dxfdim.cc \
           src/linearextrude.cc \
           src/rotateextrude.cc \
           src/printutils.cc \
           src/progress.cc \
           src/parsersettings.cc \
           src/stl-utils.cc \
           \
           src/nodedumper.cc \
           src/traverser.cc \
           src/PolySetEvaluator.cc \
           src/ModuleCache.cc \
           src/PolySetCache.cc \
           src/Tree.cc \
           \
           src/builtin.cc \
           src/export.cc \
           src/import.cc \
           src/dxftess.cc \
           src/dxftess-cgal.cc \
           src/CSGTermEvaluator.cc \
           src/svg.cc \
           \
           src/console.cc \
           src/NodeTask.cc \
           src/NodeTaskVisitor.cc \
           src/NodeTaskProto.pb.cc \
           src/NodeTaskSerialisation.cc

cgal {
HEADERS += src/cgal.h \
           src/cgalfwd.h \
           src/cgalutils.h \
           src/CGALEvaluator.h \
           src/CGALCache.h \
           src/PolySetCGALEvaluator.h \
           src/CGAL_Nef_polyhedron.h

SOURCES += src/cgalutils.cc \
           src/CGALEvaluator.cc \
           src/PolySetCGALEvaluator.cc \
           src/CGALCache.cc \
           src/CGAL_Nef_polyhedron.cc \
           src/CGAL_Nef_polyhedron_DxfData.cc \
           src/cgaladv_minkowski2.cc
}

isEmpty(PREFIX):PREFIX = /usr/local

target.path = $$PREFIX/bin/
INSTALLS += target

examples.path = $$PREFIX/share/openscad/examples/
examples.files = examples/*
INSTALLS += examples

libraries.path = $$PREFIX/share/openscad/libraries/
libraries.files = libraries/*
INSTALLS += libraries

applications.path = $$PREFIX/share/applications
applications.files = icons/openscad.desktop
INSTALLS += applications

icons.path = $$PREFIX/share/pixmaps
icons.files = icons/openscad.png
INSTALLS += icons


