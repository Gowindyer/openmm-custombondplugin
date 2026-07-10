/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondOpenCLKernelFactory.h"
#include "openmm/opencl/OpenCLPlatform.h"
#include "openmm/CustomBondPluginKernels.h"
#include <string>

using namespace OpenMM;

OpenCLPlatform platform;

void initializeTests(int argc, char* argv[]) {
    if (argc > 1)
        platform.setPropertyDefaultValue("Precision", std::string(argv[1]));
    if (argc > 2)
        platform.setPropertyDefaultValue("OpenCLPlatformIndex", std::string(argv[2]));
    if (argc > 3)
        platform.setPropertyDefaultValue("DeviceIndex", std::string(argv[3]));
    platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), new CustomBondOpenCLKernelFactory());
}

#include "TestCustomBondPluginForce.h"

void runPlatformTests() {
    testParallelComputation();
}
