.. _ch-installation:

Installation
============

A complete installation of the MiniZinc system comprises the MiniZinc *compiler tool chain*, one or more *solvers*, and (optionally) the *MiniZinc IDE*. 

The easiest way to get a full, working MiniZinc system is to use the **bundled binary packages**, available from https://www.minizinc.org/downloads/.

The bundled binary packages contain the compiler and IDE, as well as the following solvers: Gecode, Chuffed, COIN-OR CBC, HiGHS, and OR-Tools CP-SAT.
Interfaces to Gurobi, CPLEX, SCIP and XPress are also included and can be used if the corresponding solvers are installed.

Alternatively, it is possible to compile all components from source code, as described in :ref:`ch-installation_detailed`.

Microsoft Windows
-----------------

To install the bundled binary packages, simply download the installer, double-click to execute it, and follow the prompts. **Note:** 32 bit Windows installations are no longer supported in the bundled binary packages.

After installation is complete, you can find the MiniZinc IDE installed as a Windows application. The file extensions ``.mzn``, ``.dzn`` and ``.fzn`` are linked to the IDE, so double-clicking any MiniZinc file should open it in the IDE.

If you want to use MiniZinc from a command prompt, you need to add the installation directory to the PATH environment variable. In a Windows command prompt you could use the following command:

.. parsed-literal::

 C:\\>setx PATH "%PATH%;C:\\Program Files\\MiniZinc |release| (bundled)\\"

Linux
-----

The MiniZinc bundled binary distribution for Linux is provided using three different distribution methods: *Snap*, *AppImage*, and an *archive* containing all necessary files. The capabilities of each package is the same, but it depends on your linux distribution which method is the easiest to use.
**Note:** linux bundles are compiled to be compatible with many linux distributions using a *x86_64* architecture, but some dependencies on low level libraries cannot be avoided. Make use of the MiniZinc issue trackers if you encounter any problems.

Snap
~~~~

