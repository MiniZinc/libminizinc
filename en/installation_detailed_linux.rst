.. _ch-installation_detailed_linux:

Installation Instructions for Linux and macOS
---------------------------------------------

These instructions should work for most Linux distributions.

MiniZinc
~~~~~~~~

Required development tools:

- CMake, version 3.4.0 or later
- On Linux: A recent C++ compiler (g++ or clang)
- On macOS: The Xcode developer tools

Before compiling MiniZinc, consider whether you want to install additional solvers. In particular, MIP solvers such as CBC, SCIP, Gurobi, IBM ILOG CPLEX, and XPRESS need to be installed prior to compiling MiniZinc, so that the build system can find them. If you want to use the solution checking functionality or extended presolving, you will also need to install Gecode before compiling MiniZinc. See below for installation instructions for these solvers.

To install MiniZinc, unpack the source code (or clone the git repository). Open a terminal and change into the source code directory. The following sequence of commands will build the MiniZinc compiler tool chain:

.. code-block:: bash

  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build .

MiniZinc IDE
~~~~~~~~~~~~

Required development tools:

- A recent C++ compiler (g++ or clang)
- Qt toolkit, version 5.9 or later (http://qt.io)

Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory for the MiniZinc IDE. Then use the following commands to compile:

.. code-block:: bash

  mkdir build
  cd build
  qmake ../MiniZincIDE
  make

Gecode
~~~~~~

You can obtain the Gecode source code from GitHub (https://github.com/gecode/gecode). You can find detailed compilation instructions for Gecode on its web site (https://www.gecode.org). In short, to compile from source, run the following in a terminal (from within the Gecode source code directory):

.. code-block:: bash

  mkdir build
  cd build
  ../configure
  make -j8
  sudo make install
  
which compiles Gecode and installs it in the standard location. You can choose a different installation directory using ``../configure --prefix=<INSTALLATION_PREFIX>``. In that case you can run ``make install`` without ``sudo``.

After installing Gecode, you can compile MiniZinc with built-in support for Gecode, which enables extended pre-solving and solution checking. In order for the MiniZinc compilation to find your Gecode installation, Gecode either has to be in the default location (such as ``/usr/local/include`` etc.), or you have to use the option ``-DGECODE_ROOT=<INSTALLATION_PREFIX>`` when calling ``cmake``.

In order to use Gecode as a *solver* with MiniZinc (as opposed to an internal pre-solving tool), you have to create an appropriate solver configuration file. Add a file ``gecode.msc`` in an appropriate location (see :ref:`sec-cmdline-conffiles`) containing the following, where you replace ``<INSTALLATION_PREFIX>`` with the actual installation path and update the version number if necessary:

.. code-block:: json

 {
  "id": "org.gecode.gecode",
  "name": "Gecode",
  "description": "Gecode FlatZinc executable",
  "version": "6.2.0",
  "mznlib": "<INSTALLATION_PREFIX>/share/gecode/mznlib",
  "executable": "<INSTALLATION_PREFIX>/bin/fzn-gecode",
  "tags": ["cp","int", "float", "set", "restart"],
  "stdFlags": ["-a","-f","-n","-p","-r","-s","-t"],
  "supportsMzn": false,
  "supportsFzn": true,
  "needsSolns2Out": true,
  "needsMznExecutable": false,
  "needsStdlibDir": false,
  "isGUIApplication": false
 }


OR Tools
~~~~~~~~

You can install the OR-Tools FlatZinc module as binary or obtain the source code from GitHub (https://github.com/google/or-tools).
You can find detailed compilation instructions for OR-Tools on https://developers.google.com/optimization/.
To compile the FlatZinc module from source, run the following in a terminal (from within the OR-Tools source code directory):

.. code-block:: bash

  make fz -j8
  make test_fz

In order to use OR-Tools with MiniZinc, you have to create an appropriate solver configuration file.
Add a file ``ortools.msc`` in an appropriate location (see :ref:`sec-cmdline-conffiles`) containing the following,
where you replace ``<INSTALLATION_PREFIX>`` with the actual installation path and update the version number if necessary:

.. code-block:: json

 {
  "id": "org.ortools.ortools",
  "name": "OR Tools",
  "description": "Or Tools FlatZinc executable",
  "version": "7.0/stable",
  "mznlib": "<INSTALLATION_PREFIX>/ortools/flatzinc/mznlib_sat",
  "executable": "<INSTALLATION_PREFIX>/bin/fz",
  "tags": ["cp","int", "lcg", "or-tools"],
  "stdFlags": ["-a","-n","-p"],
  "supportsMzn": false,
  "supportsFzn": true,
  "needsSolns2Out": true,
  "needsMznExecutable": false,
  "needsStdlibDir": false,
  "isGUIApplication": false
 }


Chuffed
~~~~~~~

You can obtain Chuffed's source from https://github.com/chuffed/chuffed. You can compile and install it using the following commands:

.. code-block:: bash

  mkdir build
  cd build
  cmake ..
  cmake --build . -- -j8
  sudo cmake --build . --target install

This will install Chuffed in the default location. Alternatively, you can choose a different installation directory by calling ``cmake .. -DCMAKE_INSTALL_PREFIX=<INSTALLATION_PREFIX>`` before ``cmake --build``. In that case, you may be able to install without using ``sudo``.

In order for MiniZinc to recognise the Chuffed solver, add a configuration file ``chuffed.msc`` to an appropriate location (see :ref:`sec-cmdline-conffiles`) with the following content, where you replace ``<INSTALLATION_PREFIX>`` with the actual installation path and update the version number if necessary:

.. code-block:: json

 {
  "id": "org.chuffed.chuffed",
  "name": "Chuffed",
  "description": "Chuffed FlatZinc executable",
  "version": "0.9",
  "mznlib": "<INSTALLATION_PREFIX>/share/chuffed/mznlib",
  "executable": "<INSTALLATION_PREFIX>/bin/fzn-chuffed",
  "tags": ["cp","lcg","int"],
  "stdFlags": ["-a","-f","-n","-r","-s","-t","-v"],
  "supportsMzn": false,
  "supportsFzn": true,
  "needsSolns2Out": true,
  "needsMznExecutable": false,
  "needsStdlibDir": false,
  "isGUIApplication": false
 }


COIN-OR CBC
~~~~~~~~~~~

CBC is an open-source Mixed Integer Programming solver. You can find out more about it at https://github.com/coin-or/Cbc. MiniZinc contains a built-in interface to CBC, so in order to use it you have to install CBC *before* compiling MiniZinc.

These instructions apply to CBC versions 2.10/stable or newer.

To download and compile CBC, run the following:

.. code-block:: bash

  svn checkout https://projects.coin-or.org/svn/Cbc/stable/2.10/ Cbc-stable
  cd Cbc-stable
  ./configure --enable-cbc-parallel
  make
  sudo make install

This will install CBC in the default location. You can choose a different installation directory using ``../configure --enable-cbc-parallel --prefix=<INSTALLATION_PREFIX>``. In that case you can run ``make install`` without ``sudo``.

The MiniZinc build system should find CBC automatically if it is installed in the default location. You can use the command line flag ``-DOSICBC_ROOT=<INSTALLATION_PREFIX>`` when running ``cmake`` for MiniZinc if you installed CBC in a different location.

SCIP
~~~~

For *SCIP (as of 6.0.1.0)*, if you download the Optimization Suite, the installation commands should be as follows.

1. Download the SCIP Optimization Suite 6.0.1 (or higher) source code: https://scip.zib.de/download.php?fname=scipoptsuite-6.0.1.tgz

2. Untar it and change directories into scipoptsuite-6.0.1

3. create a build directory and change directories there

4. Execute 

.. code-block:: bash

    cmake .. -DCMAKE_BUILD_TYPE=Release [-DCMAKE_INSTALL_PREFIX=/home/user/local/scip/installation]

The first flag is necessary, the second one is optional in order to install SCIP and SoPlex non-systemwide.

5. Compile and install SoPlex, SCIP, and its optional components:

.. code-block:: bash

    make && make install

6. Configure minizinc:

.. code-block:: bash

    cmake .. -DUSE_PROPRIETARY=on [-DCMAKE_PREFIX_PATH=/home/user/local/scip/installation] 

The optional prefix path variable is only necessary if you installed SCIP in a non-systemwide directory.

7. Compile Minizinc and enjoy SCIP as a solver.

If you have folders for SCIP and SoPlex separately, follow these steps:

.. code-block:: bash

  $ tar xvfz scipoptsuite-6.0.1.tgz
  $ cd scipoptsuite-6.0.1
  $ cd soplex
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make -j5
  $ cd ../scip
  $ mdkir build
  $ cd build
  $ cmake .. -DSOPLEX_DIR=~/Downloads/Software/scipoptsuite-6.0.1/soplex/build
  $ make -j5
  $ sudo make install                    ## Now MZN should find it
