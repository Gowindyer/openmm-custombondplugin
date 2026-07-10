/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondCpuKernelFactory.h"
#include "CustomBondReferenceKernels.h"
#include "openmm/cpu/CpuPlatform.h"
#include "openmm/CustomBondPluginKernels.h"
#include "openmm/OpenMMException.h"
#include "openmm/Platform.h"
#include "openmm/internal/ContextImpl.h"

using namespace OpenMM;

#ifdef OPENMM_BUILDING_STATIC_LIBRARY
static void registerPlatforms() {
#else
extern "C" OPENMM_EXPORT void registerPlatforms() {
#endif
}

#ifdef OPENMM_BUILDING_STATIC_LIBRARY
static void registerKernelFactories() {
#else
extern "C" OPENMM_EXPORT void registerKernelFactories() {
#endif
    for (int i = 0; i < Platform::getNumPlatforms(); i++) {
        Platform& platform = Platform::getPlatform(i);
        if (dynamic_cast<CpuPlatform*>(&platform) != NULL) {
            CustomBondCpuKernelFactory* factory = new CustomBondCpuKernelFactory();
            platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), factory);
        }
    }
}

extern "C" OPENMM_EXPORT void registerCustomBondCpuKernelFactories() {
    registerKernelFactories();
}

KernelImpl* CustomBondCpuKernelFactory::createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const {
    if (name == CalcCustomBondPluginForceKernel::Name())
        return new CustomBondReferenceCalcCustomBondPluginForceKernel(name, platform);
    throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
