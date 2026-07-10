%module custombondplugin

%import(module="openmm.openmm") "swig/OpenMMSwigHeaders.i"
%include "swig/typemaps.i"
%include <std_string.i>
%include <std_vector.i>

%template(DoubleVector) std::vector<double>;

%{
#include "openmm/CustomBondPluginForce.h"
#include "OpenMM.h"
#include "OpenMMAmoeba.h"
#include "OpenMMDrude.h"
#include "openmm/RPMDIntegrator.h"
#include "openmm/RPMDMonteCarloBarostat.h"
%}

%exception {
    try {
        $action
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        return NULL;
    }
}

%inline %{
int addCustomBondPluginForce(unsigned long long systemAddress, OpenMM::CustomBondPluginForce* force) {
    OpenMM::System* system = reinterpret_cast<OpenMM::System*>((uintptr_t) systemAddress);
    return system->addForce(force);
}
%}

%pythoncode %{
def addForce(system, force):
    index = addCustomBondPluginForce(int(getattr(system, "this", system)), force)
    force.thisown = False
    return index
%}

namespace OpenMM {

class CustomBondPluginForce : public Force {
public:
    explicit CustomBondPluginForce(const std::string& energy);
    int getNumBonds() const;
    int getNumPerBondParameters() const;
    int getNumGlobalParameters() const;
    int getNumEnergyParameterDerivatives() const;
    const std::string& getEnergyFunction() const;
    void setEnergyFunction(const std::string& energy);
    int addPerBondParameter(const std::string& name);
    const std::string& getPerBondParameterName(int index) const;
    void setPerBondParameterName(int index, const std::string& name);
    int addGlobalParameter(const std::string& name, double defaultValue);
    const std::string& getGlobalParameterName(int index) const;
    void setGlobalParameterName(int index, const std::string& name);
    double getGlobalParameterDefaultValue(int index) const;
    void setGlobalParameterDefaultValue(int index, double defaultValue);
    void addEnergyParameterDerivative(const std::string& name);
    const std::string& getEnergyParameterDerivativeName(int index) const;
    int addBond(int particle1, int particle2, const std::vector<double>& parameters=std::vector<double>());

    %apply int& OUTPUT {int& particle1};
    %apply int& OUTPUT {int& particle2};
    void getBondParameters(int index, int& particle1, int& particle2, std::vector<double>& parameters) const;
    %clear int& particle1;
    %clear int& particle2;

    void setBondParameters(int index, int particle1, int particle2, const std::vector<double>& parameters);
    void updateParametersInContext(Context& context);
    void setUsesPeriodicBoundaryConditions(bool periodic);
    bool usesPeriodicBoundaryConditions() const;

    %extend {
        void updateParametersInContextFromAddress(unsigned long long contextAddress) {
            OpenMM::Context* context = reinterpret_cast<OpenMM::Context*>((uintptr_t) contextAddress);
            $self->updateParametersInContext(*context);
        }

        static OpenMM::CustomBondPluginForce& cast(OpenMM::Force& force) {
            return dynamic_cast<OpenMM::CustomBondPluginForce&>(force);
        }

        static bool isinstance(OpenMM::Force& force) {
            return (dynamic_cast<OpenMM::CustomBondPluginForce*>(&force) != NULL);
        }
    }
};

}

%pythoncode %{
def _updateParametersInContext(self, context):
    return self.updateParametersInContextFromAddress(int(getattr(context, "this", context)))

CustomBondPluginForce.updateParametersInContext = _updateParametersInContext
%}
