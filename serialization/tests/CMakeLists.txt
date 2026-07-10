add_executable(TestSerializeCustomBondPluginForce TestSerializeCustomBondPluginForce.cpp)
target_include_directories(TestSerializeCustomBondPluginForce
    PRIVATE
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${CMAKE_SOURCE_DIR}/serialization/include
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
)
target_link_libraries(TestSerializeCustomBondPluginForce
    PRIVATE
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_LIBRARY}
)
add_test(NAME TestSerializeCustomBondPluginForce COMMAND TestSerializeCustomBondPluginForce)
