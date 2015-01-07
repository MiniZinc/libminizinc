from distutils.core import setup, Extension

module1 = Extension('minizinc',
                    sources = ['pyinterface.cpp'],
                    # libraries = ['gecode_interface', 'solver_interface', 'minizinc',
                    #              'gecodedriver',
                    #              'gecodeminimodel',
                    #              'gecodesearch',
                    #              'gecodeset',
                    #              'gecodefloat',
                    #              'gecodeint',
                    #              'gecodekernel',
                    #              'gecodesupport'
                    #              ]
                    )

setup (name = 'MiniZinc',
       version = '2.0',
       description = 'A Python interface to the MiniZinc constraint modelling language',
       ext_modules = [module1])
