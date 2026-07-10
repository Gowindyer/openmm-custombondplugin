#ifndef OPENMM_CUSTOMBONDREFERENCEKERNELS_H_
#define OPENMM_CUSTOMBONDREFERENCEKERNELS_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/CustomBondPluginKernels.h"
#include "lepton/CompiledExpression.h"
#include <string>
#include <vector>

namespace OpenMM {

class CustomBondReferenceBondIxn;

class CustomBondReferenceCalcCustomBondPluginForceKernel : public CalcCustomBondPluginForceKernel {
public:
    CustomBondReferenceCalcCustomBondPluginForceKernel(std::string name, const Platform& platform) :
            CalcCustomBondPluginForceKernel(name, platform), ixn(NULL) {
    }
    ~CustomBondReferenceCalcCustomBondPluginForceKernel();
    void initialize(const System& system, const CustomBondPluginForce& force);
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    void copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond);
private:
    int numBonds;
    CustomBondReferenceBondIxn* ixn;
    std::vector<std::vector<int> > bondIndexArray;
    std::vector<std::vector<double> > bondParamArray;
    Lepton::CompiledExpression energyExpression, forceExpression;
    std::vector<Lepton::CompiledExpression> energyParamDerivExpressions;
    std::vector<std::string> parameterNames, globalParameterNames, energyParamDerivNames;
    bool usePeriodic;
};

} // namespace OpenMM

#endif /*OPENMM_CUSTOMBONDREFERENCEKERNELS_H_*/
