#ifndef OPENMM_CUSTOMBONDCUDAKERNELFACTORY_H_
#define OPENMM_CUSTOMBONDCUDAKERNELFACTORY_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/KernelFactory.h"

namespace OpenMM {

class CustomBondCudaKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMBONDCUDAKERNELFACTORY_H_*/
