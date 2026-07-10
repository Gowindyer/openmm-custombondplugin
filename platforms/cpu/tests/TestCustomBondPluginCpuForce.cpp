/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondCpuKernelFactory.h"
#include "openmm/cpu/CpuPlatform.h"
#include "openmm/CustomBondPluginKernels.h"
#include <cstdlib>
#include <iostream>

using namespace OpenMM;

CpuPlatform platform;

void initializeTests(int argc, char* argv[]) {
    if (!CpuPlatform::isProcessorSupported()) {
        std::cout << "CPU is not supported.  Exiting." << std::endl;
        exit(0);
    }
    platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), new CustomBondCpuKernelFactory());
}

#include "TestCustomBondPluginForce.h"

void runPlatformTests() {
}
