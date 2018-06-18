First steps with the MiniZinc IDE
=================================

The MiniZinc IDE provides a simple interface to most of MiniZinc's functionality. It lets you edit model and data files, solve them with any of the solvers supported by MiniZinc, run debugging and profiling tools, and submit solutions to online courses (such as the MiniZinc Coursera courses).

We recommend using the bundled binary distribution of MiniZinc introduced in :numref:`ch-installation`.

When you open the MiniZinc IDE for the first time, it will ask you whether you want to be notified when an update is available. If you installed the IDE from sources, it may next ask you to locate your installation of the MiniZinc compiler. Please refer to :numref:`sec-ide-config` for more details on this.

The IDE will then greet you with the *MiniZinc Playground*, a window that will look like this:

.. figure:: figures/mzn-ide-playground.png
  :width: 600px

You can start writing your first MiniZinc model! Let's try something very simple:

.. figure:: figures/mzn-ide-playground2.png
  :width: 600px

In order to solve the model, you click on the *Run* button in the toolbar, or use the keyboard shortcut *Ctrl+R* (or *command+R* on macOS):

.. figure:: figures/mzn-ide-playground3.png
  :width: 600px

As you can see, an output window pops up that displays a solution to the problem you entered.
Let us now try a model that requires some additional data.

.. figure:: figures/mzn-ide-playground4.png
  :width: 600px

When you run this model, the IDE will ask you to enter a value for the parameter *n*:

.. figure:: figures/mzn-ide-playground-param.png
  :width: 300px

After entering, for example, the value 4 and clicking *Ok*, the solver will execute the model for *n=4*:

.. figure:: figures/mzn-ide-playground5.png
  :width: 600px

Alternatively, data can also come from a file. Let's create a new file with the data and save it as ``data.dzn``:

.. figure:: figures/mzn-ide-data.png
  :width: 600px

When you now go back to the *Playground* tab and click *Run*, the IDE will give you the option to select a data file:

.. figure:: figures/mzn-ide-select-data.png
  :width: 300px

Click on the ``data.dzn`` entry, then on *Ok*, and the model will be run with the given data file:

.. figure:: figures/mzn-ide-playground-data.png
  :width: 600px

Of course you can save your model to a file, and load it from a file, and the editor supports the usual functionality.

This should be enough to get you started with the MiniZinc tutorial in :numref:`part-tutorial`. If you want to know more about the MiniZinc IDE, continue reading from :numref:`ch-ide`.
