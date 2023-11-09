.. _ch-installation_detailed:

Installation from Source Code
=============================

The easiest way to install a complete version of MiniZinc is to use the binary packages we provide for several platforms, as explained in :ref:`ch-installation`. In the following, we describe how to install MiniZinc from source code.

All components of MiniZinc are free and open source software, and compilation should be straightforward if you have all the necessary build tools installed. However, third-party components, in particular the different solvers, may be more difficult to install correctly, and we cannot provide any support for these components.

The source code for MiniZinc can be downloaded from its GitHub repository at https://github.com/MiniZinc/libminizinc.
The source code for the MiniZinc IDE is available from https://github.com/MiniZinc/MiniZincIDE.
The ``master`` branch of each repository points to the latest release version, while the ``develop`` branch points to the current state of development (and may be buggy or not even compile sometimes).
Installation details can be found in each project's ``README.md`` files.

You will also need to install additional solvers to use with MiniZinc.
To get started, try Gecode (http://www.gecode.org) or Chuffed (https://github.com/chuffed/chuffed).
We don't cover installation instructions for these solvers here.
