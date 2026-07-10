#ifndef OPENMM_WINDOWSEXPORTCUSTOMBOND_H_
#define OPENMM_WINDOWSEXPORTCUSTOMBOND_H_

#if defined(_WIN32)
    #if defined(OPENMM_CUSTOMBOND_BUILDING_SHARED_LIBRARY)
        #define OPENMM_EXPORT_CUSTOMBOND __declspec(dllexport)
    #elif defined(OPENMM_USE_STATIC_LIBRARIES)
        #define OPENMM_EXPORT_CUSTOMBOND
    #else
        #define OPENMM_EXPORT_CUSTOMBOND __declspec(dllimport)
    #endif
#else
    #define OPENMM_EXPORT_CUSTOMBOND
#endif

#endif /*OPENMM_WINDOWSEXPORTCUSTOMBOND_H_*/
