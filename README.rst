=====
plfit
=====
-------------------------------------------------
Fitting power-law distributions to empirical data
-------------------------------------------------

:Author: Tamas Nepusz

This program fits power-law distributions to empirical (discrete or
continuous) data, according to the method of Clauset, Shalizi and
Newman [1]_.

Requirements
------------

You only need a recent C compiler (e.g., ``gcc``), CMake_ and ``make`` to
compile the main executable. If you also want to compile the Python module with
the same functionality, you will also need SWIG_.

.. _SWIG: http://www.swig.org
.. _CMake: http://www.cmake.org

It is possible (although a bit more complicated) to compile the program
and the Python module without CMake_ or ``make``; see the
`Compiling without CMake or make`_ section below.

Compilation
-----------

Compiling with CMake
^^^^^^^^^^^^^^^^^^^^

This is easy. Assuming that you have downloaded the source code already,
just do this from the command line::

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

If you have permission to write in ``/usr/local``, you can also invoke
``make install`` at this point, which will place the executable in
``/usr/local/bin``. Adjusting ``CMAKE_INSTALL_PREFIX`` using ``ccmake``
will let you install ``plfit`` in a different folder.

The above code snippet compiles the main executable (``plfit``) but not
the Python module. If you need the Python module as well, the dance goes
like this::

    $ mkdir build
    $ cd build
    $ cmake ..
    $ ccmake .
    [ccmake starts up; set the PLFIT_COMPILE_PYTHON_MODULE variable to ON]
    $ make

Compiling without CMake or make
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

I like to use a separate build folder for compiling stuff, so we start
again by creating one::

    $ mkdir build
    $ cd build

Now we simply compile all the C files into a single executable using
``gcc``::

    $ gcc -o plfit -I../src -lm ../src/*.c

To build the Python interface as well, some extra legwork is needed
with SWIG::

    $ swig -Wall -O -python -outcurrentdir ../src/plfit.i
    $ gcc -shared -fPIC -o _plfit.so -I../src -I/usr/include/python2.7 \
      -lm plfit_wrap.c ../src/*.c

where ``/usr/include/python2.7`` should be replaced with the directory
where the ``Python.h`` file of your Python interpreter is.

That's it. The Python interface itself consists of two files:
``plfit.py`` and ``_plfit.so``. Both of them are to be found in the
``build`` folder.

Usage
-----

From the command line
^^^^^^^^^^^^^^^^^^^^^

Prepare a data file consisting of your input data, one number (i.e. sample)
per line. So, for instance, if you want to find out whether the degrees
of some graph follow a power-law distribution or not, just dump the
degrees into the input file, one vertex per line. If you are interested
in whether the magnitude of earthquakes follow a power-law, dump the
magnitudes into the input file, one earthquake per line. There is no
need to calculate the cumulative distribution function or the probability
density function before, nor to normalize the input data. Then simply run::

    $ ./plfit input_data.txt

For some inspiration, see the sample data files under the directory ``test``.
The program will assume that the data is discrete when the file contains
integers only; otherwise it will assume that the data is continuous. If your
data file contains integers only but you know that they come from a continuous
distribution, use ``-c`` to force continuous fitting::

    $ ./plfit -c input_data.txt

The ``-h`` switch gives you a nice help message::

    $ ./plfit -h

From Python
^^^^^^^^^^^

The Python module is just a thin wrapper around the original C calls,
so take a look at ``plfit.h`` and use your instincts to figure out
how the corresponding Python functions are named ;) This should
get you started::

    >>> import plfit
    >>> data = [float(line) for line in open("input_file.txt")]
    >>> plfit.plfit_discrete(data)
    >>> plfit.alpha, plfit.xmin, plfit.L

You can also invoke ``dir(plfit)`` to examine the namespace of the
``plfit`` module, listing all the functions and data types.
There's also a test script in ``test/test_python_module.py``.

References
----------

.. [1] Clauset A, Shalizi CR and Newman MEJ: Power-law distributions
       in empirical data. SIAM Review 51, 661-703 (2009).
