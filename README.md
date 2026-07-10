# OpenMM CustomBond Plugin

This repository is a standalone, forked copy of OpenMM's `CustomBondForce`
implementation.  It exposes a separate API class, `CustomBondPluginForce`, and a
separate kernel name, `CalcCustomBondPluginForce`, so it can be modified without
changing OpenMM's built-in `CustomBondForce`.

The layout follows the same idea as `openmmexampleplugin`: the plugin builds
against an installed OpenMM prefix supplied by `OPENMM_DIR`.

By default this plugin compiles with `OPENMM_CUSTOMBOND_USE_LEPTON_JIT=ON`,
matching typical OpenMM builds on x86 and ARM.  The required asmjit headers are
vendored under `thirdparty/asmjit` because OpenMM's install tree does not
install them.

## Build on macOS

Use an OpenMM install that includes the Reference and CPU platform headers and
libraries.

```bash
cmake -S . -B build \
  -DOPENMM_DIR=/path/to/openmm/install \
  -DOPENMM_CUSTOMBOND_BUILD_REFERENCE=ON \
  -DOPENMM_CUSTOMBOND_BUILD_CPU=ON \
  -DOPENMM_CUSTOMBOND_BUILD_CUDA=OFF \
  -DOPENMM_CUSTOMBOND_BUILD_TESTS=ON

cmake --build build --parallel 4

ctest --test-dir build -R '^TestCustomBondPlugin(Reference|Cpu)Force$' --output-on-failure
```

## Build on a CUDA Machine

Use an OpenMM install that was built with the CUDA platform.  The CUDA toolkit,
NVRTC, and NVIDIA driver must be visible to CMake and the runtime linker.

```bash
cmake -S . -B build-cuda \
  -DOPENMM_DIR=/path/to/openmm-cuda/install \
  -DOPENMM_CUSTOMBOND_BUILD_REFERENCE=ON \
  -DOPENMM_CUSTOMBOND_BUILD_CPU=OFF \
  -DOPENMM_CUSTOMBOND_BUILD_CUDA=ON \
  -DOPENMM_CUSTOMBOND_BUILD_TESTS=ON

cmake --build build-cuda --target TestCustomBondPluginCudaForce --parallel 4

ctest --test-dir build-cuda -R '^TestCustomBondPluginCudaForce' --output-on-failure
```

For mixed and double precision CUDA tests, add:

```bash
-DOPENMM_CUSTOMBOND_BUILD_CUDA_DOUBLE_PRECISION_TESTS=ON
```

## Build on an OpenCL Machine

Use an OpenMM install that was built with the OpenCL platform.  The OpenCL
headers/runtime must be visible to CMake and the runtime linker.

```bash
cmake -S . -B build-opencl \
  -DOPENMM_DIR=/path/to/openmm-opencl/install \
  -DOPENMM_CUSTOMBOND_BUILD_REFERENCE=ON \
  -DOPENMM_CUSTOMBOND_BUILD_CPU=OFF \
  -DOPENMM_CUSTOMBOND_BUILD_CUDA=OFF \
  -DOPENMM_CUSTOMBOND_BUILD_OPENCL=ON \
  -DOPENMM_CUSTOMBOND_BUILD_TESTS=ON

cmake --build build-opencl --target TestCustomBondPluginOpenCLForce --parallel 4

ctest --test-dir build-opencl -R '^TestCustomBondPluginOpenCLForce' --output-on-failure
```

For mixed and double precision OpenCL tests, add:

```bash
-DOPENMM_CUSTOMBOND_BUILD_OPENCL_DOUBLE_PRECISION_TESTS=ON
```

## Install

```bash
cmake --install build --prefix /path/to/custombond/install
```

The main library is installed under `lib/`, and platform plugin libraries are
installed under `lib/plugins/`, matching OpenMM's plugin convention.

## Python Tests

The Python wrapper and pytest suite follow the same pattern as
`openmm-native-nonbonded-plugin`.  They require a Python environment with
`openmm`, `pytest`, `swig`, and the OpenMM SWIG header files available under the
selected `OPENMM_DIR`.

```bash
cmake -S . -B build-python \
  -DOPENMM_DIR=/path/to/openmm/install \
  -DOPENMM_CUSTOMBOND_BUILD_REFERENCE=ON \
  -DOPENMM_CUSTOMBOND_BUILD_CPU=ON \
  -DOPENMM_CUSTOMBOND_BUILD_OPENCL=ON \
  -DOPENMM_CUSTOMBOND_BUILD_TESTS=ON \
  -DOPENMM_CUSTOMBOND_BUILD_PYTHON_WRAPPERS=ON

cmake --build build-python --target CustomBondPythonTest --parallel 4
```

The CMake target sets `CUSTOMBOND_PLUGIN_DIR` and `CUSTOMBOND_TEST_PLATFORMS`
for the enabled platform plugins.  When running pytest manually, set those
variables yourself if the plugin libraries are not installed into OpenMM's
normal plugin directory.

## Notes

This code is derived from OpenMM source files.  Keep the per-file copyright and
license headers intact when modifying copied code.
