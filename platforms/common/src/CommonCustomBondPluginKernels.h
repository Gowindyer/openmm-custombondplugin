#ifndef OPENMM_COMMONCUSTOMBONDPLUGINKERNELS_H_
#define OPENMM_COMMONCUSTOMBONDPLUGINKERNELS_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "openmm/CustomBondPluginKernels.h"
#include "openmm/common/ComputeArray.h"
#include "openmm/common/ComputeContext.h"
#include "openmm/common/ComputeParameterSet.h"
#include <vector>

namespace OpenMM {

class CommonCalcCustomBondPluginForceKernel : public CalcCustomBondPluginForceKernel {
public:
    CommonCalcCustomBondPluginForceKernel(std::string name, const Platform& platform, ComputeContext& cc, const System& system) :
            CalcCustomBondPluginForceKernel(name, platform), hasInitializedKernel(false), cc(cc), system(system), params(NULL) {
    }
    ~CommonCalcCustomBondPluginForceKernel();
    void initialize(const System& system, const CustomBondPluginForce& force);
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    void copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond);
private:
    class ForceInfo;
    class GlobalParamPreComputation;
    int numBonds;
    bool hasInitializedKernel;
    ComputeContext& cc;
    ForceInfo* info;
    const System& system;
    ComputeParameterSet* params;
    ComputeArray globalParamValuesArray;
    std::vector<std::string> globalParamNames;
    std::vector<double> globalParamValues;
};

class CommonParallelCalcCustomBondPluginForceKernel : public CalcCustomBondPluginForceKernel {
public:
    CommonParallelCalcCustomBondPluginForceKernel(std::string name, const Platform& platform, ComputeContext& cc, const System& system);
    CommonCalcCustomBondPluginForceKernel& getKernel(int index) {
        return dynamic_cast<CommonCalcCustomBondPluginForceKernel&> (kernels[index].getImpl());
    }
    void initialize(const System& system, const CustomBondPluginForce& force);
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    void copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond);
private:
    class Task;
    ComputeContext& cc;
    std::vector<Kernel> kernels;
};

} // namespace OpenMM

#endif /*OPENMM_COMMONCUSTOMBONDPLUGINKERNELS_H_*/
