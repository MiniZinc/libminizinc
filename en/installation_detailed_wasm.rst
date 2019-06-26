.. _ch-installation_detailed_wasm:

Installation Instructions for Webassembly / JavaScript
------------------------------------------------------

Required development tools:

- CMake, version 3.4.0 or later (from https://cmake.org or e.g. through homebrew)
- emscripten sdk (from https://emscripten.org)

**Compiling MiniZinc:** Unpack the source code (or clone the git repository). Open a terminal and change into the source code directory. Make sure that the emscripten sdk is on your PATH, e.g. by sourcing the ``emsdk_env.sh`` script (see emscripten documentation). The following sequence of commands will build the MiniZinc compiler tool chain:

.. code-block:: bash

  mkdir build
  cd build
  emconfigure cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
  cmake --build .

Webassemble/JavaScript support is currently experimental.
