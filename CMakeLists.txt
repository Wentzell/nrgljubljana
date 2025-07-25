# ##############################################################################
#
# NRG Ljubljana
#
# Copyright (C) Rok Zitko, rok.zitko@ijs.si
#
# NRG Ljubljana is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# NRG Ljubljana is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# nrgljubljana (in the file COPYING.txt in this directory). If not, see
# <http://www.gnu.org/licenses/>.
#
# ##############################################################################

cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
cmake_policy(VERSION ${CMAKE_VERSION})
if(POLICY CMP0173) # CMakeFindFrameworks Availability
  cmake_policy(SET CMP0173 OLD)
endif()

# ############
# Define Project
project(NRGLJUBLJANA VERSION 2025.01 LANGUAGES C CXX)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

# Get the git hash & print status
execute_process(COMMAND git rev-parse HEAD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE PROJECT_GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "${PROJECT_NAME} version : ${PROJECT_VERSION}")
message(STATUS "${PROJECT_NAME} Git hash: ${PROJECT_GIT_HASH}")

# Assert that Install directory is given and valid.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR (NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX}))
  message(FATAL_ERROR "No install prefix given (or invalid)")
endif()
message(STATUS "-------- CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX} --------")

# ############
# Options

# Make additional Find Modules available
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/share/cmake/Modules)

# FindMathematica
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/Mathematica)

# Default to Release build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Type of build" FORCE)
endif()
message(STATUS "-------- BUILD-TYPE: ${CMAKE_BUILD_TYPE} --------")

# Export the list of compile-commands into compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global compiler options
add_compile_options(
  $<$<CONFIG:Debug>:-Og>
  $<$<CONFIG:Debug>:-fno-var-tracking>
  $<$<CONFIG:Debug>:-ggdb3>
)

# Create an Interface target for compiler warnings
if(NOT TARGET project_warnings)
  add_library(project_warnings INTERFACE)
endif()
install(TARGETS project_warnings EXPORT nrgljubljana-targets)
target_compile_options(project_warnings
  INTERFACE
    -Wall
    -Wextra
    -Wnon-virtual-dtor
#    -Wold-style-cast  # because Eigen
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
#    -Wconversion # USEFUL, but too many false flags..
#    -Wsign-conversion # because {fmt}, Eigen
    $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond>
    $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op>
    -Wno-sign-compare
    -Wno-unused-parameter # USEFUL, occasionally...
    -Wno-unused-function   # issue in matrix tool
    -Wno-deprecated-declarations  # HighFive using boost/ublas
    $<$<CXX_COMPILER_ID:GNU>:-Wshadow=local>
    $<$<CXX_COMPILER_ID:Clang>:-Wshadow>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-gcc-compat>
)

# ###########
# MPI support

message(STATUS "-------- MPI detection -------------")
unset(MPIEXEC_EXECUTABLE CACHE)
find_package(MPI REQUIRED)

# Backward compatibility for older FindMPI.cmake
if(NOT MPIEXEC_EXECUTABLE)
  set(MPIEXEC_EXECUTABLE ${MPIEXEC} CACHE FILENAME "MPI Executable")
endif()

# Compatibility to Open-MPI 3.0.0: check whether MPI executable has option --oversubscribe and add it
execute_process(COMMAND ${MPIEXEC_EXECUTABLE} --oversubscribe ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS} ls ${MPIEXEC_POSTFLAGS} RESULT_VARIABLE HAS_NO_OVERSUBSCRIBE OUTPUT_QUIET ERROR_QUIET)
if(NOT HAS_NO_OVERSUBSCRIBE)
 list(APPEND MPIEXEC_PREFLAGS --oversubscribe)
 set(MPIEXEC_PREFLAGS ${MPIEXEC_PREFLAGS} CACHE STRING "These flags will be directly before the executable that is being run by mpiexec." FORCE)
endif()

message(STATUS "MPI_FOUND: ${MPI_FOUND} VERSION: ${MPI_VERSION}")
message(STATUS "MPI_CXX_COMPILER: ${MPI_CXX_COMPILER}")
message(STATUS "MPIEXEC: ${MPIEXEC}")
message(STATUS "MPIEXEC_EXECUTABLE: ${MPIEXEC_EXECUTABLE}")
message(STATUS "MPIEXEC_NUMPROC_FLAG: ${MPIEXEC_NUMPROC_FLAG}")
message(STATUS "MPIEXEC_PREFLAGS: ${MPIEXEC_PREFLAGS}")
message(STATUS "MPIEXEC_POSTFLAGS: ${MPIEXEC_POSTFLAGS}")

