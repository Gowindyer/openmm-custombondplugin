add_executable(TestCustomBondPluginOpenCLForce TestCustomBondPluginOpenCLForce.cpp)
target_include_directories(TestCustomBondPluginOpenCLForce
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/platforms/opencl/include
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_OPENCL_INCLUDE_DIR}
        ${OPENMM_COMMON_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
        ${OpenCL_INCLUDE_DIRS}
)
target_link_libraries(TestCustomBondPluginOpenCLForce
    PRIVATE
        OpenMMCustomBondOpenCL
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_OPENCL_LIBRARY}
        ${OPENMM_LIBRARY}
        ${OpenCL_LIBRARIES}
)
add_test(NAME TestCustomBondPluginOpenCLForceSingle COMMAND TestCustomBondPluginOpenCLForce single)
if(OPENMM_CUSTOMBOND_BUILD_OPENCL_DOUBLE_PRECISION_TESTS)
    add_test(NAME TestCustomBondPluginOpenCLForceMixed COMMAND TestCustomBondPluginOpenCLForce mixed)
    add_test(NAME TestCustomBondPluginOpenCLForceDouble COMMAND TestCustomBondPluginOpenCLForce double)
endif()
