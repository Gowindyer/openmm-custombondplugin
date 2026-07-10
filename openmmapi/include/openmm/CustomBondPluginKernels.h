#ifndef CUSTOMBOND_OPENMM_KERNELS_H_
#define CUSTOMBOND_OPENMM_KERNELS_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/CustomBondPluginForce.h"
#include "openmm/KernelImpl.h"
#include "openmm/Platform.h"
#include "openmm/System.h"
#include <string>

namespace OpenMM {

/**
 * This kernel is invoked by CustomBondPluginForce to calculate the forces acting on the system and the energy of the system.
 */
class CalcCustomBondPluginForceKernel : public KernelImpl {
public:
    static std::string Name() {
        return "CalcCustomBondPluginForce";
    }
    CalcCustomBondPluginForceKernel(std::string name, const Platform& platform) : KernelImpl(name, platform) {
    }
    virtual void initialize(const System& system, const CustomBondPluginForce& force) = 0;
    virtual double execute(ContextImpl& context, bool includeForces, bool includeEnergy) = 0;
    virtual void copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond) = 0;
};

} // namespace OpenMM

#endif /*CUSTOMBOND_OPENMM_KERNELS_H_*/
