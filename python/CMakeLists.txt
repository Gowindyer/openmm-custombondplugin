set(WRAP_FILE CustomBondPluginWrapper.cpp)
set(MODULE_NAME custombondplugin)

add_custom_command(
    OUTPUT "${WRAP_FILE}"
    COMMAND "${SWIG_EXECUTABLE}"
        -python -c++
        -o "${WRAP_FILE}"
        "-I${OPENMM_DIR}/include"
        "-I${CMAKE_SOURCE_DIR}/openmmapi/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/custombondplugin.i"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/custombondplugin.i"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

add_custom_target(CustomBondPythonInstall DEPENDS "${WRAP_FILE}")
add_dependencies(CustomBondPythonInstall ${OPENMM_CUSTOMBOND_TARGET})
set(CUSTOMBONDPLUGIN_HEADER_DIR "${CMAKE_SOURCE_DIR}/openmmapi/include")
set(CUSTOMBONDPLUGIN_LIBRARY_DIR "${CMAKE_BINARY_DIR}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
add_custom_command(TARGET CustomBondPythonInstall POST_BUILD
    COMMAND "${PYTHON_EXECUTABLE}" -m pip install .
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

if(OPENMM_CUSTOMBOND_BUILD_TESTS AND BUILD_TESTING)
    add_subdirectory(tests)
endif()
