#ifndef OPENMM_CUSTOMBONDOPENCLKERNELFACTORY_H_
#define OPENMM_CUSTOMBONDOPENCLKERNELFACTORY_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/KernelFactory.h"

namespace OpenMM {

class CustomBondOpenCLKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMBONDOPENCLKERNELFACTORY_H_*/
