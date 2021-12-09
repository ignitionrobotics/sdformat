message(STATUS "Building for arch: ${ARCH}")

#################################################
# Find tinyxml2.
ign_find_package(TINYXML2 REQUIRED)

################################################
# Find urdfdom parser. Logic:
#
#  1. if USE_INTERNAL_URDF is unset, try to use system installation, fallback to internal copy
#  2. if USE_INTERNAL_URDF is set to True, use the internal copy
#  3. if USE_INTERNAL_URDF is set to False, force to search system installation, fail on error
if (NOT DEFINED USE_INTERNAL_URDF OR NOT USE_INTERNAL_URDF)
  ign_find_package(IgnURDFDOM VERSION 1.0 QUIET)
  if (NOT IgnURDFDOM_FOUND)
    if (NOT DEFINED USE_INTERNAL_URDF)
      # fallback to internal urdf
      set(USE_INTERNAL_URDF ON)
    else()
      ign_build_error("Couldn't find the urdfdom >= 1.0 system installation")
    endif()
  endif()
endif()

if (USE_INTERNAL_URDF)
  message(STATUS "Using internal URDF")
endif()

#################################################
# Find ign command line utility:
ign_find_package(ignition-tools)

################################################
# Find the Python interpreter for running the
# check_test_ran.py script
find_package(PythonInterp 3 QUIET)

################################################
# Find psutil python package for memory tests
find_python_module(psutil)
if(NOT PY_PSUTIL)
  ign_build_warning("Python psutil package not found. Memory leak tests will be skipped")
endif()

################################################
# Find Valgrind for checking memory leaks in the
# tests
find_program(VALGRIND_PROGRAM NAMES valgrind PATH ${VALGRIND_ROOT}/bin)
option(SDFORMAT_RUN_VALGRIND_TESTS "Run sdformat tests with Valgrind" FALSE)
mark_as_advanced(SDFORMAT_RUN_VALGRIND_TESTS)
if (SDFORMAT_RUN_VALGRIND_TESTS AND NOT VALGRIND_PROGRAM)
  ign_build_warning("valgrind not found. Memory check tests will be skipped.")
endif()

################################################
# Find ruby executable to produce xml schemas
find_program(RUBY ruby)
if (NOT RUBY)
    ign_build_error ("Ruby version 1.9 is needed to build xml schemas")
else()
    message(STATUS "Found ruby executable: ${RUBY}")
endif()

########################################
# Find ignition math
# Set a variable for generating ProjectConfig.cmake
ign_find_package(ignition-math6 VERSION REQUIRED)
set(IGN_MATH_VER ${ignition-math6_VERSION_MAJOR})

