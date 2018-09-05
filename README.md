# README #

This repository contains the MiniZinc documentation.

### Building the docs ###

You need the following tools to build the documentation:

* Python 3
* GNU make
* Sphinx version 1.8.0. Install it using the command
  `pip install git+https://github.com/sphinx-doc/sphinx`
* Sphinx Read The Docs html theme. Install it using the command
  `pip install sphinx_rtd_theme`
* If you want to build the PDF documentation, you also need a LaTeX distribution that includes xetex, and install the following fonts: Charter, Lato and Inconsolata.

To build the HTML documentation, simply run `make html`. To build the PDF, run `make latexpdf`.

### Including the MiniZinc library reference documentation ###

The reference documentation for the MiniZinc library can be generated automatically from the source code.
For this to work, you need an installation of `libminizinc` (either from a binary distribution or from sources, see http://www.minizinc.org).
Them run the following command:

``mzn2doc --rst-output --include-stdlib --output-base $MINIZINC_DOC_SOURCE_DIR/en $LIBMINIZINC_INSTALL_DIR/share/minizinc/std/globals.mzn``

Where `$MINIZINC_DOC_SOURCE_DIR` is your local copy of the MiniZinc documentation repository (the `en` directory should contain the `index.rst` file),
and `$LIBMINIZINC_INSTALL_DIR` is the directory where MiniZinc is installed.
