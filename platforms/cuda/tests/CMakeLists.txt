add_executable(TestCustomBondPluginCudaForce TestCustomBondPluginCudaForce.cpp)
target_include_directories(TestCustomBondPluginCudaForce
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/platforms/cuda/include
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_CUDA_INCLUDE_DIR}
        ${OPENMM_COMMON_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
        ${CUDAToolkit_INCLUDE_DIRS}
)
target_link_libraries(TestCustomBondPluginCudaForce
    PRIVATE
        OpenMMCustomBondCUDA
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_CUDA_LIBRARY}
        ${OPENMM_LIBRARY}
)
add_test(NAME TestCustomBondPluginCudaForceSingle COMMAND TestCustomBondPluginCudaForce single)
if(OPENMM_CUSTOMBOND_BUILD_CUDA_DOUBLE_PRECISION_TESTS)
    add_test(NAME TestCustomBondPluginCudaForceMixed COMMAND TestCustomBondPluginCudaForce mixed)
    add_test(NAME TestCustomBondPluginCudaForceDouble COMMAND TestCustomBondPluginCudaForce double)
endif()