# Create an interface target
add_library(mpi INTERFACE)
target_include_directories(mpi SYSTEM INTERFACE ${MPI_C_INCLUDE_PATH})
target_link_libraries(mpi INTERFACE ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
separate_arguments(MPI_C_COMPILE_FLAGS) # Convert to list
target_compile_options(mpi INTERFACE ${MPI_C_COMPILE_FLAGS})
install(TARGETS mpi EXPORT nrgljubljana-targets)

# ############
# Dependencies

# HDF5
find_package(HDF5 REQUIRED COMPONENTS C HL)
add_library(hdf5 INTERFACE)
target_include_directories(hdf5 SYSTEM INTERFACE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf5 INTERFACE "${HDF5_LIBRARIES}" ${HDF5_HL_LIBRARIES})
target_compile_options(hdf5 INTERFACE ${HDF5_DEFINITIONS})
install(TARGETS hdf5 EXPORT nrgljubljana-targets)

# OpenMP
find_package(OpenMP REQUIRED)
add_library(openmp INTERFACE)
target_compile_options(openmp INTERFACE ${OpenMP_CXX_FLAGS})
target_link_libraries(openmp INTERFACE ${OpenMP_CXX_FLAGS})
install(TARGETS openmp EXPORT nrgljubljana-targets)

# Boost
# https://stackoverflow.com/questions/57415206/is-there-a-way-to-get-rid-of-the-new-boost-version-may-have-incorrect-or-missin
set(Boost_NO_WARN_NEW_VERSIONS 1)
set(Boost_NO_BOOST_CMAKE ON)
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.53 REQUIRED COMPONENTS serialization mpi)
# note: header-only libraries such as multiprecision do not need to be listed
# as components in find_package()

# GSL
find_package(GSL REQUIRED)
add_library(gsl INTERFACE)
target_include_directories(gsl SYSTEM INTERFACE ${GSL_INCLUDE_DIRS})
target_link_libraries(gsl INTERFACE ${GSL_LIBRARIES})
install(TARGETS gsl EXPORT nrgljubljana-targets)

# GMP
find_package(GMP REQUIRED)
install(TARGETS gmp EXPORT nrgljubljana-targets)

# CPM for downloading dependencies
include(CMake/CPM.cmake)

# Google Test
CPMAddPackage("gh:google/googletest#v1.15.2")

# Eigen, tries finding in system first
CPMFindPackage(
  NAME Eigen3
  GITLAB_REPOSITORY libeigen/eigen
  GIT_TAG 3.4.0
  OPTIONS
    "BUILD_TESTING OFF" # Disable tests for Eigen
)

# High five library
CPMAddPackage(
  GITHUB_REPOSITORY highfive-devs/highfive
  GIT_TAG v3.1.0
  OPTIONS
    "HIGHFIVE_BUILD_DOCS OFF" # Disable HighFive documentation
)

# fmt
# Check if fmt is already a target; important when using as an outside library in nrgljubljana_interface
if (NOT TARGET fmt::fmt)
  CPMAddPackage("gh:fmtlib/fmt#11.2.0")
endif()

# range-v3
CPMAddPackage("gh:ericniebler/range-v3#0.12.0")

# MPFR
#find_package(MPFR REQUIRED)
#install(TARGETS mpfr EXPORT nrgljubljana-targets)
### Not yet. Long-term goal: promote thermodynamic quantities to multiple-precision variables.

# Mathematica (optional)
find_package(Mathematica)
if(Mathematica_FOUND)
  message(STATUS "Mathematica executable ${Mathematica_KERNEL_EXECUTABLE}")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/BLAS_LAPACK)
# BLAS/Lapack
find_package(LAPACK REQUIRED) # also looks up BLAS
add_library(blas_lapack INTERFACE)
string(FIND "${BLAS_LIBRARIES}" "libmkl" MKL_FIND_POS)
if(MKL_FIND_POS GREATER -1)
  message(STATUS "Using MKL (libmkl) for linear algebra")
  list(APPEND BLAS_LINKER_FLAGS "-DMKL")
  # https://software.intel.com/en-us/forums/intel-math-kernel-library/topic/759670
  # https://software.intel.com/en-us/articles/a-new-linking-model-single-dynamic-library-mkl_rt-since-intel-mkl-103
