file(GLOB test_files ${CMAKE_CURRENT_SOURCE_DIR}/Test*.py)
foreach(file ${test_files})
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(CUSTOMBOND_PYTHON_TEST_PLATFORMS)
if(OPENMM_CUSTOMBOND_BUILD_REFERENCE)
    list(APPEND CUSTOMBOND_PYTHON_TEST_PLATFORMS Reference)
endif()
if(OPENMM_CUSTOMBOND_BUILD_CPU)
    list(APPEND CUSTOMBOND_PYTHON_TEST_PLATFORMS CPU)
endif()
if(OPENMM_CUSTOMBOND_BUILD_CUDA)
    list(APPEND CUSTOMBOND_PYTHON_TEST_PLATFORMS CUDA)
endif()
if(OPENMM_CUSTOMBOND_BUILD_OPENCL)
    list(APPEND CUSTOMBOND_PYTHON_TEST_PLATFORMS OpenCL)
endif()
list(JOIN CUSTOMBOND_PYTHON_TEST_PLATFORMS "," CUSTOMBOND_PYTHON_TEST_PLATFORM_STRING)

add_custom_target(CustomBondPythonTest DEPENDS ${test_files})
add_dependencies(CustomBondPythonTest CustomBondPythonInstall)
if(TARGET OpenMMCustomBondReference)
    add_dependencies(CustomBondPythonTest OpenMMCustomBondReference)
endif()
if(TARGET OpenMMCustomBondCPU)
    add_dependencies(CustomBondPythonTest OpenMMCustomBondCPU)
endif()
if(TARGET OpenMMCustomBondCUDA)
    add_dependencies(CustomBondPythonTest OpenMMCustomBondCUDA)
endif()
if(TARGET OpenMMCustomBondOpenCL)
    add_dependencies(CustomBondPythonTest OpenMMCustomBondOpenCL)
endif()

add_custom_command(
    TARGET CustomBondPythonTest POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E env
            "CUSTOMBOND_PLUGIN_DIR=${CMAKE_BINARY_DIR}"
            "CUSTOMBOND_TEST_PLATFORMS=${CUSTOMBOND_PYTHON_TEST_PLATFORM_STRING}"
            "DYLD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:${OPENMM_DIR}/lib:${OPENMM_DIR}/lib/plugins:$ENV{DYLD_LIBRARY_PATH}"
            "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}:${OPENMM_DIR}/lib:${OPENMM_DIR}/lib/plugins:$ENV{LD_LIBRARY_PATH}"
            "${PYTHON_EXECUTABLE}" -m pytest --verbose Test*.py
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
