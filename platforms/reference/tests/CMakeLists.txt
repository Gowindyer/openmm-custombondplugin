add_executable(TestCustomBondPluginReferenceForce TestCustomBondPluginReferenceForce.cpp)
target_include_directories(TestCustomBondPluginReferenceForce
    PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/platforms/reference/include
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
)
target_link_libraries(TestCustomBondPluginReferenceForce
    PRIVATE
        OpenMMCustomBondReference
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_LIBRARY}
)
add_test(NAME TestCustomBondPluginReferenceForce COMMAND TestCustomBondPluginReferenceForce)
