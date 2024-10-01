# README #

This directory contains the MiniZinc documentation.

### Building the docs ###

You need the following tools to build the documentation:

* Python 3
* GNU make
* Sphinx version 7.2. Install it using the command
  `pip install sphinx`
* Sphinx Read The Docs html theme. Install it using the command
  `pip install sphinx_rtd_theme`
* 404 Page generator `pip install sphinx-notfound-page`
* If you want to build the PDF documentation, you also need a LaTeX
  distribution that includes xetex, and install the following fonts: Noto Serif,
  Lato and Inconsolata.

To build the HTML documentation, simply run `make html`. To build the PDF, run `make latexpdf`.

### Including the MiniZinc library reference documentation ###

The reference documentation for the MiniZinc library can be generated
automatically from the source code. Then run the following command:

``mzn2doc --rst-output --include-stdlib --output-base en ../share/minizinc/std/globals.mzn``

