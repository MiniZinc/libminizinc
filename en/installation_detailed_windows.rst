.. _ch-installation_detailed_windows:

Installation Instructions for Microsoft Windows
-----------------------------------------------

MiniZinc
~~~~~~~~

Required development tools:

- CMake, version 3.4.0 or later (https://cmake.org)
- Microsoft Visual C++ 2013 or later (e.g. the Community Edition available from https://www.visualstudio.com/de/downloads/)
- Optional, only needed for MiniZinc IDE: Qt toolkit, version 5.9 or later (http://qt.io)

Unpack the source code (or clone the git repository). Open a command prompt and change into the source code directory. The following sequence of commands will build a 64 bit version of the MiniZinc compiler tool chain (you may need to adapt the ``cmake`` command to fit your version of Visual Studio):

.. code-block:: bash

  mkdir build
  cd build
  cmake -G"Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX="C:/Program Files/MiniZinc" ..
  cmake --build . --config Release --target install

This will install MiniZinc in the usual Program Files location. You can change where it gets installed by modifying the ``CMAKE_INSTALL_PREFIX``.

MiniZinc IDE
~~~~~~~~~~~~

Required development tools:

- Microsoft Visual C++ 2013 or later (e.g. the Community Edition available from https://www.visualstudio.com/de/downloads/)
- Qt toolkit, version 5.9 or later (http://qt.io)

Unpack the source code (or clone the git repository). Open a Visual Studio command prompt that matches the version of the Qt libraries installed on your system. Change into the source code directory for the MiniZinc IDE. Then use the following commands to compile:

.. code-block:: bash

  mkdir build
  cd build
  qmake ../MiniZincIDE
  nmake

Gecode
~~~~~~

You can obtain the Gecode source code from GitHub (https://github.com/gecode/gecode). You can find detailed compilation instructions for Gecode on its web site (https://www.gecode.org). Gecode's build system relies on some Unix tools, so you may have to install an environment such as Cygwin. In short, to compile from source, run the following in a terminal from within the Gecode source code directory, where you replace <INSTALLATION_PREFIX> with the path where you want Gecode to be installed:

.. code-block:: bash

  mkdir build
  cd build
  ../configure --prefix=<INSTALLATION_PREFIX>
  make -j8
  make install

After installing Gecode, you can compile MiniZinc with built-in support for Gecode, which enables extended pre-solving and solution checking. In order for the MiniZinc compilation to find your Gecode installation, you have to use the option ``-DGECODE_ROOT=<INSTALLATION_PREFIX>`` when calling ``cmake``.

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


Chuffed
~~~~~~~

You can obtain Chuffed's source from https://github.com/chuffed/chuffed. You can compile and install it using the following commands:

.. code-block:: bash

  mkdir build
  cd build
  cmake .. -DCMAKE_INSTALL_PREFIX=<INSTALLATION_PREFIX>
  cmake --build .
  cmake --build . --target install

This will install Chuffed in the installation directory specified as ``<INSTALLATION_PREFIX>``.

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

These instructions apply to CBC versions 2.10/stable or newer. Thanks to David Catteeuw for help with building the parallel version of CBC.

1. OBTAIN AND BUILD CBC

  CBC is on github, but has many dependencies: https://github.com/coin-or/Cbc

  AMPL provides a CBC binary and has the entire project with dependencies on github: https://github.com/ampl/coin

  Building Couenne fails, but we don't need it. Remove couenne, ipopt, and bonmin directories.
  
  Install zlib and bzip2, e.g., using vcpkg. Its CMake integration does not work as of vcpkg 2018.11.23 so you need to use its install folder manually,
  e.g., by adding the following to CBC's CMakeLists.txt:
  
    ::
     
       include_directories(...vcpkg/installed/x86-windows/include)
       link_libraries(...vcpkg/installed/x86-windows/lib/zlib.lib)
       link_libraries(...vcpkg/installed/x86-windows/lib/bz2.lib)
   
  Then 

  .. code-block:: bash
  
    $ cd C:\dev
    $ git clone https://github.com/ampl/coin.git
    $ cd coin
    $ rmdir /s Bonmin
    $ rmdir /s Couenne
    $ rmdir /s Ipopt
    $ mkdir build
    $ cd build
    $ cmake .. -G "Visual Studio 15 2017" -A x64 -DCMAKE_INSTALL_PREFIX=C:\dev\Cbc_install
    $ cmake --build . --target install
 
  => successfully builds ``Cbc.lib``, debug version in ``C:\dev\coin\build\Debug``.


2. BUILD MINIZINC WITH CBC

  Copy ``C:\dev\coin\build\Debug\*.lib`` to ``C:\dev\Cbc_install\lib``

  Configure MiniZinc's CMake with ``-DOSICBC_ROOT=C:\dev\cbc_install``.
