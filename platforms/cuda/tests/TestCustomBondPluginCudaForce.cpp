/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondCudaKernelFactory.h"
#include "openmm/cuda/CudaPlatform.h"
#include "openmm/CustomBondPluginKernels.h"
#include <string>

using namespace OpenMM;

CudaPlatform platform;

void initializeTests(int argc, char* argv[]) {
    if (argc > 1)
        platform.setPropertyDefaultValue("Precision", std::string(argv[1]));
    if (argc > 2)
        platform.setPropertyDefaultValue("DeviceIndex", std::string(argv[2]));
    platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), new CustomBondCudaKernelFactory());
}

#include "TestCustomBondPluginForce.h"

void runPlatformTests() {
    testParallelComputation();
}
