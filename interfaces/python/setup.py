from distutils.core import setup, Extension
import sys

EXTRA_COMPILE_ARGS = [
    '-O3',
    '-Wno-unused-label',
    '-Wno-unused-function',
]
EXTRA_LINK_ARGS = []

if sys.platform == 'darwin':
    EXTRA_COMPILE_ARGS.extend([
        '-stdlib=libc++',
        '-arch', 'x86_64',
        '-mmacosx-version-min=10.8',
    ])
    EXTRA_LINK_ARGS.extend(['-arch', 'x86_64', '-stdlib=libc++', '-mmacosx-version-min=10.8'])
elif sys.platform.startswith('linux'):
    pass

module1 = Extension('minizinc_internal',
                    sources = ['pyinterface.cpp'],
                    libraries = ['minizinc_gecode', 'minizinc',
                                 'gecodedriver',
                                 'gecodeminimodel',
                                 'gecodesearch',
                                 'gecodeset',
                                 'gecodefloat',
                                 'gecodeint',
                                 'gecodekernel',
                                 'gecodesupport'
                                 ],
                    extra_compile_args= EXTRA_COMPILE_ARGS + ['-fPIC'],
                    extra_link_args = EXTRA_LINK_ARGS
                    )

setup (name = 'MiniZinc',
       version = '2.0',
       description = 'A Python interface to the MiniZinc constraint modelling language',
       ext_modules = [module1])
