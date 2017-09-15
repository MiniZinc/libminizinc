Installation
============

A complete installation of the MiniZinc system comprises the MiniZinc *compiler tool chain*, one or more *solvers*, and (optionally) the *MiniZinc IDE*. We provide fully self-contained binary packages for all major operating systems that contain all of these components. Alternatively, it is possible to compile all components from source code.

Binary Packages
---------------

The easiest way to get a full, working MiniZinc system is to use the **bundled binary packages**, available from http://www.minizinc.org/software.html.

The bundlded binary packages contain the compiler and IDE, as well as the following solvers: Gecode, Chuffed, COIN-OR CBC, and a Gurobi interface (the Gurobi library itself is not included). For backwards compatibility with older versions of MiniZinc, the packages also contain the now deprecated G12 suite of solvers (G12 fd, G12 lazy, G12 MIP).

Microsoft Windows
~~~~~~~~~~~~~~~~~

To install the bundled binary packages, simply download the installer, double-click to execute it, and follow the prompts. **Note:** you should select the 64 bit version of the installer if your Windows is a 64 bit operating system, otherwise pick the 32 bit version.

After installation is complete, you can find the MiniZinc IDE installed as a Windows application. The file extensions ``.mzn``, ``.dzn`` and ``.fzn`` are linked to the IDE, so double-clicking any MiniZinc file should open it in the IDE.

If you want to use MiniZinc from a command prompt, you need to add the installation directory (typically, this would be similar to ``C:\Program Files\MiniZinc 2.1.2 (bundled)``) to the PATH environment variable.

Linux
~~~~~

The MiniZinc bundled binary distribution for Linux is provided as an archive that contains everything that is needed to run MiniZinc. It was compiled on a Ubuntu 16.04 LTS system, but it bundles all required libraries except for the system C and C++ libraries (so it should be compatible with any Linux distribution that uses the same C and C++ libraries as Ubuntu 16.04). **Note:** you should select the 64 bit version of the installer if your Linux is a 64 bit operating system, otherwise pick the 32 bit version.

After downloading, uncompress the archive, for example in your home directory or any other location where you want to install it:

.. code-block:: bash

  $ tar xf MiniZincIDE-2.1.2-bundle-linux-x86_64.tgz

This will unpack MiniZinc into a directory that is called the same as the archive file (without the ``.tgz``). You can run the MiniZinc IDE or any of the command line tools directly from that directory, or add it to your PATH environment variable for easier access. **Note:** the MiniZinc IDE needs to be started using the ``MiniZincIDE.sh`` script, which sets up a number of paths that are required by the IDE.

Apple macOS
~~~~~~~~~~~

The macOS bundled binary distribution works with any version of OS X starting from 10.9. After downloading the disk image (``.dmg``) file, double click it if it doesn't open automatically. You will see an icon for the MiniZinc IDE that you can drag into your Applications folder (or anywhere else you want to install MiniZinc).

In order to use the MiniZinc tools from a terminal, you need to add the path to the MiniZinc installation to the PATH environment variable. If you installed the MiniZinc IDE in the standard Applications folder, the following command will add the correct path:

.. code-block:: bash

  $ export PATH=/Applications/MiniZincIDE.app/Contents/Resources:$PATH

Compilation from Source Code
----------------------------

All components of MiniZinc are free and open source software, and compilation should be straightforward if you have all the necessary build tools installed. However, third-party components, in particular the different solvers, may be more difficult to install correctly, and we cannot provide any support for these components.

The source code for MiniZinc can be downloaded from its GitHub repository at https://github.com/MiniZinc/libminizinc. The source code for the MiniZinc IDE is available from https://github.com/MiniZinc/MiniZincIDE.

You will also need to install additional solvers to use with MiniZinc. To get started, try Gecode (http://www.gecode.org) or Chuffed (https://github.com/chuffed/chuffed). We don't cover installation instructions for these solvers here.

Microsoft Windows
~~~~~~~~~~~~~~~~~

Required development tools:

- CMake, version 3.0.0 or later (http://cmake.org)
- Microsoft Visual C++ 2013 or later (e.g. the Community Edition available from https://www.visualstudio.com/de/downloads/)
- Optional, only needed for MiniZinc IDE: Qt toolkit, version 5.4 or later (http://qt.io)

**Compiling MiniZinc:** Unpack the source code (or clone the git repository). Open a command prompt and change into the source code directory. The following sequence of commands will build a 64 bit version of the MiniZinc compiler tool chain (you may need to adapt the ``cmake`` command to fit your version of Visual Studio):

.. code-block:: bash

  mkdir build
  cd build
  cmake -G"Visual Studio 14 2015 Win64" -DCMAKE_INSTALL_PREFIX="C:/Program Files/MiniZinc" ..
  cmake --build . --config Release --target install

This will install MiniZinc in the usual Program Files location. You can change where it gets installed by modifying the ``CMAKE_INSTALL_PREFIX``.

**Compiling the MiniZinc IDE:** Unpack the source code (or clone the git repository). Open a Visual Studio command prompt that matches the version of the Qt libraries installed on your system. Change into the source code directory for the MiniZinc IDE. Then use the following commands to compile:

.. code-block:: bash

  mkdir build
  cd build
  qmake ../MiniZincIDE
  nmake

Linux
~~~~~

Required development tools:

- CMake, version 3.0.0 or later
- A recent C++ compiler (g++ or clang)
- Optional, only needed for MiniZinc IDE: Qt toolkit, version 5.4 or later (http://qt.io)

**Compiling MiniZinc:** Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory. The following sequence of commands will build the MiniZinc compiler tool chain:

.. code-block:: bash

  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build .

**Compiling the MiniZinc IDE:** Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory for the MiniZinc IDE. Then use the following commands to compile:

.. code-block:: bash

  mkdir build
  cd build
  qmake ../MiniZincIDE
  make


Apple macOS
~~~~~~~~~~~

Required development tools:

- CMake, version 3.0.0 or later (from http://cmake.org or e.g. through homebrew)
- The Xcode developer tools
- Optional, only needed for MiniZinc IDE: Qt toolkit, version 5.4 or later (http://qt.io)

**Compiling MiniZinc:** Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory. The following sequence of commands will build the MiniZinc compiler tool chain:

.. code-block:: bash

  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build .

**Compiling the MiniZinc IDE:** Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory for the MiniZinc IDE. Then use the following commands to compile:

.. code-block:: bash

  mkdir build
  cd build
  qmake ../MiniZincIDE
  make



Adding Third-party Solvers
--------------------------

