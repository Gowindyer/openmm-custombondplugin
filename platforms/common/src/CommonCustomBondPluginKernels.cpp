/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CommonCustomBondPluginKernels.h"
#include "CommonCustomBondPluginKernelSources.h"
#include "openmm/CustomBondPluginForce.h"
#include "openmm/OpenMMException.h"
#include "openmm/common/ComputeForceInfo.h"
#include "openmm/common/ComputeParameterInfo.h"
#include "openmm/common/ContextSelector.h"
#include "openmm/common/ExpressionUtilities.h"
#include "lepton/ParsedExpression.h"
#include "lepton/Parser.h"
#include <algorithm>
#include <map>
#include <sstream>

using namespace OpenMM;
using namespace std;

class CommonCalcCustomBondPluginForceKernel::ForceInfo : public ComputeForceInfo {
public:
    ForceInfo(const CustomBondPluginForce& force) : force(force) {
    }
    int getNumParticleGroups() {
        return force.getNumBonds();
    }
    void getParticlesInGroup(int index, vector<int>& particles) {
        int particle1, particle2;
        thread_local static vector<double> parameters;
        force.getBondParameters(index, particle1, particle2, parameters);
        particles.resize(2);
        particles[0] = particle1;
        particles[1] = particle2;
    }
    bool areGroupsIdentical(int group1, int group2) {
        int particle1, particle2;
        thread_local static vector<double> parameters1, parameters2;
        force.getBondParameters(group1, particle1, particle2, parameters1);
        force.getBondParameters(group2, particle1, particle2, parameters2);
        for (int i = 0; i < (int) parameters1.size(); i++)
            if (parameters1[i] != parameters2[i])
                return false;
        return true;
    }
private:
    const CustomBondPluginForce& force;
};

CommonCalcCustomBondPluginForceKernel::~CommonCalcCustomBondPluginForceKernel() {
    ContextSelector selector(cc);
    if (params != NULL)
        delete params;
}

void CommonCalcCustomBondPluginForceKernel::initialize(const System& system, const CustomBondPluginForce& force) {
    ContextSelector selector(cc);
    int numContexts = cc.getNumContexts();
    int startIndex = cc.getContextIndex()*force.getNumBonds()/numContexts;
    int endIndex = (cc.getContextIndex()+1)*force.getNumBonds()/numContexts;
    numBonds = endIndex-startIndex;
    if (numBonds == 0)
        return;
    vector<vector<int> > atoms(numBonds, vector<int>(2));
    params = new ComputeParameterSet(cc, force.getNumPerBondParameters(), numBonds, "customBondPluginParams");
    vector<vector<double> > paramVector(numBonds);
    for (int i = 0; i < numBonds; i++)
        force.getBondParameters(startIndex+i, atoms[i][0], atoms[i][1], paramVector[i]);
    params->setParameterValues(paramVector, true);
    info = new ForceInfo(force);
    cc.addForce(info);

    Lepton::ParsedExpression energyExpression = Lepton::Parser::parse(force.getEnergyFunction()).optimize();
    Lepton::ParsedExpression forceExpression = energyExpression.differentiate("r").optimize();
    map<string, Lepton::ParsedExpression> expressions;
    expressions["energy += "] = energyExpression;
    expressions["real dEdR = "] = forceExpression;

    map<string, string> variables;
    variables["r"] = "r";
    for (int i = 0; i < force.getNumPerBondParameters(); i++) {
        const string& name = force.getPerBondParameterName(i);
        variables[name] = "bondParams"+params->getParameterSuffix(i);
    }
    if (force.getNumGlobalParameters() > 0) {
        string argName = cc.getBondedUtilities().addArgument(cc.getGlobalParamValues(), "real");
        for (int i = 0; i < force.getNumGlobalParameters(); i++) {
            const string& name = force.getGlobalParameterName(i);
            int index = cc.registerGlobalParam(name);
            string value = argName+"["+cc.intToString(index)+"]";
            variables[name] = value;
        }
    }
    for (int i = 0; i < force.getNumEnergyParameterDerivatives(); i++) {
        string paramName = force.getEnergyParameterDerivativeName(i);
        string derivVariable = cc.getBondedUtilities().addEnergyParameterDerivative(paramName);
        Lepton::ParsedExpression derivExpression = energyExpression.differentiate(paramName).optimize();
        expressions[derivVariable+" += "] = derivExpression;
    }
    stringstream compute;
    for (int i = 0; i < (int) params->getParameterInfos().size(); i++) {
        ComputeParameterInfo& parameter = params->getParameterInfos()[i];
        string argName = cc.getBondedUtilities().addArgument(parameter.getArray(), parameter.getType());
        compute << parameter.getType() << " bondParams" << (i+1) << " = " << argName << "[index];\n";
    }
    vector<const TabulatedFunction*> functions;
    vector<pair<string, string> > functionNames;
    compute << cc.getExpressionUtilities().createExpressions(expressions, variables, functions, functionNames, "temp");
    map<string, string> replacements;
    replacements["APPLY_PERIODIC"] = (force.usesPeriodicBoundaryConditions() ? "1" : "0");
    replacements["COMPUTE_FORCE"] = compute.str();
    cc.getBondedUtilities().addInteraction(atoms, cc.replaceStrings(CommonCustomBondPluginKernelSources::bondForce, replacements), force.getForceGroup());
}

double CommonCalcCustomBondPluginForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {
    return 0.0;
}

void CommonCalcCustomBondPluginForceKernel::copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond) {
    ContextSelector selector(cc);
    int numContexts = cc.getNumContexts();
    int startIndex = cc.getContextIndex()*force.getNumBonds()/numContexts;
    int endIndex = (cc.getContextIndex()+1)*force.getNumBonds()/numContexts;
    if (numBonds != endIndex-startIndex)
        throw OpenMMException("updateParametersInContext: The number of bonds has changed");
    if (numBonds == 0 || firstBond >= endIndex || lastBond < startIndex || firstBond > lastBond)
        return;
    firstBond = max(firstBond, startIndex);
    lastBond = min(lastBond, endIndex-1);

    int numToSet = lastBond-firstBond+1;
    vector<vector<double> > paramVector(numToSet);
    int atom1, atom2;
    for (int i = 0; i < numToSet; i++)
        force.getBondParameters(firstBond+i, atom1, atom2, paramVector[i]);
    params->setParameterValuesSubset(firstBond-startIndex, paramVector, true);
    cc.invalidateMolecules(info, false, true);
}

class CommonParallelCalcCustomBondPluginForceKernel::Task : public ComputeContext::WorkTask {
public:
    Task(ContextImpl& context, CommonCalcCustomBondPluginForceKernel& kernel, bool includeForce,
            bool includeEnergy, double& energy) : context(context), kernel(kernel),
            includeForce(includeForce), includeEnergy(includeEnergy), energy(energy) {
    }
    void execute() {
        energy += kernel.execute(context, includeForce, includeEnergy);
    }
private:
    ContextImpl& context;
    CommonCalcCustomBondPluginForceKernel& kernel;
    bool includeForce, includeEnergy;
    double& energy;
};

CommonParallelCalcCustomBondPluginForceKernel::CommonParallelCalcCustomBondPluginForceKernel(std::string name, const Platform& platform, ComputeContext& cc, const System& system) :
        CalcCustomBondPluginForceKernel(name, platform), cc(cc) {
    for (ComputeContext* context : cc.getAllContexts())
        kernels.push_back(Kernel(new CommonCalcCustomBondPluginForceKernel(name, platform, *context, system)));
}

void CommonParallelCalcCustomBondPluginForceKernel::initialize(const System& system, const CustomBondPluginForce& force) {
    for (int i = 0; i < (int) kernels.size(); i++)
        getKernel(i).initialize(system, force);
}

double CommonParallelCalcCustomBondPluginForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {
    for (int i = 0; i < cc.getNumContexts(); i++) {
        ComputeContext::WorkThread& thread = cc.getAllContexts()[i]->getWorkThread();
        thread.addTask(new Task(context, getKernel(i), includeForces, includeEnergy, cc.getEnergyWorkspace()));
    }
    return 0.0;
}

void CommonParallelCalcCustomBondPluginForceKernel::copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond) {
    for (int i = 0; i < (int) kernels.size(); i++)
        getKernel(i).copyParametersToContext(context, force, firstBond, lastBond);
}
