/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondReferenceKernelFactory.h"
#include "CustomBondReferenceKernels.h"
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
        if (platform.getName() == "Reference") {
            CustomBondReferenceKernelFactory* factory = new CustomBondReferenceKernelFactory();
            platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), factory);
        }
    }
}

extern "C" OPENMM_EXPORT void registerCustomBondReferenceKernelFactories() {
    registerKernelFactories();
}

KernelImpl* CustomBondReferenceKernelFactory::createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const {
    if (name == CalcCustomBondPluginForceKernel::Name())
        return new CustomBondReferenceCalcCustomBondPluginForceKernel(name, platform);
    throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
