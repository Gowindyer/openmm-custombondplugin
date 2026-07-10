import os

import pytest

plugin = pytest.importorskip("custombondplugin")
mm = pytest.importorskip("openmm")
from openmm import unit

TOL = 1.0e-5


def _requested_cases():
    requested = os.environ.get("CUSTOMBOND_TEST_PLATFORMS")
    if requested:
        platforms = [name for name in requested.split(",") if name]
    else:
        platforms = ["Reference", "CPU", "CUDA", "OpenCL"]

    cases = []
    for platform_name in platforms:
        if platform_name in ("CUDA", "OpenCL"):
            cases.append((platform_name, "single"))
            cases.append((platform_name, "mixed"))
            cases.append((platform_name, "double"))
        else:
            cases.append((platform_name, ""))
    return cases


cases = _requested_cases()
ids = [platform_name + precision for platform_name, precision in cases]


def _load_custombond_plugins():
    search_dirs = []
    for path in os.environ.get("CUSTOMBOND_PLUGIN_DIR", "").split(os.pathsep):
        if path:
            search_dirs.append(path)
    for path in os.environ.get("OPENMM_PLUGIN_DIR", "").split(os.pathsep):
        if path:
            search_dirs.append(path)

    seen = set()
    for path in search_dirs:
        path = os.path.abspath(path)
        if path in seen or not os.path.isdir(path):
            continue
        seen.add(path)
        try:
            mm.Platform.loadPluginsFromDirectory(path)
        except Exception:
            pass


def _platform(platform_name, precision):
    _load_custombond_plugins()
    try:
        platform = mm.Platform.getPlatformByName(platform_name)
    except Exception as exc:
        pytest.skip(f"{platform_name} platform is not available: {exc}")
    properties = {}
    if precision:
        properties["Precision"] = precision
    return platform, properties


def _double_vector(values):
    vector = plugin.DoubleVector()
    for value in values:
        vector.append(float(value))
    return vector


def _energy_value(energy):
    if unit.is_quantity(energy):
        return energy.value_in_unit(unit.kilojoule_per_mole)
    return energy


def _force_vec(force):
    if unit.is_quantity(force):
        return force.value_in_unit(unit.kilojoule_per_mole/unit.nanometer)
    return force


def _assert_equal_tol(expected, found, tol=TOL):
    expected_value = _energy_value(expected)
    found_value = _energy_value(found)
    scale = max(abs(expected_value), 1.0)
    assert abs(expected_value - found_value)/scale <= tol


def _assert_equal_vec(expected, found, tol=TOL):
    found = _force_vec(found)
    _assert_equal_tol(expected.x, found.x, tol)
    _assert_equal_tol(expected.y, found.y, tol)
    _assert_equal_tol(expected.z, found.z, tol)


def _create_context(system, platform_name, precision):
    platform, properties = _platform(platform_name, precision)
    integrator = mm.VerletIntegrator(0.01)
    try:
        return mm.Context(system, integrator, platform, properties)
    except mm.OpenMMException as exc:
        if platform_name in ("CUDA", "OpenCL") and "No compatible" in str(exc):
            label = f"{platform_name} {precision}" if precision else platform_name
            pytest.skip(f"{label} context is not available: {exc}")
        raise


def _add_force(system, force):
    return plugin.addForce(system, force)


@pytest.mark.parametrize("platform_name, precision", cases, ids=ids)
def test_bond_force(platform_name, precision):
    system = mm.System()
    system.addParticle(1.0)
    system.addParticle(1.0)
    system.addParticle(1.0)

    force = plugin.CustomBondPluginForce("scale*k*(r-r0)^2")
    force.addPerBondParameter("r0")
    force.addPerBondParameter("k")
    force.addGlobalParameter("scale", 0.5)
    force.addBond(0, 1, _double_vector([1.5, 0.8]))
    force.addBond(1, 2, _double_vector([1.2, 0.7]))
    _add_force(system, force)

    assert not force.usesPeriodicBoundaryConditions()
    assert not system.usesPeriodicBoundaryConditions()

    context = _create_context(system, platform_name, precision)
    context.setPositions([mm.Vec3(0, 2, 0), mm.Vec3(0, 0, 0), mm.Vec3(1, 0, 0)])
    state = context.getState(getForces=True, getEnergy=True)
    forces = state.getForces(asNumpy=False)

    _assert_equal_vec(mm.Vec3(0, -0.8*0.5, 0), forces[0])
    _assert_equal_vec(mm.Vec3(0.7*0.2, 0, 0), forces[2])
    _assert_equal_vec(
        mm.Vec3(-_force_vec(forces[0]).x-_force_vec(forces[2]).x,
                -_force_vec(forces[0]).y-_force_vec(forces[2]).y,
                -_force_vec(forces[0]).z-_force_vec(forces[2]).z),
        forces[1],
    )
    _assert_equal_tol(0.5*0.8*0.5*0.5 + 0.5*0.7*0.2*0.2, state.getPotentialEnergy())


