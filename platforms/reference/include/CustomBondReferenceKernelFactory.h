#ifndef OPENMM_CUSTOMBONDREFERENCEKERNELFACTORY_H_
#define OPENMM_CUSTOMBONDREFERENCEKERNELFACTORY_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/KernelFactory.h"

namespace OpenMM {

class CustomBondReferenceKernelFactory : public KernelFactory {
public:
    KernelImpl* createKernelImpl(std::string name, const Platform& platform, ContextImpl& context) const;
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMBONDREFERENCEKERNELFACTORY_H_*/
