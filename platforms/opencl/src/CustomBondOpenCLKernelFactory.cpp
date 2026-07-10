/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondOpenCLKernelFactory.h"
#include "CommonCustomBondPluginKernels.h"
#include "openmm/opencl/OpenCLContext.h"
#include "openmm/opencl/OpenCLPlatform.h"
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
        Platform& platform = Platform::getPlatformByName("OpenCL");
        CustomBondOpenCLKernelFactory* factory = new CustomBondOpenCLKernelFactory();
        platform.registerKernelFactory(CalcCustomBondPluginForceKernel::Name(), factory);
    }
    catch (std::exception ex) {
        // Ignore.
    }
}

extern "C" OPENMM_EXPORT void registerCustomBondOpenCLKernelFactories() {
    try {
        Platform::getPlatformByName("OpenCL");
    }
    catch (...) {
        if (!OpenCLPlatform::isPlatformSupported())
            return;
        Platform::registerPlatform(new OpenCLPlatform());
    }
    registerKernelFactories();
}

KernelImpl* CustomBondOpenCLKernelFactory::createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const {
    OpenCLPlatform::PlatformData& data = *static_cast<OpenCLPlatform::PlatformData*>(context.getPlatformData());
    OpenCLContext& cl = *data.contexts[0];
    if (name == CalcCustomBondPluginForceKernel::Name()) {
        if (data.contexts.size() > 1)
            return new CommonParallelCalcCustomBondPluginForceKernel(name, platform, cl, context.getSystem());
        return new CommonCalcCustomBondPluginForceKernel(name, platform, cl, context.getSystem());
    }
    throw OpenMMException((std::string("Tried to create kernel with illegal kernel name '")+name+"'").c_str());
}
