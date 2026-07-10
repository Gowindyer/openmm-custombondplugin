/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondReferenceKernelFactory.h"
#include "openmm/reference/ReferencePlatform.h"
#include "openmm/CustomBondPluginKernels.h"

using namespace OpenMM;

ReferencePlatform platform;

void initializeTests(int argc, char* argv[]) {
    platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), new CustomBondReferenceKernelFactory());
}

#include "TestCustomBondPluginForce.h"

void runPlatformTests() {
}
