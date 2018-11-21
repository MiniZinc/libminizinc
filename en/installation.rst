.. _ch-installation:

Installation
============

A complete installation of the MiniZinc system comprises the MiniZinc *compiler tool chain*, one or more *solvers*, and (optionally) the *MiniZinc IDE*. We provide fully self-contained binary packages for all major operating systems that contain all of these components. Alternatively, it is possible to compile all components from source code.

Binary Packages
---------------

The easiest way to get a full, working MiniZinc system is to use the **bundled binary packages**, available from http://www.minizinc.org/software.html.

The bundlded binary packages contain the compiler and IDE, as well as the following solvers: Gecode, Chuffed, COIN-OR CBC, and a Gurobi interface (the Gurobi library itself is not included). For backwards compatibility with older versions of MiniZinc, the packages also contain the now deprecated G12 suite of solvers (G12 fd, G12 lazy, G12 MIP).

Microsoft Windows
~~~~~~~~~~~~~~~~~

To install the bundled binary packages, simply download the installer, double-click to execute it, and follow the prompts. **Note:** 32 bit Windows installations are no longer supported in the bundled binary packages.

After installation is complete, you can find the MiniZinc IDE installed as a Windows application. The file extensions ``.mzn``, ``.dzn`` and ``.fzn`` are linked to the IDE, so double-clicking any MiniZinc file should open it in the IDE.

If you want to use MiniZinc from a command prompt, you need to add the installation directory to the PATH environment variable. In a Windows command prompt you could use the following command:

.. parsed-literal::

 C:\\>setx PATH "%PATH%;C:\\Program Files\\MiniZinc |release| (bundled)\\"

Linux
~~~~~

The MiniZinc bundled binary distribution for Linux is provided using three different distribution methods: *Snap*, *AppImage*, and an *archive* containing all necessary files. The capabilities of each package is the same, but it depends on your linux distribution which method is the easiest to use.
**Note:** linux bundles are compiled to be compatible with many linux distributions using a *x86_64* architecture, but some dependencies on low level libraries cannot be avoided. Make use of the MiniZinc issue trackers if you encounter any problems.

Snap
^^^^

The MiniZinc Snap is the easiest way to install the MiniZinc bundle if Snap (https://snapcraft.io/) is available on you system. The installation will provide the ``minizinc`` command within terminal environments and will add an application entry for the MiniZincIDE. The IDE can be started from the command line using ``minizinc.ide``. The following command will commence the installation the MiniZinc snap:

.. parsed-literal::

  $ snap install minizinc --classic

**Note:** the MiniZinc snap requires the *classic* permission model to access solvers that are installed somewhere else on your system.

An advantage of using the MiniZinc snap is that it will be automatically updated whenever a new version of the snap is available. Using snap channels it is also possible to automatically follow the developmental versions of MiniZinc. More information about the MiniZinc snap can be found in the Snap Store:

.. image:: https://snapcraft.io/static/images/badges/en/snap-store-black.svg
   :target: https://snapcraft.io/minizinc
   :alt: MiniZinc in the snap store
   :align: center

AppImage
^^^^^^^^

The MiniZinc AppImage provides a way of installing the MiniZinc bundles without any added requirements. AppImages (https://appimage.org/) provide self-contained versions of applications with all their required dependencies. To use the MiniZinc AppImage you simply download the AppImage, make it executable, and run:

.. parsed-literal::

  $ chmod +x MiniZincIDE-|release|-x86_64.AppImage
  $ ./MiniZincIDE-|release|-x86_64.AppImage

AppImages have no standard integration with the desktop environment. A script has been added to the MiniZinc AppImage to integrate the application into both the terminal and the desktop environment. The one requirement for this script is that the AppImage *CANNOT be moved* after the install script has been run. A suggestion would be to store the AppImages in ``~/Applications/`` or ``/opt/Applications/``. The following commands move the AppImage to the second location and install ``minizinc`` and ``MiniZincIDE`` to ``/usr/local/bin`` and will add a desktop entry and its corresponding icon to ``$HOME/.local/share/{applications,icons}``:

.. parsed-literal::

  $ mv ./MiniZincIDE-|release|-x86_64.AppImage /opt/Applications/
  $ /opt/Applications/MiniZincIDE-|release|-x86_64.AppImage install

**Note:** to change the installation locations can be changed using the environmental variables ``$BIN_LOCATION`` and ``$DESKTOP_LOCATION``. The default behaviour can be achieved using the following command:

.. parsed-literal::

  $ BIN_LOCATION=/usr/local/bin DESKTOP_LOCATION=$HOME/.local/share ./MiniZincIDE-|release|-x86_64.AppImage install

Archive
^^^^^^^

The MiniZinc archive bundle is straightforward, but doesn't offer any automatic integrations with the desktop environment. After downloading, uncompress the archive, for example in your home directory or any other location where you want to install it:

.. parsed-literal::

  $ tar xf MiniZincIDE-|release|-bundle-linux-x86_64.tgz

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

Third party solvers for MiniZinc typically consist of two parts: a solver *executable*, and a solver-specific MiniZinc *library*. MiniZinc must be aware of the location of both the executable and the library in order to compile and run a model with that solver. Each solver therefore needs to provide a *configuration file* in a location where the MiniZinc toolchain can find it.

The easiest way to add a solver to the MiniZinc system is via the MiniZinc IDE. This is explained in :numref:`sec-ide-add-solvers`. You can also add configuration files manually, as explained in :numref:`sec-cmdline-conffiles`.

