set(OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME OpenMMCustomBondOpenCL)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CustomBondOpenCLKernelFactory.cpp
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernelSources.cpp
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernels.cpp
)
set(SOURCE_INCLUDE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/CustomBondOpenCLKernelFactory.h
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernelSources.h
    ${CMAKE_SOURCE_DIR}/platforms/common/src/CommonCustomBondPluginKernels.h
)

add_library(${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME} SHARED ${SOURCE_FILES} ${SOURCE_INCLUDE_FILES})
target_include_directories(${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/openmmapi/include
        ${CMAKE_SOURCE_DIR}/platforms/common/src
        ${OPENMM_INCLUDE_DIR}
        ${OPENMM_REFERENCE_INCLUDE_DIR}
        ${OPENMM_OPENCL_INCLUDE_DIR}
        ${OPENMM_COMMON_INCLUDE_DIR}
        ${OPENMM_ASMJIT_INCLUDE_DIR}
        ${OpenCL_INCLUDE_DIRS}
)
target_compile_definitions(${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME} PRIVATE OPENMM_BUILDING_SHARED_LIBRARY)
target_link_libraries(${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME}
    PRIVATE
        ${OPENMM_CUSTOMBOND_TARGET}
        ${OPENMM_LIBRARY}
        ${OPENMM_OPENCL_LIBRARY}
        ${OpenCL_LIBRARIES}
)
set_target_properties(${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME} PROPERTIES
    BUILD_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
    INSTALL_RPATH "${OPENMM_CUSTOMBOND_PLUGIN_RPATH}"
)
install(TARGETS ${OPENMMCUSTOMBONDOPENCL_LIBRARY_NAME}
    LIBRARY DESTINATION lib/plugins
    RUNTIME DESTINATION lib/plugins
    ARCHIVE DESTINATION lib/plugins
)

if(OPENMM_CUSTOMBOND_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
