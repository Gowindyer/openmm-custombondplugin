from distutils.core import setup
from distutils.extension import Extension
import os
import platform

openmm_dir = '@OPENMM_DIR@'
custombondplugin_header_dir = '@CUSTOMBONDPLUGIN_HEADER_DIR@'
custombondplugin_library_dir = '@CUSTOMBONDPLUGIN_LIBRARY_DIR@'

extra_compile_args = ['-std=c++11']
extra_link_args = []

if platform.system() == 'Darwin':
    extra_compile_args += ['-stdlib=libc++', '-mmacosx-version-min=10.7']
    extra_link_args += [
        '-stdlib=libc++',
        '-mmacosx-version-min=10.7',
        '-Wl,-rpath,' + os.path.join(openmm_dir, 'lib'),
        '-Wl,-rpath,' + custombondplugin_library_dir,
    ]

extension = Extension(
    name='_custombondplugin',
    sources=['CustomBondPluginWrapper.cpp'],
    libraries=['OpenMM', 'OpenMMCustomBond'],
    include_dirs=[os.path.join(openmm_dir, 'include'), custombondplugin_header_dir],
    library_dirs=[os.path.join(openmm_dir, 'lib'), custombondplugin_library_dir],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
)

setup(
    name='custombondplugin',
    version='1.0',
    py_modules=['custombondplugin'],
    ext_modules=[extension],
)
