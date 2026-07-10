set(OPENMMCUSTOMBONDCUDA_LIBRARY_NAME OpenMMCustomBondCUDA)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CustomBondCudaKernelFactory.cpp
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernelSources.cpp
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernels.cpp
)
set(SOURCE_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CustomBondCudaKernelFactory.h
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernelSources.h
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernels.h
)

add_library(${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME} SHARED ${SOURCE_FILES} ${SOURCE_INCLUDE_FILES})
target_include_directories(${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${CMAKE_SOURCE_DIR}/platforms/common/src
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_CUDA_INCLUDE_DIR}
        ${OPENMM_COMMON_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
        ${CUDAToolkit_INCLUDE_DIRS}
)
target_compile_definitions(${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME} PRIVATE OPENMM_BUILDING_SHARED_LIBRARY)
target_link_libraries(${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME}
    PRIVATE
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_LIBRARY}
        ${OPENMM_CUDA_LIBRARY}
)
set_target_properties(${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME} PROPERTIES
    BUILD_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
    INSTALL_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
)
install(TARGETS ${OPENMMCUSTOMBONDCUDA_LIBRARY_NAME}
    LIBRARY DESTINATION lib/plugins
    RUNTIME DESTINATION lib/plugins
    ARCHIVE DESTINATION lib/plugins
)

if(OPENMM_CUSTOMBOND_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
