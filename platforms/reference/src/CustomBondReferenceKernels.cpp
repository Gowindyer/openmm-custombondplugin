/* -------------------------------------------------------------------------- *
 *                              OpenMMCustomBond                              *
 * -------------------------------------------------------------------------- */

#include "CustomBondReferenceKernels.h"
#include "openmm/reference/ReferenceForce.h"
#include "openmm/reference/ReferencePlatform.h"
#include "lepton/Operation.h"
#include "lepton/ParsedExpression.h"
#include "lepton/Parser.h"
#include "openmm/CustomBondPluginForce.h"
#include "openmm/OpenMMException.h"
#include "openmm/internal/CompiledExpressionSet.h"
#include "openmm/internal/ContextImpl.h"
#include <map>
#include <set>

using namespace OpenMM;
using namespace std;

static vector<Vec3>& extractPositions(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *data->positions;
}

static vector<Vec3>& extractForces(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *data->forces;
}

static Vec3* extractBoxVectors(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return data->periodicBoxVectors;
}

static map<string, double>& extractEnergyParameterDerivatives(ContextImpl& context) {
    ReferencePlatform::PlatformData* data = reinterpret_cast<ReferencePlatform::PlatformData*>(context.getPlatformData());
    return *data->energyParameterDerivatives;
}

static void validateVariables(const Lepton::ExpressionTreeNode& node, const set<string>& variables) {
    const Lepton::Operation& op = node.getOperation();
    if (op.getId() == Lepton::Operation::VARIABLE && variables.find(op.getName()) == variables.end())
        throw OpenMMException("Unknown variable in expression: "+op.getName());
    for (auto& child : node.getChildren())
        validateVariables(child, variables);
}