@pytest.mark.parametrize("platform_name, precision", cases, ids=ids)
def test_update_parameters(platform_name, precision):
    system = mm.System()
    system.addParticle(1.0)
    system.addParticle(1.0)

    force = plugin.CustomBondPluginForce("k*(r-r0)^2")
    force.addPerBondParameter("k")
    force.addPerBondParameter("r0")
    force.addBond(0, 1, _double_vector([1.0, 1.5]))
    _add_force(system, force)

    context = _create_context(system, platform_name, precision)
    context.setPositions([mm.Vec3(0, 0, 0), mm.Vec3(2, 0, 0)])
    state = context.getState(getEnergy=True)
    _assert_equal_tol(0.25, state.getPotentialEnergy())

    force.setBondParameters(0, 0, 1, _double_vector([2.0, 1.5]))
    force.updateParametersInContext(context)
    state = context.getState(getEnergy=True)
    _assert_equal_tol(0.5, state.getPotentialEnergy())


@pytest.mark.parametrize("platform_name, precision", cases, ids=ids)
def test_periodic(platform_name, precision):
    system = mm.System()
    system.addParticle(1.0)
    system.addParticle(1.0)
    system.setDefaultPeriodicBoxVectors(mm.Vec3(3, 0, 0), mm.Vec3(0, 3, 0), mm.Vec3(0, 0, 3))

    force = plugin.CustomBondPluginForce("scale*k*(r-r0)^2")
    force.addPerBondParameter("r0")
    force.addPerBondParameter("k")
    force.addGlobalParameter("scale", 0.5)
    force.addBond(0, 1, _double_vector([1.9, 0.8]))
    force.setUsesPeriodicBoundaryConditions(True)
    _add_force(system, force)

    context = _create_context(system, platform_name, precision)
    context.setPositions([mm.Vec3(0, 2, 0), mm.Vec3(0, 0, 0)])
    state = context.getState(getForces=True, getEnergy=True)
    forces = state.getForces(asNumpy=False)

    _assert_equal_vec(mm.Vec3(0, -0.8*0.9, 0), forces[0])
    _assert_equal_vec(mm.Vec3(0, 0.8*0.9, 0), forces[1])
    _assert_equal_tol(0.5*0.8*0.9*0.9, state.getPotentialEnergy())


@pytest.mark.parametrize("platform_name, precision", cases, ids=ids)
def test_energy_parameter_derivatives(platform_name, precision):
    system = mm.System()
    system.addParticle(1.0)
    system.addParticle(1.0)
    system.addParticle(1.0)

    force = plugin.CustomBondPluginForce("k*(r-r0)^2")
    force.addGlobalParameter("r0", 0.0)
    force.addGlobalParameter("k", 0.0)
    force.addEnergyParameterDerivative("k")
    force.addEnergyParameterDerivative("r0")
    force.addBond(0, 1, _double_vector([]))
    force.addBond(1, 2, _double_vector([]))
    _add_force(system, force)

    context = _create_context(system, platform_name, precision)
    context.setPositions([mm.Vec3(0, 2, 0), mm.Vec3(0, 0, 0), mm.Vec3(1, 0, 0)])

    for i in range(10):
        r0 = 0.1*i
        k = 10-i
        context.setParameter("r0", r0)
        context.setParameter("k", k)
        state = context.getState(getParameterDerivatives=True)
        derivs = state.getEnergyParameterDerivatives()
        _assert_equal_tol(-2*k*((2-r0)+(1-r0)), derivs["r0"])
        _assert_equal_tol((2-r0)*(2-r0) + (1-r0)*(1-r0), derivs["k"])
