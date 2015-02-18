from distutils.core import setup, Extension

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
                    #depends = ['global.h',  'Model.h',  'Solution.h',  'Set.h',  'Variable.h', 
                    #           'global.cpp','Model.cpp','Solution.cpp','Set.cpp','Variable.cpp']
                    )

setup (name = 'MiniZinc',
       version = '2.0',
       description = 'A Python interface to the MiniZinc constraint modelling language',
       ext_modules = [module1])