endif()
message(STATUS "BLAS_LIBRARIES=${BLAS_LIBRARIES}")
message(STATUS "BLAS_LINKER_FLAGS=${BLAS_LINKER_FLAGS}")
message(STATUS "LAPACK_LIBRARIES=${LAPACK_LIBRARIES}")
message(STATUS "LAPACK_LINKER_FLAGS=${LAPACK_LINKER_FLAGS}")
target_link_libraries(blas_lapack INTERFACE LAPACK::LAPACK)
install(TARGETS blas_lapack EXPORT nrgljubljana-targets)

# Check for availability of cblas interface
set(CMAKE_REQUIRED_LIBRARIES ${LAPACK_LIBRARIES} ${BLAS_LIBRARIES})
set(CMAKE_REQUIRED_FLAGS ${LAPACK_LINKER_FLAGS} ${BLAS_LINKER_FLAGS})
check_function_exists(cblas_dgemm HAVE_CBLAS)
unset(CMAKE_REQUIRED_LIBRARIES)
unset(CMAKE_REQUIRED_FLAGS)
if(HAVE_CBLAS)
  message(STATUS "cblas interface found in blas library")
  set(CBLAS_WORKAROUND OFF)
else()
  message(WARNING "cblas interface not found; will provide a workaround")
  set(CBLAS_WORKAROUND ON)
endif()

# Symmetry types to compile in
option(SYM_MORE "Compile in an extended set of symmetry types" ON)
option(SYM_ALL  "Compile in the full set of symmetry types (long compilation time)" ON)

if(SYM_MORE)
  message(STATUS "Building an extended set of symmetry types")
endif()
if(SYM_ALL)
  message(STATUS "Building the full set of symmetry types")
endif()

# Test suites
option(TEST_LONG "Enable tests with long runtime" OFF)

# Dynamic Analyzer Checks

option(ASAN  "Compile library and executables with LLVM Address Sanitizer" OFF)
option(UBSAN "Compile library and executables with LLVM Undefined Behavior Sanitizer" OFF)

if(ASAN)
  if(NOT TARGET asan)
    find_package(sanitizer REQUIRED "asan")
  endif()
  install(TARGETS asan EXPORT nrgljubljana-targets)
endif()
if(UBSAN)
  if(NOT TARGET ubsan)
    find_package(sanitizer REQUIRED "ubsan")
  endif()
  install(TARGETS ubsan EXPORT nrgljubljana-targets)
endif()

# Resolve Clang Linktime Problems
# CMake will adjust any linker flags from '-L path_to/mylib.so' to -lmylib
# if the proper mylib.so is automatically found by the linker, i.e.
# the directory comes first in LIBRARY_PATH.
# The clang linker however ignores LIBRARY_PATH.
# We thus explicitly add the content of LIBRARY_PATH to the LDFLAGS
# FIXME For future cmake versions we should populate the
# INTERFACE_LINK_DIRECTORIES of the triqs target
# ---------------------------------
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND DEFINED ENV{LIBRARY_PATH})
  string(REPLACE ":" ";" LINK_DIRS $ENV{LIBRARY_PATH})
  foreach(dir ${LINK_DIRS})
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -L${dir}")
    string(APPEND CMAKE_MODULE_LINKER_FLAGS " -L${dir}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -L${dir}")
  endforeach()
endif()

# #############
# Build Project

# Build and install the nrgljubljana library
add_subdirectory(c++)

# Tools
add_subdirectory(tools)

# Tests
option(Build_Tests "Build tests" ON)
if(Build_Tests AND NOT IS_SUBPROJECT)
  enable_testing()
  add_subdirectory(test)
endif()

# Build the documentation
option(Build_Documentation "Build documentation" OFF)
if(Build_Documentation AND NOT IS_SUBPROJECT)
  message(STATUS "Documentation Build enabled")
  add_subdirectory(doc)
endif()

# Additional configuration files
add_subdirectory(share)

# Mathematica part of NRG Ljubljana (nrginit)
add_subdirectory(nrginit)

# #############
# Debian Package

option(BUILD_DEBIAN_PACKAGE "Build a deb package" OFF)
if(BUILD_DEBIAN_PACKAGE AND NOT IS_SUBPROJECT)
  if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
    message(FATAL_ERROR "CMAKE_INSTALL_PREFIX must be /usr for packaging")
  endif()
  set(CPACK_PACKAGE_NAME nrgljubljana)
  set(CPACK_GENERATOR "DEB")
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  set(CPACK_PACKAGE_CONTACT "https://github.com/rokzitko/nrgljubljana")
  execute_process(COMMAND dpkg --print-architecture OUTPUT_VARIABLE CMAKE_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
  set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
  include(CPack)
endif()

