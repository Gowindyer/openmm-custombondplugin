cmake_minimum_required(VERSION 3.16)

project(OpenMMCustomBondPlugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(CTest)

option(OPENMM_CUSTOMBOND_BUILD_REFERENCE "Build the Reference platform plugin" ON)
option(OPENMM_CUSTOMBOND_BUILD_CPU "Build the CPU platform plugin" ON)
option(OPENMM_CUSTOMBOND_BUILD_CUDA "Build the CUDA platform plugin" OFF)
option(OPENMM_CUSTOMBOND_BUILD_OPENCL "Build the OpenCL platform plugin" OFF)
option(OPENMM_CUSTOMBOND_BUILD_TESTS "Build CustomBond plugin tests" ${BUILD_TESTING})
option(OPENMM_CUSTOMBOND_BUILD_CUDA_DOUBLE_PRECISION_TESTS "Build mixed/double precision CUDA tests" OFF)
option(OPENMM_CUSTOMBOND_BUILD_OPENCL_DOUBLE_PRECISION_TESTS "Build mixed/double precision OpenCL tests" OFF)
option(OPENMM_CUSTOMBOND_USE_LEPTON_JIT "Compile against an OpenMM build that uses Lepton JIT" ON)
set(OPENMM_ASMJIT_INCLUDE_DIR "" CACHE PATH "Directory containing the asmjit folder, needed when LEPTON_USE_JIT is enabled")

if(OPENMM_CUSTOMBOND_USE_LEPTON_JIT)
    if(NOT OPENMM_ASMJIT_INCLUDE_DIR AND EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/asmjit/asmjit/x86.h")
        set(OPENMM_ASMJIT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/asmjit")
    endif()
    if(NOT EXISTS "${OPENMM_ASMJIT_INCLUDE_DIR}/asmjit/x86.h")
        message(FATAL_ERROR "OPENMM_CUSTOMBOND_USE_LEPTON_JIT is ON, but asmjit/x86.h was not found. Set OPENMM_ASMJIT_INCLUDE_DIR or use the vendored thirdparty/asmjit headers.")
    endif()
    add_compile_definitions(LEPTON_USE_JIT)
endif()

set(OPENMM_DIR "" CACHE PATH "OpenMM install prefix, for example /usr/local/openmm")
if(NOT OPENMM_DIR AND DEFINED ENV{OPENMM_DIR})
    set(OPENMM_DIR "$ENV{OPENMM_DIR}" CACHE PATH "OpenMM install prefix" FORCE)
endif()
if(NOT OPENMM_DIR)
    message(FATAL_ERROR "Set OPENMM_DIR to an installed OpenMM prefix.")
endif()
file(TO_CMAKE_PATH "${OPENMM_DIR}" OPENMM_DIR)
get_filename_component(OPENMM_DIR "${OPENMM_DIR}" ABSOLUTE)

set(OPENMM_INCLUDE_DIR "${OPENMM_DIR}/include")
set(OPENMM_REFERENCE_INCLUDE_DIR "${OPENMM_INCLUDE_DIR}/openmm/reference")
set(OPENMM_CPU_INCLUDE_DIR "${OPENMM_INCLUDE_DIR}/openmm/cpu")
set(OPENMM_CUDA_INCLUDE_DIR "${OPENMM_INCLUDE_DIR}/openmm/cuda")
set(OPENMM_OPENCL_INCLUDE_DIR "${OPENMM_INCLUDE_DIR}/openmm/opencl")
set(OPENMM_COMMON_INCLUDE_DIR "${OPENMM_INCLUDE_DIR}/openmm/common")
set(OPENMM_LIBRARY_DIRS "${OPENMM_DIR}/lib" "${OPENMM_DIR}/lib64")
set(OPENMM_PLUGIN_LIBRARY_DIRS "${OPENMM_DIR}/lib/plugins" "${OPENMM_DIR}/lib64/plugins")

if(NOT EXISTS "${OPENMM_INCLUDE_DIR}/openmm/Platform.h")
    message(FATAL_ERROR "Could not find OpenMM headers under ${OPENMM_INCLUDE_DIR}.")
endif()

find_library(OPENMM_LIBRARY NAMES OpenMM PATHS ${OPENMM_LIBRARY_DIRS} NO_DEFAULT_PATH)
if(NOT OPENMM_LIBRARY)
    message(FATAL_ERROR "Could not find libOpenMM in ${OPENMM_LIBRARY_DIRS}.")
endif()

if(OPENMM_CUSTOMBOND_BUILD_CPU)
    if(NOT EXISTS "${OPENMM_INCLUDE_DIR}/openmm/cpu/CpuPlatform.h")
        message(FATAL_ERROR "CPU plugin requested, but ${OPENMM_INCLUDE_DIR}/openmm/cpu/CpuPlatform.h was not found.")
    endif()
    find_library(OPENMM_CPU_LIBRARY NAMES OpenMMCPU PATHS ${OPENMM_PLUGIN_LIBRARY_DIRS} ${OPENMM_LIBRARY_DIRS} NO_DEFAULT_PATH)
    if(NOT OPENMM_CPU_LIBRARY)
        message(FATAL_ERROR "CPU plugin requested, but libOpenMMCPU was not found in ${OPENMM_PLUGIN_LIBRARY_DIRS}.")
    endif()
endif()

if(OPENMM_CUSTOMBOND_BUILD_CUDA)
    if(NOT EXISTS "${OPENMM_INCLUDE_DIR}/openmm/cuda/CudaPlatform.h")
        message(FATAL_ERROR "CUDA plugin requested, but ${OPENMM_INCLUDE_DIR}/openmm/cuda/CudaPlatform.h was not found.")
    endif()
    find_library(OPENMM_CUDA_LIBRARY NAMES OpenMMCUDA PATHS ${OPENMM_PLUGIN_LIBRARY_DIRS} ${OPENMM_LIBRARY_DIRS} NO_DEFAULT_PATH)
    if(NOT OPENMM_CUDA_LIBRARY)
        message(FATAL_ERROR "CUDA plugin requested, but libOpenMMCUDA was not found in ${OPENMM_PLUGIN_LIBRARY_DIRS}.")
    endif()
    find_package(CUDAToolkit REQUIRED)
endif()

if(OPENMM_CUSTOMBOND_BUILD_OPENCL)
    if(NOT EXISTS "${OPENMM_INCLUDE_DIR}/openmm/opencl/OpenCLPlatform.h")
        message(FATAL_ERROR "OpenCL plugin requested, but ${OPENMM_INCLUDE_DIR}/openmm/opencl/OpenCLPlatform.h was not found.")
    endif()
    find_library(OPENMM_OPENCL_LIBRARY NAMES OpenMMOpenCL PATHS ${OPENMM_PLUGIN_LIBRARY_DIRS} ${OPENMM_LIBRARY_DIRS} NO_DEFAULT_PATH)
    if(NOT OPENMM_OPENCL_LIBRARY)
        message(FATAL_ERROR "OpenCL plugin requested, but libOpenMMOpenCL was not found in ${OPENMM_PLUGIN_LIBRARY_DIRS}.")
    endif()
    find_package(OpenCL REQUIRED)
endif()

find_program(PYTHON_EXECUTABLE NAMES python3 python)
find_program(SWIG_EXECUTABLE swig)
if(PYTHON_EXECUTABLE AND SWIG_EXECUTABLE)
    set(OPENMM_CUSTOMBOND_BUILD_PYTHON_WRAPPERS ON CACHE BOOL "Build wrappers for Python")
else()
    set(OPENMM_CUSTOMBOND_BUILD_PYTHON_WRAPPERS OFF CACHE BOOL "Build wrappers for Python")
endif()

set(OPENMM_CUSTOMBOND_LIBRARY_NAME OpenMMCustomBond)
set(OPENMM_CUSTOMBOND_TARGET ${OPENMM_CUSTOMBOND_LIBRARY_NAME})
set(OPENMM_CUSTOMBOND_PLUGIN_RPATH ${OPENMM_LIBRARY_DIRS} ${OPENMM_PLUGIN_LIBRARY_DIRS})

file(GLOB CUSTOMBOND_API_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/include/OpenMMCustomBond.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/include/openmm/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/include/openmm/internal/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serialization/include/openmm/serialization/*.h
)
set(CUSTOMBOND_API_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/src/CustomBondPluginForce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/src/CustomBondPluginForceImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serialization/src/CustomBondPluginForceProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serialization/src/CustomBondSerializationProxyRegistration.cpp
)

add_library(${OPENMM_CUSTOMBOND_TARGET} SHARED ${CUSTOMBOND_API_SOURCES} ${CUSTOMBOND_API_HEADERS})
target_include_directories(${OPENMM_CUSTOMBOND_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/openmmapi/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/serialization/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
)
target_compile_definitions(${OPENMM_CUSTOMBOND_TARGET}
    PRIVATE
        OPENMM_CUSTOMBOND_BUILDING_SHARED_LIBRARY
        LEPTON_BUILDING_SHARED_LIBRARY
)
target_link_libraries(${OPENMM_CUSTOMBOND_TARGET} PUBLIC ${OPENMM_LIBRARY})
set_target_properties(${OPENMM_CUSTOMBOND_TARGET} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    BUILD_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
    INSTALL_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
)

if(OPENMM_CUSTOMBOND_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(serialization/tests)
endif()

if(OPENMM_CUSTOMBOND_BUILD_REFERENCE)
    add_subdirectory(platforms/reference)
endif()

if(OPENMM_CUSTOMBOND_BUILD_CPU)
    add_subdirectory(platforms/cpu)
endif()

if(OPENMM_CUSTOMBOND_BUILD_CUDA)
    add_subdirectory(platforms/cuda)
endif()

if(OPENMM_CUSTOMBOND_BUILD_OPENCL)
    add_subdirectory(platforms/opencl)
endif()

if(OPENMM_CUSTOMBOND_BUILD_PYTHON_WRAPPERS)
    add_subdirectory(python)
endif()

install(TARGETS ${OPENMM_CUSTOMBOND_TARGET}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES openmmapi/include/OpenMMCustomBond.h DESTINATION include)
install(DIRECTORY openmmapi/include/openmm DESTINATION include FILES_MATCHING PATTERN "*.h")
install(DIRECTORY serialization/include/openmm DESTINATION include FILES_MATCHING PATTERN "*.h")
