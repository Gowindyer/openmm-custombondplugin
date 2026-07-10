/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondCudaKernelFactory.h"
#include "CommonCustomBondPluginKernels.h"
#include "openmm/cuda/CudaContext.h"
#include "openmm/cuda/CudaPlatform.h"
#include "openmm/CustomBondPluginKernels.h"
#include "openmm/OpenMMException.h"
#include "openmm/Platform.h"
#include "openmm/internal/ContextImpl.h"
#include "openmm/internal/windowsExport.h"
#include <exception>

using namespace OpenMM;

extern "C" OPENMM_EXPORT void registerPlatforms() {
}

extern "C" OPENMM_EXPORT void registerKernelFactories() {
    try {
        Platform& platform = Platform::getPlatformByName("CUDA");
        CustomBondCudaKernelFactory* factory = new CustomBondCudaKernelFactory();
        platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), factory);
    }
    catch (std::exception ex) {
        // Ignore.
    }
}

extern "C" OPENMM_EXPORT void registerCustomBondCudaKernelFactories() {
    try {
        Platform::getPlatformByName("CUDA");
    }
    catch (...) {
        Platform::registerPlatform(new CudaPlatform());
    }
    registerKernelFactories();
}

KernelImpl* CustomBondCudaKernelFactory::createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const {
    CudaPlatform::PlatformData& data = *static_cast<CudaPlatform::PlatformData*>(context.getPlatformData());
    CudaContext& cu = *data.contexts[0];
    if (name == CalcCustomBondPluginForceKernel::Name()) {
        if (data.contexts.size() > 1)
            return new CommonParallelCalcCustomBondPluginForceKernel(name, platform, cu, context.getSystem());
        return new CommonCalcCustomBondPluginForceKernel(name, platform, cu, context.getSystem());
    }
    throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
