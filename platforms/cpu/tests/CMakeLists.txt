add_executable(TestCustomBondPluginCpuForce TestCustomBondPluginCpuForce.cpp)
target_include_directories(TestCustomBondPluginCpuForce
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/platforms/cpu/src
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_CPU_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
)
target_link_libraries(TestCustomBondPluginCpuForce
    PRIVATE
        OpenMMCustomBondCPU
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_CPU_LIBRARY}
        ${OPENMM_LIBRARY}
)
add_test(NAME TestCustomBondPluginCpuForce COMMAND TestCustomBondPluginCpuForce single)
