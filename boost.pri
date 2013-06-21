boost {

  # Optionally specify location of boost using the
  # BOOSTDIR env. variable
  BOOST_DIR = $$(BOOSTDIR)
  !isEmpty(BOOST_DIR) {
    QMAKE_INCDIR += $$BOOST_DIR
    message("boost location: $$BOOST_DIR")
    win32: QMAKE_LIBDIR += -L$$BOOST_DIR/lib
  }

  CONFIG(mingw-cross-env) {
    DEFINES += BOOST_STATIC
    DEFINES += BOOST_THREAD_USE_LIB
    DEFINES += Boost_USE_STATIC_LIBS
    BOOST_LINK_FLAGS = -lboost_thread_win32-mt -lboost_chrono-mt
  } 

  # check for OPENSCAD_LIBDIR + multithread
  isEmpty(BOOST_LINK_FLAGS) {
    OPENSCAD_LIBDIR = $$(OPENSCAD_LIBRARIES)
    !isEmpty(OPENSCAD_LIBDIR) {
      exists($$OPENSCAD_LIBDIR/lib/libboost*thread-mt*) {
        BOOST_LINK_FLAGS = -lboost_thread-mt
      } else {
        exists($$OPENSCAD_LIBDIR/lib/libboost*thread*) {
          BOOST_LINK_FLAGS = -lboost_thread
        }
      }
    }
  }

  # check for BOOSTDIR + multithread
  isEmpty(BOOST_LINK_FLAGS) {
    BOOST_DIR = $$(BOOSTDIR)
    !isEmpty(BOOST_DIR) {
      exists($$BOOST_DIR/lib/libboost*thread-mt*) {
        BOOST_LINK_FLAGS = -lboost_thread-mt
      } else {
        exists($$BOOST_DIR/lib/libboost*thread*) {
          BOOST_LINK_FLAGS = -lboost_thread
        }
      }
    }
  }

  isEmpty(BOOST_LINK_FLAGS) {
    unix {
      BMT_TEST1 = /usr/lib64/libboost*thread-mt*
      BMT_TEST2 = /usr/lib/libboost*thread-mt*
      BMT_TEST3 = /usr/pkg/lib/libboost*thread-mt* # netbsd
      exists($$BMT_TEST1)|exists($$BMT_TEST2)|exists($$BMT_TEST3) {
        BOOST_LINK_FLAGS = -lboost_thread-mt
      }
    }
  }

  isEmpty(BOOST_LINK_FLAGS) {
    unix|macx {
      BOOST_LINK_FLAGS = -lboost_thread
    }
  }

  LIBS += $$BOOST_LINK_FLAGS

}
