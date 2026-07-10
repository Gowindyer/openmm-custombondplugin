#ifndef OPENMM_CUSTOMBONDCPUKERNELFACTORY_H_
#define OPENMM_CUSTOMBONDCPUKERNELFACTORY_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/KernelFactory.h"

namespace OpenMM {

class CustomBondCpuKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMBONDCPUKERNELFACTORY_H_*/