The MiniZinc Snap is the easiest way to install the MiniZinc bundle if Snap (https://snapcraft.io/) is available on your system. The installation will provide the ``minizinc`` command within terminal environments and will add an application entry for the MiniZinc IDE. The IDE can be started from the command line using ``minizinc.ide``. The following command will commence the installation the MiniZinc snap:

.. code-block:: bash

  $ snap install minizinc --classic

**Note:** the MiniZinc snap requires the *classic* permission model to access solvers that are installed somewhere else on your system.

An advantage of using the MiniZinc snap is that it will be automatically updated whenever a new version of the snap is available. Using snap channels it is also possible to automatically follow the developmental versions of MiniZinc. More information about the MiniZinc snap can be found in the Snap Store.

.. only:: builder_html

  .. image:: https://snapcraft.io/static/images/badges/en/snap-store-black.svg
    :target: https://snapcraft.io/minizinc
    :alt: MiniZinc in the snap store
    :align: center

AppImage
~~~~~~~~

The MiniZinc AppImage provides a way of installing the MiniZinc bundles without any added requirements. AppImages (https://appimage.org/) provide self-contained versions of applications with all their required dependencies. To use the MiniZinc AppImage you simply download the AppImage, make it executable, and run:

.. parsed-literal::

  $ chmod +x MiniZincIDE-|release|-x86_64.AppImage
  $ ./MiniZincIDE-|release|-x86_64.AppImage

AppImages have no standard integration with the desktop environment. A script has been added to the MiniZinc AppImage to integrate the application into both the terminal and the desktop environment. The one requirement for this script is that the AppImage *CANNOT be moved* after the install script has been run. A suggestion would be to store the AppImages in ``~/Applications/`` or ``/opt/Applications/``. The following commands move the AppImage to the second location and install ``minizinc`` and ``MiniZincIDE`` to ``/usr/local/bin`` and will add a desktop entry and its corresponding icon to ``$HOME/.local/share/{applications,icons}``:

.. parsed-literal::

  $ mv ./MiniZincIDE-|release|-x86_64.AppImage /opt/Applications/
  $ /opt/Applications/MiniZincIDE-|release|-x86_64.AppImage install

**Note:** The installation locations can be changed using the environmental variables ``$BIN_LOCATION`` and ``$DESKTOP_LOCATION``. The default behaviour can be achieved using the following command:

.. parsed-literal::

  $ BIN_LOCATION=/usr/local/bin DESKTOP_LOCATION=$HOME/.local/share ./MiniZincIDE-|release|-x86_64.AppImage install

Archive
~~~~~~~

The MiniZinc archive bundle is straightforward, but doesn't offer any automatic integrations with the desktop environment. After downloading, uncompress the archive, for example in your home directory or any other location where you want to install it:

.. parsed-literal::

  $ tar xf MiniZincIDE-|release|-bundle-linux-x86_64.tgz

This will unpack MiniZinc into a directory that is called the same as the archive file (without the ``.tgz``). You can start the MiniZinc IDE using the ``MiniZincIDE.sh`` shell script, which sets up a number of paths and environment variables.

In order to use the command line tools, after unpacking, make the following changes to some environment variables (assuming a bash-like shell):

.. parsed-literal::

  $ export PATH=MiniZincIDE-|release|-bundle-linux-x86_64/bin:$PATH
  $ export LD_LIBRARY_PATH=MiniZincIDE-|release|-bundle-linux-x86_64/lib:$LD_LIBRARY_PATH
  $ export QT_PLUGIN_PATH=MiniZincIDE-|release|-bundle-linux-x86_64/plugins:$QT_PLUGIN_PATH

Apple macOS
-----------

The macOS bundled binary distribution works with any version of OS X starting from 10.9. After downloading the disk image (``.dmg``) file, double click it if it doesn't open automatically. You will see an icon for the MiniZinc IDE that you can drag into your Applications folder (or anywhere else you want to install MiniZinc).

In order to use the MiniZinc tools from a terminal, you need to add the path to the MiniZinc installation to the PATH environment variable. If you installed the MiniZinc IDE in the standard Applications folder, the following command will add the correct path:

.. code-block:: bash

  $ export PATH=/Applications/MiniZincIDE.app/Contents/Resources:$PATH

Docker
------

Container images are published to the GitHub Container Registry at
``ghcr.io/minizinc/minizinc``. They contain the compiler tool chain and the
bundled solvers, but not the IDE.

To solve a model with the image directly:

.. code-block:: bash

  docker run --rm -v "$PWD:/work" -w /work ghcr.io/minizinc/minizinc model.mzn data.dzn

Adding MiniZinc to your own image
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``-dist`` images contain nothing but the MiniZinc installation: no operating
system, no C library, no shell. They are not runnable on their own, and exist to
be copied into an image you already maintain:

.. parsed-literal::

  FROM python:3.12-slim
  COPY --from=ghcr.io/minizinc/minizinc:\ |release|\ -dist /opt/minizinc /opt/minizinc
  ENV PATH=/opt/minizinc/bin:$PATH LD_LIBRARY_PATH=/opt/minizinc/lib

This is the recommended way to use MiniZinc in a larger image. Because the layer
carries no operating system of its own, it never needs security updates: the base
image remains yours, and you update it on your own schedule.

Use ``-musl-dist`` instead if your base uses musl rather than glibc, for example
Alpine Linux, which also needs ``apk add libstdc++``.

Available images
~~~~~~~~~~~~~~~~

Every image is available for ``linux/amd64`` and ``linux/arm64``.

=========================  =================================================
Tag suffix                 Contents
=========================  =================================================
*(none)*                   Runnable, glibc. Based on Google's ``distroless/cc``
``-musl``                  Runnable, musl
``-dist``                  MiniZinc only, for ``COPY --from``, glibc build
``-musl-dist``             MiniZinc only, for ``COPY --from``, musl build
=========================  =================================================

Versions are tagged as ``2.10.1``, ``2.10``, ``2`` and ``latest``, each pointing
at the newest release in its series, plus ``edge`` for the current development
build. Combine the two, for example ``ghcr.io/minizinc/minizinc:2.10-musl-dist``.

The runnable images are rebuilt weekly so that fixes to the underlying base image
are picked up; the MiniZinc version they contain does not change, but the image
digest does. If you need a byte-for-byte reproducible build, refer to an image by
digest rather than by tag:

.. code-block:: bash

  docker pull ghcr.io/minizinc/minizinc@sha256:...

Adding Third-party Solvers
--------------------------

Third party solvers for MiniZinc typically consist of two parts: a solver *executable*, and a solver-specific MiniZinc *library*. MiniZinc must be aware of the location of both the executable and the library in order to compile and run a model with that solver. Each solver therefore needs to provide a *configuration file* in a location where the MiniZinc toolchain can find it.

The easiest way to add a solver to the MiniZinc system is via the MiniZinc IDE. This is explained in :numref:`sec-ide-add-solvers`. You can also add configuration files manually, as explained in :numref:`sec-cmdline-conffiles`.