namespace OpenMM {

class CustomBondReferenceBondIxn {
public:
    CustomBondReferenceBondIxn(const Lepton::CompiledExpression& energyExpression,
            const Lepton::CompiledExpression& forceExpression, const vector<string>& parameterNames,
            const vector<Lepton::CompiledExpression>& energyParamDerivExpressions) :
            energyExpression(energyExpression), forceExpression(forceExpression),
            energyParamDerivExpressions(energyParamDerivExpressions), usePeriodic(false) {
        expressionSet.registerExpression(this->energyExpression);
        expressionSet.registerExpression(this->forceExpression);
        for (int i = 0; i < this->energyParamDerivExpressions.size(); i++)
            expressionSet.registerExpression(this->energyParamDerivExpressions[i]);
        rIndex = expressionSet.getVariableIndex("r");
        numParameters = parameterNames.size();
        for (auto& param : parameterNames)
            bondParamIndex.push_back(expressionSet.getVariableIndex(param));
    }
    void setPeriodic(Vec3* vectors) {
        usePeriodic = true;
        boxVectors[0] = vectors[0];
        boxVectors[1] = vectors[1];
        boxVectors[2] = vectors[2];
    }
    void setGlobalParameters(const map<string, double>& parameters) {
        for (auto& param : parameters)
            expressionSet.setVariable(expressionSet.getVariableIndex(param.first), param.second);
    }
    void calculateBondIxn(vector<int>& atomIndices, vector<Vec3>& atomCoordinates,
                          vector<double>& parameters, vector<Vec3>& forces,
                          double* totalEnergy, double* energyParamDerivs) {
        double deltaR[ReferenceForce::LastDeltaRIndex];
        for (int i = 0; i < numParameters; i++)
            expressionSet.setVariable(bondParamIndex[i], parameters[i]);

        int atomAIndex = atomIndices[0];
        int atomBIndex = atomIndices[1];
        if (usePeriodic)
            ReferenceForce::getDeltaRPeriodic(atomCoordinates[atomAIndex], atomCoordinates[atomBIndex], boxVectors, deltaR);
        else
            ReferenceForce::getDeltaR(atomCoordinates[atomAIndex], atomCoordinates[atomBIndex], deltaR);

        expressionSet.setVariable(rIndex, deltaR[ReferenceForce::RIndex]);
        double dEdR = forceExpression.evaluate();
        dEdR = deltaR[ReferenceForce::RIndex] > 0 ? dEdR/deltaR[ReferenceForce::RIndex] : 0;

        forces[atomAIndex][0] += dEdR*deltaR[ReferenceForce::XIndex];
        forces[atomAIndex][1] += dEdR*deltaR[ReferenceForce::YIndex];
        forces[atomAIndex][2] += dEdR*deltaR[ReferenceForce::ZIndex];

        forces[atomBIndex][0] -= dEdR*deltaR[ReferenceForce::XIndex];
        forces[atomBIndex][1] -= dEdR*deltaR[ReferenceForce::YIndex];
        forces[atomBIndex][2] -= dEdR*deltaR[ReferenceForce::ZIndex];

        for (int i = 0; i < energyParamDerivExpressions.size(); i++)
            energyParamDerivs[i] += energyParamDerivExpressions[i].evaluate();
        if (totalEnergy != NULL)
            *totalEnergy += energyExpression.evaluate();
    }
private:
    Lepton::CompiledExpression energyExpression;
    Lepton::CompiledExpression forceExpression;
    vector<Lepton::CompiledExpression> energyParamDerivExpressions;
    CompiledExpressionSet expressionSet;
    vector<int> bondParamIndex;
    int rIndex;
    int numParameters;
    bool usePeriodic;
    Vec3 boxVectors[3];
};

CustomBondReferenceCalcCustomBondPluginForceKernel::~CustomBondReferenceCalcCustomBondPluginForceKernel() {
    if (ixn != NULL)
        delete ixn;
}

void CustomBondReferenceCalcCustomBondPluginForceKernel::initialize(const System& system, const CustomBondPluginForce& force) {
    numBonds = force.getNumBonds();
    int numParameters = force.getNumPerBondParameters();
    usePeriodic = force.usesPeriodicBoundaryConditions();

    bondIndexArray.resize(numBonds, vector<int>(2));
    bondParamArray.resize(numBonds, vector<double>(numParameters));
    vector<double> params;
    for (int i = 0; i < numBonds; ++i) {
        int particle1, particle2;
        force.getBondParameters(i, particle1, particle2, params);
        bondIndexArray[i][0] = particle1;
        bondIndexArray[i][1] = particle2;
        for (int j = 0; j < numParameters; j++)
            bondParamArray[i][j] = params[j];
    }

    Lepton::ParsedExpression expression = Lepton::Parser::parse(force.getEnergyFunction()).optimize();
    energyExpression = expression.createCompiledExpression();
    forceExpression = expression.differentiate("r").createCompiledExpression();
    for (int i = 0; i < numParameters; i++)
        parameterNames.push_back(force.getPerBondParameterName(i));
    for (int i = 0; i < force.getNumGlobalParameters(); i++)
        globalParameterNames.push_back(force.getGlobalParameterName(i));
    for (int i = 0; i < force.getNumEnergyParameterDerivatives(); i++) {
        string param = force.getEnergyParameterDerivativeName(i);
        energyParamDerivNames.push_back(param);
        energyParamDerivExpressions.push_back(expression.differentiate(param).createCompiledExpression());
    }
    set<string> variables;
    variables.insert("r");
    variables.insert(parameterNames.begin(), parameterNames.end());
    variables.insert(globalParameterNames.begin(), globalParameterNames.end());
    validateVariables(expression.getRootNode(), variables);
    ixn = new CustomBondReferenceBondIxn(energyExpression, forceExpression, parameterNames, energyParamDerivExpressions);
}

double CustomBondReferenceCalcCustomBondPluginForceKernel::execute(ContextImpl& context, bool includeForces, bool includeEnergy) {
    vector<Vec3>& posData = extractPositions(context);
    vector<Vec3>& forceData = extractForces(context);
    double energy = 0;
    map<string, double> globalParameters;
    for (auto& name : globalParameterNames)
        globalParameters[name] = context.getParameter(name);
    ixn->setGlobalParameters(globalParameters);
    if (usePeriodic)
        ixn->setPeriodic(extractBoxVectors(context));
    vector<double> energyParamDerivValues(energyParamDerivNames.size()+1, 0.0);
    for (int i = 0; i < numBonds; i++)
        ixn->calculateBondIxn(bondIndexArray[i], posData, bondParamArray[i], forceData, includeEnergy ? &energy : NULL, &energyParamDerivValues[0]);
    map<string, double>& energyParamDerivs = extractEnergyParameterDerivatives(context);
    for (int i = 0; i < energyParamDerivNames.size(); i++)
        energyParamDerivs[energyParamDerivNames[i]] += energyParamDerivValues[i];
    return energy;
}

void CustomBondReferenceCalcCustomBondPluginForceKernel::copyParametersToContext(ContextImpl& context, const CustomBondPluginForce& force, int firstBond, int lastBond) {
    if (numBonds != force.getNumBonds())
        throw OpenMMException("updateParametersInContext: The number of bonds has changed");

    int numParameters = force.getNumPerBondParameters();
    vector<double> params;
    for (int i = firstBond; i <= lastBond; ++i) {
        int particle1, particle2;
        force.getBondParameters(i, particle1, particle2, params);
        if (particle1 != bondIndexArray[i][0] || particle2 != bondIndexArray[i][1])
            throw OpenMMException("updateParametersInContext: The set of particles in a bond has changed");
        for (int j = 0; j < numParameters; j++)
            bondParamArray[i][j] = params[j];
    }
}

} // namespace OpenMM
