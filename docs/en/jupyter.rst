Using MiniZinc in Jupyter Notebooks
===================================

You can use MiniZinc inside a Jupyter / IPython notebook using the ``iminizinc`` Python module. The module provides a "cell magic" extension that lets you solve MiniZinc models.

The module requires an existing installation of MiniZinc.

Installation
------------

You can install or upgrade this module via ``pip``:


.. code-block:: bash

    pip install -U iminizinc

Consult your Python documentation to find out if you need any extra options (e.g. you may want to use the ``--user`` flag to install only for the current user, or you may want to use virtual environments).

Make sure that the ``minizinc`` binary are on the ``PATH`` environment variable when you start the notebook server. The easiest way to do that is to get the "bundled installation" that includes the MiniZinc IDE and a few solvers, available from GitHub here: https://github.com/MiniZinc/MiniZincIDE/releases/latest
You then need to change your ``PATH`` environment variable to include the MiniZinc installation.

Basic usage
-----------

After installing the module, you have to load the extension using ``%load_ext iminizinc``. This will enable the cell magic ``%%minizinc``, which lets you solve MiniZinc models. Here is a simple example:

.. code::

    In[1]:  %load_ext iminizinc
            
    In[2]:  n=8
            
    In[3]:  %%minizinc
            
            include "globals.mzn";
            int: n;
            array[1..n] of var 1..n: queens;
            constraint all_different(queens);
            constraint all_different([queens[i]+i | i in 1..n]);
            constraint all_different([queens[i]-i | i in 1..n]);
            solve satisfy;
    Out[3]: {u'queens': [4, 2, 7, 3, 6, 8, 5, 1]}
            
As you can see, the model binds variables in the environment (in this case, ``n``) to MiniZinc parameters, and returns an object with fields for all declared decision variables.

Alternatively, you can bind the decision variables to Python variables:

.. code::

    In[1]:  %load_ext iminizinc
            
    In[2]:  n=8
            
    In[3]:  %%minizinc -m bind
            
            include "globals.mzn";
            int: n;
            array[1..n] of var 1..n: queens;
            constraint all_different(queens);
            constraint all_different([queens[i]+i | i in 1..n]);
            constraint all_different([queens[i]-i | i in 1..n]);
            solve satisfy;
            
    In[4]:  queens
    
    Out[4]: [4, 2, 7, 3, 6, 8, 5, 1]

If you want to find all solutions of a satisfaction problem, or all intermediate solutions of an optimisation problem, you can use the ``-a`` flag:

.. code::

    In[1]:  %load_ext iminizinc
            
    In[2]:  n=6
            
    In[3]:  %%minizinc -a
            
            include "globals.mzn";
            int: n;
            array[1..n] of var 1..n: queens;
            constraint all_different(queens);
            constraint all_different([queens[i]+i | i in 1..n]);
            constraint all_different([queens[i]-i | i in 1..n]);
            solve satisfy;
            
    Out[3]: [{u'queens': [5, 3, 1, 6, 4, 2]},
             {u'queens': [4, 1, 5, 2, 6, 3]},
             {u'queens': [3, 6, 2, 5, 1, 4]},
             {u'queens': [2, 4, 6, 1, 3, 5]}]

The magic supports a number of additional options, in particular loading MiniZinc models and data from files. Some of these may only work with the development version of MiniZinc (i.e., not the one that comes with the bundled binary releases). You can take a look at the help using

.. code::

    In[1]:  %%minizinc?
