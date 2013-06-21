cgal {
  DEFINES += ENABLE_CGAL

  # Optionally specify location of CGAL using the 
  # CGALDIR env. variable
  CGAL_DIR = $$(CGALDIR)
  CGAL_LIB_DIR = $$(CGALLIBDIR)
  !isEmpty(CGAL_DIR) {
    QMAKE_INCDIR += $$CGAL_DIR/
    win32: QMAKE_INCDIR += $$CGAL_DIR/auxiliary/gmp/include
    QMAKE_LIBDIR += $$CGAL_LIB_DIR
    message("CGAL inc location: $$CGAL_DIR")
    message("CGAL lib location: $$CGAL_LIB_DIR")
  }

  CONFIG(mingw-cross-env) {
    LIBS += -lgmp -lmpfr -lCGAL
    QMAKE_CXXFLAGS += -frounding-math 
  } else {
    win32 {
      *-g++* { 
        QMAKE_CXXFLAGS += -frounding-math 
      }
      LIBS += $$CGAL_DIR/auxiliary/gmp/lib/libmpfr-4.lib -lCGAL-vc90-mt-s
    } else {
      LIBS += -lgmp -lmpfr -lCGAL
      QMAKE_CXXFLAGS += -frounding-math 
    }
  }

	*clang* {
		QMAKE_CXXFLAGS -= -frounding-math
	}
}
